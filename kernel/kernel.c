#include <aurora.h>
#include "../user-programs/include/aurora_syscall.h"

AURORA_KERNEL_PROGRAM;

/* ROM reset PC is 0.  Module-level assembly is emitted before compiler
 * functions, so keep a permanent first-word jump to the C runtime entry even
 * when optimization or helper functions change function emission order. */
__asm__(
    ".globl __aurora_rom_entry\n"
    "__aurora_rom_entry:\n"
    "  jmp _start\n");

enum {
  AURORA_UROM_BEGIN = 0x80000000u,
  AURORA_UROM_END = 0x81000000u,
  AURORA_URAM_BEGIN = 0x81000000u,
  AURORA_URAM_END = 0x82000000u,
  AURORA_TICK_DEVICE = 0x07000000u,
  /* Read-only kernel device: system uptime in milliseconds modulo 2^32. */
  AURORA_TIMER_DEVICE = 0x08000000u,
  AURORA_USER_KERNEL_STACK = 0x02010000u,
  AURORA_CALLBACK_KERNEL_STACK_0 = 0x02020000u,
  AURORA_CALLBACK_KERNEL_STACK_1 = 0x02030000u,
  AURORA_INTERRUPT_FRAME_WORDS = 18u,
  AURORA_CALLBACK_RESTORE_MAGIC = 0xA32CB007u,
  KEYBOARD_QUEUE_SIZE = 32u,
  PROCESS_SYSTEM = 0u,
  PROCESS_USER = 1u,
  PROCESS_COUNT = 2u,
};

typedef struct aurora_process_context {
  aurora_word_t r[16];
  aurora_word_t pc;
  aurora_word_t flags;
  aurora_word_t ksp;
  aurora_word_t valid;
  aurora_word_t subscribed_vector;
  aurora_keyboard_callback_t callback;
  aurora_word_t callback_trampoline;
  aurora_word_t callback_restore_ksp;
  aurora_word_t callback_active;
  aurora_word_t callback_stack_bank;
} aurora_process_context;

/* Kernel-resident state.  It is intentionally private to kernel RAM. */
static volatile aurora_word_t keyboard_queue[KEYBOARD_QUEUE_SIZE];
static volatile aurora_word_t keyboard_read;
static volatile aurora_word_t keyboard_write;
static volatile aurora_word_t keyboard_irq_sequence;
/* Volatile prevents the generic optimizer from replacing x * 256 with SFT;
 * this CPU's physical signed shift-count convention is target-specific. */
static volatile aurora_word_t keyboard_event_radix = 256u;
static volatile aurora_word_t user_fault_vector;
static volatile aurora_word_t user_exit_code;
static volatile aurora_process_context process_table[PROCESS_COUNT];
static volatile aurora_word_t current_process;

extern void aurora_syscall_entry(void);
void aurora_tick_irq(void);
void aurora_keyboard_irq(void);
__attribute__((interrupt)) void aurora_user_memory_fault(void);
__attribute__((interrupt)) void aurora_user_illegal_instruction(void);

/* Keep the range checks in comparison form.  Aurora's old constant-AND
 * selector can otherwise spend unbounded time on the optimized /24 test. */
__attribute__((noinline, optnone)) static int
is_user_code_address(aurora_word_t address) {
  if (address < AURORA_UROM_BEGIN)
    return 0;
  if (address >= AURORA_UROM_END)
    return 0;
  return 1;
}

__attribute__((noinline, optnone)) static int
is_user_read_address(aurora_word_t address) {
  if (address >= AURORA_UROM_BEGIN) {
    if (address < AURORA_UROM_END)
      return 1;
  }
  if (address >= AURORA_URAM_BEGIN) {
    if (address < AURORA_URAM_END)
      return 1;
  }
  return 0;
}

static void kernel_console_put_int(aurora_word_t bits) {
  aurora_word_t digits[10];
  aurora_word_t count = 0u;
  aurora_word_t magnitude = bits;

  if ((bits & 0x80000000u) != 0u) {
    aurora.io.PutChar('-');
    magnitude = 0u - bits;
  }
  if (magnitude == 0u) {
    aurora.io.PutChar('0');
    return;
  }
  while (magnitude != 0u) {
    digits[count++] = magnitude % 10u;
    magnitude /= 10u;
  }
  while (count != 0u)
    aurora.io.PutChar('0' + digits[--count]);
}

/* Boot messages use kernel-resident string literals, so they must not pass
 * through the user-pointer validation used by the PUT_STRING system call. */
static void kernel_boot_put_string(const char *text) {
  while (*text != '\0') {
    aurora.io.PutChar((aurora_word_t)*text);
    ++text;
  }
}

static void kernel_status_prefix(void) {
  aurora_word_t milliseconds = aurora.Read(AURORA_TIMER_DEVICE);

  aurora.io.PutChar('[');
  kernel_console_put_int(milliseconds);
  kernel_boot_put_string(" ms");
  aurora.io.PutChar(']');
  aurora.io.PutChar(' ');
}

static void kernel_boot_status(const char *message) {
  kernel_status_prefix();
  kernel_boot_put_string(message);
  aurora.io.PutChar('\n');
}

static void mark_user_not_runnable(void) {
  volatile aurora_process_context *user = &process_table[PROCESS_USER];

  user->valid = 0u;
  user->subscribed_vector = 0u;
  user->callback = (aurora_keyboard_callback_t)0;
  user->callback_trampoline = 0u;
  user->callback_restore_ksp = 0u;
  user->callback_active = 0u;
}

static void kernel_fault_spacing(void) {
  aurora.io.PutChar('\n');
  aurora.io.PutChar('\n');
  aurora.io.PutChar('\n');
}

__attribute__((noinline, noreturn)) static void
halt_after_user_fault(const char *message) {
  /* No runnable user process remains in the current fixed two-process model. */
  mark_user_not_runnable();

  kernel_fault_spacing();
  kernel_boot_status(message);
  kernel_boot_status("Nothing to do. Halt.");
  aurora.Halt();
}

__attribute__((noinline, noreturn)) static void
halt_after_user_exit(aurora_word_t exit_code) {
  user_exit_code = exit_code;
  mark_user_not_runnable();

  kernel_fault_spacing();
  kernel_status_prefix();
  kernel_boot_put_string("USER_PROCESS exited with code ");
  kernel_console_put_int(exit_code);
  kernel_boot_put_string(".\n");
  kernel_boot_status("Nothing to do. Halt.");
  aurora.Halt();
}

static aurora_status_t kernel_console_put_string(aurora_word_t address) {
  if (!is_user_read_address(address))
    return AURORA_EFAULT;

  /* Bound unterminated input so a bad user pointer cannot monopolize kernel. */
  for (aurora_word_t length = 0u; length < 4096u; ++length, ++address) {
    if (!is_user_read_address(address))
      return AURORA_EFAULT;
    aurora_word_t character = aurora.Read(address);
    if (character == 0u)
      return AURORA_OK;
    aurora.io.PutChar(character & 0xffu);
  }
  return AURORA_EINVAL;
}

__attribute__((noinline)) static void
enqueue_keyboard_word(aurora_word_t character) {
  aurora_word_t next = (keyboard_write + 1u) % KEYBOARD_QUEUE_SIZE;
  /* Drop the newest word when full; this never writes user memory in IRQ. */
  if (next == keyboard_read)
    return;
  keyboard_queue[keyboard_write] = character;
  keyboard_write = next;
}

__attribute__((noinline)) static int
dequeue_keyboard_word(aurora_word_t *character) {
  if (keyboard_read == keyboard_write)
    return 0;
  *character = keyboard_queue[keyboard_read];
  keyboard_read = (keyboard_read + 1u) % KEYBOARD_QUEUE_SIZE;
  return 1;
}

/*
 * Snapshot a suspended user frame in the PCB, copy it to the dedicated
 * callback KSP, replace its PC and r0, and put the return trampoline on USP.
 * CALLBACK_RETURN reconstructs from the PCB instead of trusting a retained
 * KSP memory frame across the whole user callback.
 */
__attribute__((noinline, optnone)) static aurora_word_t
prepare_user_callback(aurora_word_t source_stack_top,
                      aurora_word_t event_word) {
  volatile aurora_process_context *user = &process_table[PROCESS_USER];
  aurora_word_t callback =
      (aurora_word_t)(unsigned long)user->callback;
  if (user->callback_active ||
      user->subscribed_vector != AURORA_IVT_KEYBOARD ||
      !is_user_code_address(callback) ||
      (user->callback_trampoline != 0u &&
       !is_user_code_address(user->callback_trampoline)) ||
      source_stack_top < AURORA_INTERRUPT_FRAME_WORDS)
    return 0u;

  aurora_word_t source = source_stack_top - AURORA_INTERRUPT_FRAME_WORDS;
  user->flags = *(volatile aurora_word_t *)(unsigned long)(source + 0u);
  user->pc = *(volatile aurora_word_t *)(unsigned long)(source + 1u);
  for (aurora_word_t i = 0; i < 16u; ++i)
    user->r[i] =
        *(volatile aurora_word_t *)(unsigned long)(source + 2u + i);

  /* Alternating banks matter when CALLBACK_RETURN immediately chains a
   * queued event: that syscall is still executing on the previous bank. */
  aurora_word_t bank;
  aurora_word_t target;
  if (user->callback_stack_bank == 0u) {
    bank = 1u;
    target = AURORA_CALLBACK_KERNEL_STACK_1;
  } else {
    bank = 0u;
    target = AURORA_CALLBACK_KERNEL_STACK_0;
  }
  for (aurora_word_t i = 0; i < AURORA_INTERRUPT_FRAME_WORDS; ++i) {
    aurora_word_t value =
        *(volatile aurora_word_t *)(unsigned long)(source + i);
    *(volatile aurora_word_t *)(unsigned long)(target + i) = value;
  }

  /* Frame layout is FLAGS, PC, r0 ... r15. */
  *(volatile aurora_word_t *)(unsigned long)(target + 1u) = callback;
  /* r0 carries the character in bits 7:0, the hardware-IRQ identity in
   * bits 23:8, and the event signature in bits 31:24. */
  *(volatile aurora_word_t *)(unsigned long)(target + 2u) = event_word;
  user->callback_restore_ksp = source_stack_top;
  user->callback_active = 1u;
  user->callback_stack_bank = bank;

  aurora_word_t trampoline = user->callback_trampoline;
  if (trampoline != 0u)
    __asm__ volatile("pushu %0" : : "r"(trampoline) : "memory");
  return target + AURORA_INTERRUPT_FRAME_WORDS;
}

/* r0=character, r1=the interrupted process's complete frame top. */
__attribute__((noinline, optnone, used)) static aurora_word_t
handle_keyboard_interrupt(aurora_word_t character,
                          aurora_word_t interrupted_stack_top) {
  volatile aurora_process_context *user = &process_table[PROCESS_USER];
  /* Some translated keys (notably Shift+9 -> '(') retain keyboard metadata
   * in bits 15:8.  The current Aurora optnone lowering incorrectly turns the
   * equivalent C compound-AND into a load/store no-op.  Force the intended
   * single-cycle ALU operation explicitly; using REM here can expose a stale
   * divider result in the compact IRQ path. */
  aurora_word_t character_mask = 0xffu;
  __asm__ volatile("and %0, %1, %0"
                   : "+r"(character)
                   : "r"(character_mask)
                   : "memory");

  /* Callback mode is deliberately one hardware IRQ -> at most one upcall.
   * Do not also put the word in the polling queue, and do not build a chain
   * of callbacks from buffered input.  This makes it impossible for a stale
   * queue entry to be delivered repeatedly after one physical key press. */
  if (user->subscribed_vector != AURORA_IVT_KEYBOARD ||
      !user->valid) {
    enqueue_keyboard_word(character);
    return 0u;
  }

  /* The callback is only a few stores.  If another keyboard IRQ arrives
   * before it returns, acknowledge and drop that word rather than mutating
   * the retained callback context or queueing a recursive delivery. */
  if (user->callback_active)
    return 0u;

  keyboard_irq_sequence = (keyboard_irq_sequence + 1u) & 0x0000ffffu;
  if (keyboard_irq_sequence == 0u)
    keyboard_irq_sequence = 1u;
  aurora_word_t event_word =
      0xA5000000u |
      (keyboard_irq_sequence * keyboard_event_radix) | character;

  aurora_word_t source = current_process == PROCESS_USER
                             ? interrupted_stack_top
                             : user->ksp;
  aurora_word_t callback_top =
      prepare_user_callback(source, event_word);
  if (callback_top == 0u)
    return 0u;
  if (current_process == PROCESS_USER)
    return callback_top;
  user->ksp = callback_top;
  return 0u;
}

/* Referenced by the naked syscall entry. */
__attribute__((noinline, optnone, used)) static aurora_word_t
take_callback_restore_stack_top(void) {
  volatile aurora_process_context *user = &process_table[PROCESS_USER];
  /* Rebuild a fresh complete frame on the user's ordinary kernel-stack bank.
   * This makes PCB state authoritative and avoids selecting a stale/corrupted
   * frame that was left behind before the asynchronous callback ran. */
  aurora_word_t target = AURORA_USER_KERNEL_STACK;
  *(volatile aurora_word_t *)(unsigned long)(target + 0u) = user->flags;
  *(volatile aurora_word_t *)(unsigned long)(target + 1u) = user->pc;
  for (aurora_word_t i = 0; i < 16u; ++i)
    *(volatile aurora_word_t *)(unsigned long)(target + 2u + i) = user->r[i];
  aurora_word_t restored_top = target + AURORA_INTERRUPT_FRAME_WORDS;
  user->callback_active = 0u;
  user->callback_restore_ksp = 0u;
  user->ksp = restored_top;
  return restored_top;
}

static void initialize_processes(void) {
  volatile aurora_process_context *system = &process_table[PROCESS_SYSTEM];
  volatile aurora_process_context *user = &process_table[PROCESS_USER];

  current_process = PROCESS_SYSTEM;
  system->valid = 0u; /* Captured by the first tick interrupt. */

  for (aurora_word_t i = 0; i < 16u; ++i)
    user->r[i] = 0u;
  user->pc = AURORA_UROM_BEGIN;
  user->flags = 0u; /* p=0: IRET enters user mode. */
  /* Prebuild the user's first complete interrupt frame.  Later frames stay
   * directly on each process's private kernel stack, avoiding a GPR copy. */
  aurora_word_t frame = AURORA_USER_KERNEL_STACK;
  aurora.Write(frame++, user->flags);
  aurora.Write(frame++, user->pc);
  for (aurora_word_t i = 0; i < 16u; ++i)
    aurora.Write(frame++, user->r[i]);
  user->ksp = frame;
  user->valid = 1u;
  user->subscribed_vector = 0u;
  user->callback = (aurora_keyboard_callback_t)0;
  user->callback_trampoline = 0u;
  user->callback_restore_ksp = 0u;
  user->callback_active = 0u;
  user->callback_stack_bank = 0u;
}

/*
 * stack_top is KSP after the naked entry has pushed r0-r15.  Below those
 * words are the hardware-pushed FLAGS and PC, in that address order.  IRET
 * therefore pops PC first and FLAGS last, so changing P cannot switch stacks
 * before PC has been restored.  The complete frame remains on that process's
 * private kernel stack; switching only selects another saved frame top.
 */
__attribute__((noinline, optnone)) aurora_word_t
aurora_schedule_from_tick(aurora_word_t stack_top) {
  /* This first scheduler deliberately has two fixed processes.  Keeping the
   * PCB addresses explicit also avoids the old backend's pathological DAG for
   * a global-array address with a run-time index passed to a call. */
  volatile aurora_process_context *system = &process_table[PROCESS_SYSTEM];
  volatile aurora_process_context *user = &process_table[PROCESS_USER];

  if (current_process == PROCESS_SYSTEM) {
    system->ksp = stack_top;
    system->valid = 1u;
    current_process = PROCESS_USER;
    return user->ksp;
  }

  user->ksp = stack_top;
  user->valid = 1u;
  current_process = PROCESS_SYSTEM;
  return system->ksp;
}

__attribute__((noreturn)) void _start(void) {
  /* In kernel mode this denotes KSP initialization. */
  aurora.SetUpStackPointer();
  kernel_boot_status("kernel stack initialized");

  aurora.DisableInterrupt();
  kernel_boot_status("interrupts disabled");

  keyboard_read = 0;
  keyboard_write = 0;
  keyboard_irq_sequence = 0;
  user_fault_vector = 0;
  user_exit_code = 0;
  kernel_boot_status("kernel state initialized");

  initialize_processes();
  kernel_boot_status("process contexts initialized");

  /* Hardware IRQs/exceptions go through IVT; software INT does not. */
  aurora.interrupt.Register(AURORA_IVT_USER_MEMORY_FAULT,
                            aurora_user_memory_fault);
  aurora.interrupt.Register(AURORA_IVT_USER_ILLEGAL_INSTRUCTION,
                            aurora_user_illegal_instruction);
  kernel_boot_status("exception handlers installed");

  aurora.interrupt.Register(AURORA_IVT_KEYBOARD, aurora_keyboard_irq);
  kernel_boot_status("keyboard interrupt installed");

  aurora.interrupt.Register(AURORA_IVT_TICK, aurora_tick_irq);
  kernel_boot_status("tick scheduler installed");

  /* Finish all boot output before enabling preemption so user output cannot
   * be interleaved with the startup banner. */
  kernel_boot_status("startup complete");
  aurora.io.PutChar('\n');
  aurora.io.PutChar('\n');
  aurora.io.PutChar('\n');

  /* Boot-to-user transfer needs a hardware-defined initial IRET frame. */
  aurora.EnableInterrupt();
  for (;;)
    aurora.Nop();
}

/*
 * Keep the keyboard entry explicit. A normal compiler-generated interrupt
 * frame cannot safely combine the all-GPR IRQ save with a nested C call yet.
 * Reading the device both acknowledges the IRQ and obtains its input word.
 */
__attribute__((naked, noinline)) void aurora_keyboard_irq(void) {
  __asm__ volatile(
    "  push r0\n"
    "  push r1\n"
    "  push r2\n"
    "  push r3\n"
    "  push r4\n"
    "  push r5\n"
    "  push r6\n"
    "  push r7\n"
    "  push r8\n"
    "  push r9\n"
    "  push r10\n"
    "  push r11\n"
    "  push r12\n"
    "  push r13\n"
    "  push r14\n"
    "  push r15\n"
    "  osp r1\n"
    "  ldi r0, 0x04000000\n"
    "  ild r0, r0\n"
    "  call handle_keyboard_interrupt\n"
    "  ldi r1, 0x00000000\n"
    "  cmp r0, r1\n"
    "  jcc z, .Lkeyboard_keep_stack\n"
    "  lsp r0\n"
    ".Lkeyboard_keep_stack:\n"
    "  pop r15\n"
    "  pop r14\n"
    "  pop r13\n"
    "  pop r12\n"
    "  pop r11\n"
    "  pop r10\n"
    "  pop r9\n"
    "  pop r8\n"
    "  pop r7\n"
    "  pop r6\n"
    "  pop r5\n"
    "  pop r4\n"
    "  pop r3\n"
    "  pop r2\n"
    "  pop r1\n"
    "  pop r0\n"
    "  iret\n");
}

/* These handlers are installed manually because they are not device names. */
__attribute__((interrupt)) void aurora_user_memory_fault(void) {
  user_fault_vector = AURORA_IVT_USER_MEMORY_FAULT;
  /* No ISA operation can replace the saved user PC, so retrying would loop. */
  halt_after_user_fault("Memory access violation in USER_PROCESS.");
}

__attribute__((interrupt)) void aurora_user_illegal_instruction(void) {
  user_fault_vector = AURORA_IVT_USER_ILLEGAL_INSTRUCTION;
  halt_after_user_fault("Illegal instruction in USER_PROCESS.");
}

/*
 * Dedicated preemptive-switch entry.  Save r0 before using it to acknowledge
 * the tick device, then save the other GPRs.  The C helper returns the prepared
 * next-process stack top in r0.  Switching KSP makes the following pops and
 * IRET restore that process rather than the interrupted one.
 */
__attribute__((naked, noinline)) void aurora_tick_irq(void) {
  __asm__ volatile(
    "  push r0\n"
    "  ldi r0, 0x07000000\n"
    "  ild r0, r0\n"
    "  push r1\n"
    "  push r2\n"
    "  push r3\n"
    "  push r4\n"
    "  push r5\n"
    "  push r6\n"
    "  push r7\n"
    "  push r8\n"
    "  push r9\n"
    "  push r10\n"
    "  push r11\n"
    "  push r12\n"
    "  push r13\n"
    "  push r14\n"
    "  push r15\n"
    "  osp r0\n"
    "  call aurora_schedule_from_tick\n"
    "  lsp r0\n"
    "  pop r15\n"
    "  pop r14\n"
    "  pop r13\n"
    "  pop r12\n"
    "  pop r11\n"
    "  pop r10\n"
    "  pop r9\n"
    "  pop r8\n"
    "  pop r7\n"
    "  pop r6\n"
    "  pop r5\n"
    "  pop r4\n"
    "  pop r3\n"
    "  pop r2\n"
    "  pop r1\n"
    "  pop r0\n"
    "  iret\n");
}

/* Called only by aurora_syscall_entry.  ABI input is r0-r3; return is r0. */
aurora_status_t kernel_syscall_dispatch(aurora_word_t request,
                                        aurora_word_t arg0,
                                        aurora_word_t arg1,
                                        aurora_word_t arg2) {
  volatile aurora_process_context *user = &process_table[PROCESS_USER];
  switch (request) {
  case AURORA_SYSCALL_ABI_VERSION:
    return 1;
  case AURORA_SYSCALL_CONSOLE_PUTCHAR:
    aurora.io.PutChar(arg0 & 0xffu);
    return AURORA_OK;
  case AURORA_SYSCALL_KEYBOARD_SUBSCRIBE:
    if (arg2 != AURORA_IVT_KEYBOARD ||
        (arg0 != 0u && (!is_user_code_address(arg0) ||
                        (arg1 != 0u && !is_user_code_address(arg1)))))
      return AURORA_EFAULT;
    user->subscribed_vector = arg0 == 0u ? 0u : arg2;
    user->callback =
        (aurora_keyboard_callback_t)(unsigned long)arg0;
    user->callback_trampoline = arg0 == 0u ? 0u : arg1;
    /* Polling words belong to the pre-subscription input mode.  Never let
     * them leak into a later callback-driven input transaction. */
    if (arg0 != 0u)
      keyboard_read = keyboard_write;
    return AURORA_OK;
  case AURORA_SYSCALL_KEYBOARD_NEXT:
    if (keyboard_read == keyboard_write)
      return AURORA_EAGAIN;
    arg0 = keyboard_queue[keyboard_read];
    keyboard_read = (keyboard_read + 1u) % KEYBOARD_QUEUE_SIZE;
    return (aurora_status_t)arg0;
  case AURORA_SYSCALL_YIELD:
    return AURORA_OK;
  case AURORA_SYSCALL_CONSOLE_PUTINT:
    kernel_console_put_int(arg0);
    return AURORA_OK;
  case AURORA_SYSCALL_CONSOLE_PUTSTRING:
    return kernel_console_put_string(arg0);
  case AURORA_SYSCALL_CALLBACK_RETURN:
    if (current_process != PROCESS_USER || !user->callback_active)
      return AURORA_EINVAL;
    return (aurora_status_t)AURORA_CALLBACK_RESTORE_MAGIC;
  case AURORA_SYSCALL_PROCESS_EXIT:
    if (current_process != PROCESS_USER)
      return AURORA_EINVAL;
    halt_after_user_exit(arg0);
  default:
    return AURORA_ENOSYS;
  }
}

/*
 * Keep r0 unsaved in the software-INT entry stub: r0 is both the request
 * register and the ABI return register.  r1-r15 are restored before IRET.
 */
__attribute__((naked, noinline)) void aurora_syscall_entry(void) {
  __asm__ volatile(
    /* A hardware IRQ must never capture the middle of a software-INT kernel
     * frame while current_process still denotes the calling user process. */
    "  di\n"
    "  push r1\n"
    "  push r2\n"
    "  push r3\n"
    "  push r4\n"
    "  push r5\n"
    "  push r6\n"
    "  push r7\n"
    "  push r8\n"
    "  push r9\n"
    "  push r10\n"
    "  push r11\n"
    "  push r12\n"
    "  push r13\n"
    "  push r14\n"
    "  push r15\n"
    "  call kernel_syscall_dispatch\n"
    "  ldi r1, 0xa32cb007\n"
    "  cmp r0, r1\n"
    "  jcc nz, .Lsyscall_finish\n"
    /* CALLBACK_RETURN selects the original hardware-IRQ frame, which has
     * r0-r15.  It must not use the normal syscall tail that intentionally
     * leaves r0 as the syscall return value. */
    "  call take_callback_restore_stack_top\n"
    "  lsp r0\n"
    "  pop r15\n"
    "  pop r14\n"
    "  pop r13\n"
    "  pop r12\n"
    "  pop r11\n"
    "  pop r10\n"
    "  pop r9\n"
    "  pop r8\n"
    "  pop r7\n"
    "  pop r6\n"
    "  pop r5\n"
    "  pop r4\n"
    "  pop r3\n"
    "  pop r2\n"
    "  pop r1\n"
    "  pop r0\n"
    "  iret\n"
    ".Lsyscall_finish:\n"
    "  pop r15\n"
    "  pop r14\n"
    "  pop r13\n"
    "  pop r12\n"
    "  pop r11\n"
    "  pop r10\n"
    "  pop r9\n"
    "  pop r8\n"
    "  pop r7\n"
    "  pop r6\n"
    "  pop r5\n"
    "  pop r4\n"
    "  pop r3\n"
    "  pop r2\n"
    "  pop r1\n"
    "  iret\n");
}
