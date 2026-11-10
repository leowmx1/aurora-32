#include <aurora.h>
#include "../user-programs/include/aurora_syscall.h"

AURORA_KERNEL_PROGRAM;

/* ROM reset PC is 0.  Module-level assembly is emitted before compiler
 * functions, so keep a permanent first-word jump to the C runtime entry even
 * when optimization or helper functions change function emission order. */
__asm__(
    ".globl __aurora_rom_entry\n"
    "__aurora_rom_entry:\n"
    "  jmp aurora_reset_entry\n"
    ".globl aurora_syscall_gateway\n"
    "aurora_syscall_gateway:\n"
    "  jmp aurora_syscall_entry\n"
    "aurora_reset_entry:\n"
    "  ldi r14, 0x02000000\n"
    "  lsp r14\n"
    "  lfp r14\n"
    "  di\n"
    "  call aurora_early_boot_log\n"
    "  jmp _start\n");

enum {
  AURORA_UROM_BEGIN = 0x80000000u,
  AURORA_UROM_END = 0x80400000u,
  AURORA_URAM_BEGIN = 0x81000000u,
  AURORA_URAM_END = 0x81400000u,
  AURORA_BANK_DEVICE = 0x0d000000u,
  AURORA_TICK_DEVICE = 0x07000000u,
  AURORA_TELNET_DEVICE = 0x06000000u,
  TELNET_QUEUE_SIZE = 128u,
  /* Read-only kernel device: system uptime in milliseconds modulo 2^32. */
  AURORA_TIMER_DEVICE = 0x08000000u,
  AURORA_USER_KERNEL_STACK = 0x02010000u,
  AURORA_INTERRUPT_FRAME_WORDS = 18u,
  KEYBOARD_QUEUE_SIZE = 128u,
  EVENT_QUEUE_SIZE = 64u,
  PROCESS_SYSTEM = 0u,
  PROCESS_COUNT = 5u,
  USER_BANK_WORDS = 0x00400000u,
  PROCESS_KSTACK_STRIDE = 0x00030000u,
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
  aurora_telnet_callback_t telnet_callback;
  aurora_word_t telnet_trampoline;
  /* One FIFO per user process; hardware IRQ observation order, not device time. */
  aurora_word_t events[EVENT_QUEUE_SIZE];
  aurora_word_t event_read, event_write, event_drops;
  aurora_word_t waiting, wake_pending, event_defer_depth;
  aurora_word_t callback_budget, callback_cooldown;
  /* waiting + sleep_ms==0: event wait; waiting + sleep_ms>0: timed wait. */
  aurora_word_t sleep_start_ms, sleep_ms;
  aurora_word_t bank, bank_base, usp, ufp, callback_usp, callback_ufp;
  aurora_word_t console, background, exit_code;
  aurora_word_t name[64];
  aurora_word_t file_refs[65];
} aurora_process_context;

/* Kernel-resident state.  It is intentionally private to kernel RAM. */
static volatile aurora_word_t keyboard_queue[KEYBOARD_QUEUE_SIZE];
static volatile aurora_word_t keyboard_read;
static volatile aurora_word_t keyboard_write;
static volatile aurora_word_t keyboard_irq_sequence;
static volatile aurora_word_t telnet_queue[TELNET_QUEUE_SIZE];
static volatile aurora_word_t telnet_read, telnet_write, telnet_irq_sequence;
/* Volatile prevents the generic optimizer from replacing x * 256 with SFT;
 * this CPU's physical signed shift-count convention is target-specific. */
static volatile aurora_word_t keyboard_event_radix = 256u;
static volatile aurora_word_t user_fault_vector;
static volatile aurora_word_t user_exit_code;
static volatile aurora_process_context process_table[PROCESS_COUNT];
static volatile aurora_word_t current_process;
/* SYSTEM owns shell_console; user_console is fixed for a program's lifetime. */
static volatile aurora_word_t shell_console;
static volatile aurora_word_t foreground[2], background[2];
static volatile aurora_word_t last_user, user_dispatch_paused;
static volatile aurora_word_t fresh_dispatch, dispatch_pc;
/* 0 = unowned; 1..4 = process; 0xffffffff = SYSTEM renderer. */
static volatile aurora_word_t graphics_owner, midi_owner;
static volatile aurora_word_t graphics_logo_needed=1u;
/* Exit notifications run with IE clear, independent of the scheduled process. */
static volatile aurora_word_t notice_active, notice_console;
static volatile aurora_word_t console_prompt[2], console_banner[2];
static volatile aurora_word_t input_swallow_lf[2];
static volatile aurora_word_t shell_fs_busy;
/* No Close syscall yet: file IDs exposed to a user stay pinned until exit. */
#define user_file_refs (process_table[current_process].file_refs)
#define user_console (process_table[current_process].console)
static aurora_word_t user_physical(aurora_word_t logical) {
  return logical + process_table[current_process].bank_base;
}
static aurora_word_t process_kernel_stack(aurora_word_t pid) {
  return AURORA_USER_KERNEL_STACK + (pid - 1u) * PROCESS_KSTACK_STRIDE;
}
static volatile aurora_word_t keyboard_drops, telnet_drops;
/* Lifetime device traffic, including CR/LF; never reset on process exit. */
static volatile aurora_word_t telnet_rx_bytes, telnet_tx_bytes;
static volatile aurora_word_t telnet_rx_ms, telnet_tx_ms;
static volatile aurora_word_t telnet_rx_seen, telnet_tx_seen;
static void kernel_telnet_putchar(aurora_word_t character) {
  aurora.Write(AURORA_TELNET_DEVICE, character & 0xffu);
  ++telnet_tx_bytes;
  telnet_tx_ms = aurora.Read(AURORA_TIMER_DEVICE);
  telnet_tx_seen = 1u;
}
/* Select by scheduled process, not a shared temporary redirection flag: a
 * preempted shell print cannot redirect a user's simultaneous output. */
static __inline__ __attribute__((always_inline)) void console_putchar(aurora_word_t c) {
  aurora_word_t port=notice_active?notice_console:(current_process?user_console:shell_console);
  if(port) {
    if(c=='\n') kernel_telnet_putchar('\r');
    kernel_telnet_putchar(c);
  } else aurora.io.PutChar(c);
}

static int is_user_read_address(aurora_word_t address);
#include "midi.h"

extern void aurora_syscall_entry(void);
void aurora_tick_irq(void);
void aurora_keyboard_irq(void);
void aurora_telnet_irq(void);
void aurora_user_memory_fault(void);
void aurora_user_illegal_instruction(void);

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
    console_putchar('-');
    magnitude = 0u - bits;
  }
  if (magnitude == 0u) {
    console_putchar('0');
    return;
  }
  while (magnitude != 0u) {
    digits[count++] = magnitude % 10u;
    magnitude /= 10u;
  }
  while (count != 0u)
    console_putchar('0' + digits[--count]);
}

static void kernel_console_put_uint(aurora_word_t value) {
  aurora_word_t digits[10], count = 0u;
  do { digits[count++] = value % 10u; value /= 10u; } while (value);
  while (count) console_putchar('0' + digits[--count]);
}

/* Called before compiler-generated RAM initialization: immediates and stack
 * only. Do not use string literals or initialized globals in this routine. */
__attribute__((used, noinline)) void aurora_early_boot_log(void) {
  aurora_word_t digits[10], n=0u, value=aurora.Read(AURORA_TIMER_DEVICE);
  aurora.io.PutChar('[');
  do { digits[n++]=value%10u; value/=10u; } while(value);
  while(n) aurora.io.PutChar('0'+digits[--n]);
  aurora.io.PutChar(' '); aurora.io.PutChar('m'); aurora.io.PutChar('s');
  aurora.io.PutChar(']'); aurora.io.PutChar(' ');
  aurora.io.PutChar('I'); aurora.io.PutChar('n'); aurora.io.PutChar('i');
  aurora.io.PutChar('t'); aurora.io.PutChar('i'); aurora.io.PutChar('a');
  aurora.io.PutChar('l'); aurora.io.PutChar('i'); aurora.io.PutChar('z');
  aurora.io.PutChar('i'); aurora.io.PutChar('n'); aurora.io.PutChar('g');
  aurora.io.PutChar(' '); aurora.io.PutChar('R'); aurora.io.PutChar('A');
  aurora.io.PutChar('M'); aurora.io.PutChar('.'); aurora.io.PutChar('\n');
}

/* Runtime radix avoids the bundled backend's right-shift pseudo/scratch
 * interaction with callee-save reloads. Keep this printer free of SFT. */
static volatile aurora_word_t kernel_hex_radix = 16u;
static void kernel_console_put_hex(aurora_word_t value) {
  aurora_word_t divisor = 0x10000000u;
  console_putchar('0'); console_putchar('x');
  for (aurora_word_t i = 8; i; --i) {
    aurora_word_t d = value / divisor;
    console_putchar(d < 10u ? '0' + d : 'A' + d - 10u);
    value %= divisor;
    divisor /= kernel_hex_radix;
  }
}

/* Boot messages use kernel-resident string literals, so they must not pass
 * through the user-pointer validation used by the PUT_STRING system call. */
static void kernel_boot_put_string(const char *text) {
  while (*text != '\0') {
    console_putchar((aurora_word_t)*text);
    ++text;
  }
}

static void kernel_status_prefix(void) {
  aurora_word_t milliseconds = aurora.Read(AURORA_TIMER_DEVICE);

  console_putchar('[');
  kernel_console_put_uint(milliseconds);
  kernel_boot_put_string(" ms");
  console_putchar(']');
  console_putchar(' ');
}

static void kernel_boot_status(const char *message) {
  kernel_status_prefix();
  kernel_boot_put_string(message);
  console_putchar('\n');
}

static void mark_user_not_runnable(aurora_word_t pid) {
  volatile aurora_process_context *user = &process_table[pid];

  if(midi_owner==pid) { kernel_midi_all_notes_off(); midi_owner=0u; }
  /* Cancel outstanding asynchronous drawing before the next program. */
  if(graphics_owner==pid) {
    aurora.Write(0x09900000u, 0u); graphics_owner=0u; graphics_logo_needed=1u;
  }
  user->valid = 0u;
  user->waiting = 0u;
  user->sleep_ms = 0u;
  user->event_read = user->event_write;
  user->wake_pending = 0u;
  user->subscribed_vector = 0u;
  user->callback = (aurora_keyboard_callback_t)0;
  user->callback_trampoline = 0u;
  user->callback_restore_ksp = 0u;
  user->callback_active = 0u;
  user->telnet_callback = (aurora_telnet_callback_t)0;
  user->telnet_trampoline = 0u;
  user->event_defer_depth = 0u;
  if(foreground[user->console]==pid) {
    foreground[user->console]=0u;
    if(user->console) telnet_read=telnet_write;
    else keyboard_read=keyboard_write;
    console_prompt[user->console]=1u;
    console_banner[user->console]=1u;
  }
  if(background[user->console]==pid) background[user->console]=0u;
}

static void kernel_fault_spacing(void) {
  console_putchar('\n');
}
__attribute__((noreturn)) static void kernel_shutdown(void);

__attribute__((noinline)) static void
finish_process_exit(aurora_word_t pid, aurora_word_t exit_code) {
  volatile aurora_process_context *user = &process_table[pid];
  user_exit_code = exit_code;
  user->exit_code = exit_code;
  mark_user_not_runnable(pid);
  notice_console=user->console;
  notice_active=1u;

  kernel_fault_spacing();
  kernel_status_prefix();
  kernel_boot_put_string("USER_PROCESS exited with code ");
  kernel_console_put_int(exit_code);
  kernel_boot_put_string(".\n  name=");
  for(aurora_word_t i=0u;user->name[i];++i) console_putchar(user->name[i]);
  kernel_boot_put_string(" slot="); kernel_console_put_uint(pid);
  kernel_boot_put_string(" bank="); kernel_console_put_uint(user->bank);
  kernel_boot_put_string(user->console?" remote ":" local ");
  kernel_boot_put_string(user->background?"background\n\n":"foreground\n\n");
  notice_active=0u;
#ifdef AURORA_DIRECT_BOOT
  kernel_boot_status("Nothing to do. Halt.");
  kernel_shutdown();
#endif
}
__attribute__((noinline)) static void finish_user_exit(aurora_word_t code) {
  finish_process_exit(current_process,code);
}

static aurora_status_t kernel_console_put_string(aurora_word_t address,
                                                aurora_word_t discard) {
  if (!is_user_read_address(address))
    return AURORA_EFAULT;

  aurora_word_t available = (address<AURORA_UROM_END?AURORA_UROM_END:AURORA_URAM_END) - address;
  aurora_word_t limit = available < 4096u ? available : 4096u;
  address=user_physical(address);
  for (aurora_word_t length = 0u; length < limit; ++length, ++address) {
    aurora_word_t character = aurora.Read(address);
    if (character == 0u)
      return AURORA_OK;
    /* Keep bounded string validation even when stdout is discarded. */
    if (!discard) console_putchar(character & 0xffu);
  }
  return available < 4096u ? AURORA_EFAULT : AURORA_EINVAL;
}

#include "console_write.h"
#include "graphics.h"
#include "shutdown.h"

__attribute__((noinline)) static void
enqueue_keyboard_word(aurora_word_t character) {
  aurora_word_t next = (keyboard_write + 1u) % KEYBOARD_QUEUE_SIZE;
  /* Drop the newest word when full; this never writes user memory in IRQ. */
  if (next == keyboard_read) {
    ++keyboard_drops;
    return;
  }
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

/* All queue/state operations run in kernel mode with IE clear. */
__attribute__((noinline, optnone)) static aurora_word_t
event_next(aurora_word_t index) {
  ++index;
  return index == EVENT_QUEUE_SIZE ? 0u : index;
}

__attribute__((noinline, optnone)) static void
discard_events(aurora_word_t tag) {
  volatile aurora_process_context *user = &process_table[current_process];
  aurora_word_t read = user->event_read, kept = read;
  while (read != user->event_write) {
    aurora_word_t event = user->events[read];
    read = event_next(read);
    aurora_word_t kind = event >= 0xa6000000u ? 0xa6u : 0xa5u;
    if (kind != tag) {
      user->events[kept] = event;
      kept = event_next(kept);
    }
  }
  user->event_write = kept;
}

__attribute__((noinline, optnone)) static void
enqueue_user_event(aurora_word_t pid, aurora_word_t event) {
  volatile aurora_process_context *user = &process_table[pid];
  aurora_word_t next = event_next(user->event_write);
  if (next == user->event_read) {
    ++user->event_drops; /* Drop newest, preserve admitted events and order. */
    return;
  }
  user->events[user->event_write] = event;
  user->event_write = next;
  if (user->waiting) {
    user->waiting = 0u;
    user->sleep_ms = 0u; /* Events cancel timed sleep rather than resume it. */
    user->wake_pending = 0u; /* Fulfils the blocked WaitEvent/Sleep. */
    user->callback_cooldown = 0u;
    user->callback_budget = 1u;
  } else {
    /* A sticky credit closes the callback-before-WaitEvent lost-wakeup race. */
    user->wake_pending = 1u;
  }
}

/*
 * Snapshot a suspended user frame in the PCB, copy it to the dedicated
 * callback KSP, replace its PC and r0, and put the return trampoline on USP.
 * CALLBACK_RETURN reconstructs from the PCB instead of trusting a retained
 * KSP memory frame across the whole user callback.
 */
__attribute__((noinline, optnone)) static aurora_word_t
prepare_user_callback(aurora_word_t source_stack_top,
                      aurora_word_t event_word,
                      aurora_word_t callback, aurora_word_t trampoline) {
  volatile aurora_process_context *user = &process_table[current_process];
  if (user->callback_active ||
      !is_user_code_address(callback) ||
      (trampoline != 0u && !is_user_code_address(trampoline)) ||
      source_stack_top < AURORA_INTERRUPT_FRAME_WORDS)
    return 0u;

  aurora_word_t source = source_stack_top - AURORA_INTERRUPT_FRAME_WORDS;
  user->flags = *(volatile aurora_word_t *)(unsigned long)(source + 0u);
  user->pc = *(volatile aurora_word_t *)(unsigned long)(source + 1u);
  for (aurora_word_t i = 0; i < 16u; ++i)
    user->r[i] =
        *(volatile aurora_word_t *)(unsigned long)(source + 2u + i);

  /* Dedicated alternating banks keep callback syscall/IRQ work separate
   * from the suspended foreground frame. */
  aurora_word_t bank;
  aurora_word_t target;
  if (user->callback_stack_bank == 0u) {
    bank = 1u;
    target = process_kernel_stack(current_process)+0x20000u;
  } else {
    bank = 0u;
    target = process_kernel_stack(current_process)+0x10000u;
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
  user->callback_usp=aurora.GetUserStackPointer();
  user->callback_ufp=aurora.GetUserFramePointer();

  if (trampoline != 0u) {
    /* PUSHU selects USP but stays privileged in the current microcode. It
     * would write bank zero through the kernel's physical-address path.
     * Explicitly translate the saved logical USP, then advance logical USP. */
    aurora.Write(user->callback_usp + user->bank_base, trampoline);
    aurora.SetUserStackPointer(user->callback_usp + 1u);
  }
  return target + AURORA_INTERRUPT_FRAME_WORDS;
}

/* Pop exactly once, only after the callback frame is successfully prepared.
 * All subscribed devices share this FIFO and cannot nest across devices. */
__attribute__((noinline, optnone)) static aurora_word_t
prepare_pending_event(aurora_word_t source) {
  volatile aurora_process_context *user = &process_table[current_process];
  if (!user->valid || user->callback_active || user->event_defer_depth ||
      !user->callback_budget || user->event_read == user->event_write)
    return 0u;
  aurora_word_t event = user->events[user->event_read];
  aurora_word_t callback, trampoline;
  if (event >= 0xa6000000u) {
    callback = (aurora_word_t)(unsigned long)user->telnet_callback;
    trampoline = user->telnet_trampoline;
  } else {
    callback = (aurora_word_t)(unsigned long)user->callback;
    trampoline = user->callback_trampoline;
  }
  aurora_word_t top = prepare_user_callback(source, event, callback, trampoline);
  if (top != 0u) {
    user->event_read = event_next(user->event_read);
    user->callback_budget = 0u;
  }
  return top;
}

__attribute__((noinline, optnone)) static aurora_word_t
deliver_irq_event(aurora_word_t interrupted_top) {
  /* Never prepare a callback for a suspended recipient: bank/USP belong to
   * the running user. Selection delivers the queued event after restoring both. */
  if(!current_process) return 0u;
  return prepare_pending_event(interrupted_top);
}

__attribute__((noinline, optnone, used)) static aurora_word_t
handle_keyboard_interrupt(aurora_word_t character, aurora_word_t interrupted_top) {
  aurora_word_t mask = 0xffu;
  __asm__ volatile("and %0, %1, %0" : "+r"(character) : "r"(mask) : "memory");
  if(input_swallow_lf[0]) { input_swallow_lf[0]=0u; if(character=='\n') return 0u; }
  aurora_word_t pid=foreground[0];
  volatile aurora_process_context *user = &process_table[pid];
  if (!pid || !user->valid) { enqueue_keyboard_word(character); return 0u; }
  if (!user->callback) {
    enqueue_keyboard_word(character);
    return 0u;
  }
  keyboard_irq_sequence = (keyboard_irq_sequence + 1u) & 0xffffu;
  if (keyboard_irq_sequence == 0u) keyboard_irq_sequence = 1u;
  enqueue_user_event(pid,0xa5000000u |
                     (keyboard_irq_sequence * keyboard_event_radix) | character);
  return pid==current_process?deliver_irq_event(interrupted_top):0u;
}

__attribute__((noinline, optnone, used)) static aurora_word_t
handle_telnet_interrupt(aurora_word_t character, aurora_word_t interrupted_top) {
  ++telnet_rx_bytes;
  telnet_rx_ms = aurora.Read(AURORA_TIMER_DEVICE);
  telnet_rx_seen = 1u;
  aurora_word_t mask = 0xffu;
  __asm__ volatile("and %0, %1, %0" : "+r"(character) : "r"(mask) : "memory");
  if(input_swallow_lf[1]) { input_swallow_lf[1]=0u; if(character=='\n') return 0u; }
  aurora_word_t pid=foreground[1];
  volatile aurora_process_context *user = &process_table[pid];
  if (!pid || !user->valid || !user->callback) {
    aurora_word_t next = (telnet_write + 1u) % TELNET_QUEUE_SIZE;
    if (next != telnet_read) {
      telnet_queue[telnet_write] = character;
      telnet_write = next;
    } else ++telnet_drops;
    return 0u;
  }
  telnet_irq_sequence = (telnet_irq_sequence + 1u) & 0xffffu;
  if (telnet_irq_sequence == 0u) telnet_irq_sequence = 1u;
  enqueue_user_event(pid,0xa5000000u |
                     (telnet_irq_sequence * keyboard_event_radix) | character);
  return pid==current_process?deliver_irq_event(interrupted_top):0u;
}

/* Referenced by the naked syscall entry. */
__attribute__((noinline, optnone, used)) static aurora_word_t
take_callback_restore_stack_top(void) {
  volatile aurora_process_context *user = &process_table[current_process];
  /* Rebuild a fresh complete frame on the user's ordinary kernel-stack bank.
   * This makes PCB state authoritative and avoids selecting a stale/corrupted
   * frame that was left behind before the asynchronous callback ran. */
  aurora_word_t target = process_kernel_stack(current_process);
  *(volatile aurora_word_t *)(unsigned long)(target + 0u) = user->flags;
  *(volatile aurora_word_t *)(unsigned long)(target + 1u) = user->pc;
  for (aurora_word_t i = 0; i < 16u; ++i)
    *(volatile aurora_word_t *)(unsigned long)(target + 2u + i) = user->r[i];
  aurora_word_t restored_top = target + AURORA_INTERRUPT_FRAME_WORDS;
  user->callback_active = 0u;
  user->callback_restore_ksp = 0u;
  user->ksp = restored_top;
  /* Do not recursively chain callbacks. Reserve the next scheduled user
   * quantum for foreground progress; WaitEvent may explicitly waive it. */
  user->callback_cooldown = 1u;
  user->callback_budget = 0u;
  aurora.SetUserStackPointer(user->callback_usp);
  aurora.SetUserFramePointer(user->callback_ufp);
  return restored_top;
}

static void initialize_user_process(aurora_word_t pid, aurora_word_t entry,
                                    aurora_word_t port, aurora_word_t bg,
                                    const aurora_word_t *name) {
  volatile aurora_process_context *user = &process_table[pid];
  for(aurora_word_t i=0;i<65u;++i) user->file_refs[i]=0u;
  user->bank=pid-1u; user->bank_base=(pid-1u)*USER_BANK_WORDS;
  user->usp=user->ufp=0x82000000u;
  user->console=port; user->background=bg; user->exit_code=0u;
  aurora_word_t len=0u;
  if(name) while(len<63u && name[len]) { user->name[len]=name[len]; ++len; }
  user->name[len]=0u;

  for (aurora_word_t i = 0; i < 16u; ++i)
    user->r[i] = 0u;
  user->pc = entry;
  user->flags = 0u; /* p=0: IRET enters user mode. */
  /* Prebuild the user's first complete interrupt frame.  Later frames stay
   * directly on each process's private kernel stack, avoiding a GPR copy. */
  aurora_word_t frame = process_kernel_stack(pid);
  aurora.Write(frame++, user->flags);
  aurora.Write(frame++, user->pc);
  for (aurora_word_t i = 0; i < 16u; ++i)
    aurora.Write(frame++, user->r[i]);
  user->ksp = frame;
  user->valid = 0u; /* Publish only after the complete context is ready. */
  user->subscribed_vector = 0u;
  user->callback = (aurora_keyboard_callback_t)0;
  user->callback_trampoline = 0u;
  user->callback_restore_ksp = 0u;
  user->callback_active = 0u;
  user->callback_stack_bank = 0u;
  user->event_read = user->event_write = user->event_drops = 0u;
  user->waiting = user->wake_pending = user->event_defer_depth = 0u;
  user->sleep_start_ms = user->sleep_ms = 0u;
  user->callback_budget = 1u;
  user->callback_cooldown = 0u;
  user->telnet_callback = (aurora_telnet_callback_t)0;
  user->telnet_trampoline = 0u;
  if(bg) background[port]=pid;
  else {
    foreground[port]=pid;
    if(port) { telnet_read=telnet_write; telnet_irq_sequence=0u; }
    else { keyboard_read=keyboard_write; keyboard_irq_sequence=0u; }
  }
  user_fault_vector = user_exit_code = 0u;
  user->valid = 1u;
}

#include "shell.h"
#include "file_api.h"

__attribute__((noinline,optnone)) static aurora_word_t
return_to_system(aurora_word_t stack_top) {
  volatile aurora_process_context *user=&process_table[current_process];
  user->ksp=stack_top;
  user->usp=aurora.GetUserStackPointer();
  user->ufp=aurora.GetUserFramePointer();
  current_process=PROCESS_SYSTEM;
  return process_table[PROCESS_SYSTEM].ksp;
}
static int any_user_runnable(void) {
  if(user_dispatch_paused) return 0;
  for(aurora_word_t pid=1u;pid<PROCESS_COUNT;++pid)
    if(process_table[pid].valid && !process_table[pid].waiting) return 1;
  return 0;
}

/*
 * stack_top is KSP after the naked entry has pushed r0-r15.  Below those
 * words are the hardware-pushed FLAGS and PC, in that address order.  IRET
 * therefore pops PC first and FLAGS last, so changing P cannot switch stacks
 * before PC has been restored.  The complete frame remains on that process's
 * private kernel stack; switching only selects another saved frame top.
 */
__attribute__((noinline, optnone)) aurora_word_t
aurora_schedule_from_tick(aurora_word_t stack_top, aurora_word_t tick) {
  volatile aurora_process_context *system = &process_table[PROCESS_SYSTEM];

  /* Only tick checks the deadline. The idle
   * loop does not poll timer or reschedule a blocked user. Unsigned subtraction
   * handles the millisecond timer wrapping through zero. */
  if(tick) {
    kernel_midi_service();
    aurora_word_t now=aurora.Read(AURORA_TIMER_DEVICE);
    for(aurora_word_t pid=1u;pid<PROCESS_COUNT;++pid) {
      volatile aurora_process_context *user=&process_table[pid];
      if(user->valid && user->waiting && user->sleep_ms &&
         (aurora_word_t)(now-user->sleep_start_ms)>=user->sleep_ms) {
        user->waiting=0u; user->sleep_ms=0u;
      }
    }
  }
  if (current_process == PROCESS_SYSTEM) {
    system->ksp = stack_top;
    system->valid = 1u;
    if(user_dispatch_paused) return stack_top;
    aurora_word_t pid=last_user;
    for(aurora_word_t n=1u;n<PROCESS_COUNT;++n) {
      ++pid; if(pid==PROCESS_COUNT) pid=1u;
      volatile aurora_process_context *user=&process_table[pid];
      if(!user->valid || user->waiting) continue;
      last_user=current_process=pid;
      aurora.Write(AURORA_BANK_DEVICE,user->bank);
      aurora.SetUserStackPointer(user->usp);
      aurora.SetUserFramePointer(user->ufp);
      if(user->callback_cooldown) {
        user->callback_cooldown=0u; user->callback_budget=0u;
      } else {
        user->callback_budget=1u;
        aurora_word_t pending_top=prepare_pending_event(user->ksp);
        if(pending_top) user->ksp=pending_top;
      }
      /* A pending tick can arrive immediately after IRET, before the first
       * user instruction. Remember that PC to avoid phase-locked starvation. */
      dispatch_pc=aurora.Read(user->ksp-AURORA_INTERRUPT_FRAME_WORDS+1u);
      fresh_dispatch=1u;
      return user->ksp;
    }
    return stack_top;
  }
  if(tick && fresh_dispatch) {
    fresh_dispatch=0u;
    if(aurora.Read(stack_top-AURORA_INTERRUPT_FRAME_WORDS+1u)==dispatch_pc)
      return stack_top; /* At most one grace tick; a JMP-to-self cannot monopolize. */
  }
  return return_to_system(stack_top);
}

__attribute__((noreturn)) void _start(void) {
  /* In kernel mode this denotes KSP initialization. */
  aurora.SetUpStackPointer();
  kernel_boot_status("Kernel stack initialized.");

  aurora.DisableInterrupt();
  kernel_boot_status("Interrupts masked for initialization.");

  /* Do not depend on the bank register's reset state. */
  aurora.Write(AURORA_BANK_DEVICE, 0u);

  keyboard_read = 0;
  keyboard_write = 0;
  keyboard_irq_sequence = 0;
  telnet_read = telnet_write = telnet_irq_sequence = 0u;
  user_fault_vector = 0;
  user_exit_code = 0;
  keyboard_drops = telnet_drops = 0u;
  kernel_midi_initialize();
  kernel_boot_status("Kernel services initialized.");

  current_process = PROCESS_SYSTEM;
  process_table[PROCESS_SYSTEM].valid = 0u;
  for(aurora_word_t pid=1u;pid<PROCESS_COUNT;++pid) process_table[pid].valid=0u;
  last_user=user_dispatch_paused=graphics_owner=midi_owner=notice_active=0u;
  foreground[0]=foreground[1]=background[0]=background[1]=0u;
#ifdef AURORA_DIRECT_BOOT
  initialize_user_process(1u,AURORA_UROM_BEGIN,0u,0u,(const aurora_word_t *)"direct-user");
#endif
  kernel_boot_status("Process manager initialized.");

  /* Hardware IRQs/exceptions go through IVT; software INT does not. */
  aurora.interrupt.Register(AURORA_IVT_USER_MEMORY_FAULT,
                            aurora_user_memory_fault);
  aurora.interrupt.Register(AURORA_IVT_USER_ILLEGAL_INSTRUCTION,
                            aurora_user_illegal_instruction);
  kernel_boot_status("Exception handlers registered.");

  aurora.interrupt.Register(AURORA_IVT_KEYBOARD, aurora_keyboard_irq);
  kernel_boot_status("Keyboard interrupt registered.");
  aurora.interrupt.Register(AURORA_IVT_TELNET, aurora_telnet_irq);
  kernel_boot_status("Telnet interrupt registered.");

  aurora.interrupt.Register(AURORA_IVT_TICK, aurora_tick_irq);
  kernel_boot_status("Tick scheduler initialized.");

  /* Finish all boot output before enabling preemption so user output cannot
   * be interleaved with the startup banner. */
  kernel_boot_status("Kernel initialization complete.");
  console_putchar('\n');

  #ifndef AURORA_DIRECT_BOOT
  shell_initialize();
  #endif
  /* INT supplies an ordinary complete system frame, including the first
   * boot-to-user transfer. Recheck runnable state inside the atomic syscall;
   * this unlocked check is only an idle-loop optimization. */
  aurora.EnableInterrupt();
  for (;;) {
    #ifndef AURORA_DIRECT_BOOT
    shell_service();
    /* shell_service leaves IRQs disabled after publishing a new user, so no
     * switch can capture a SYSTEM frame inside the loader/shell call stack. */
    aurora.EnableInterrupt();
    #endif
    if (any_user_runnable()) {
      register aurora_word_t request __asm__("r0") = AURORA_SYSCALL_YIELD;
      __asm__ volatile("int aurora_syscall_entry" : "+r"(request) : : "memory");
    } else {
      aurora.Nop(); /* Keep IRQs enabled; HALT is terminal on this CPU. */
    }
  }
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

__attribute__((naked, noinline)) void aurora_telnet_irq(void) {
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
    "  ldi r0, 0x06000000\n"
    "  ild r0, r0\n"
    "  call handle_telnet_interrupt\n"
    "  ldi r1, 0x00000000\n"
    "  cmp r0, r1\n"
    "  jcc z, .Ltelnet_keep_stack\n"
    "  lsp r0\n"
    ".Ltelnet_keep_stack:\n"
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



/* Hardware user protection faults are fatal: never IRET/retry/schedule out
 * of the failed context. Ordinary syscall argument errors remain recoverable. */
__attribute__((noinline, optnone, used, noreturn)) static void
finish_user_fault(aurora_word_t vector) {
  aurora.DisableInterrupt();
  user_fault_vector = vector;
  if (!current_process || !process_table[PROCESS_SYSTEM].valid) {
    kernel_boot_status("Unexpected exception in SYSTEM. Halt.");
    kernel_blue_screen(vector,0);
  }
  mark_user_not_runnable(current_process);
  kernel_fault_spacing();
  kernel_boot_status(vector == AURORA_IVT_USER_MEMORY_FAULT ?
      "Memory access violation in USER_PROCESS." :
      "Illegal instruction in USER_PROCESS.");
  console_putchar('\n');
  kernel_blue_screen(vector,1);
}
__attribute__((naked, noinline)) void aurora_user_memory_fault(void) {
  __asm__ volatile("ldi r0, 0x03000004\n"
                   "call finish_user_fault\n"
                   "halt\n");
}
__attribute__((naked, noinline)) void aurora_user_illegal_instruction(void) {
  __asm__ volatile("ldi r0, 0x03000005\n"
                   "call finish_user_fault\n"
                   "halt\n");
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
    "  ldi r1, 1\n"
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
  volatile aurora_process_context *user = &process_table[current_process];
  /* All resource ownership decisions and syscalls execute with IE clear. */
  if(request==AURORA_SYSCALL_GRAPHICS_FILL_RECT || request==AURORA_SYSCALL_GRAPHICS_BITMAP ||
     request==AURORA_SYSCALL_GRAPHICS_BUSY || request==AURORA_SYSCALL_GRAPHICS_RECT16 ||
     request==AURORA_SYSCALL_GRAPHICS_IMAGE) {
    if(graphics_owner==0xffffffffu) return AURORA_EBUSY;
    if(graphics_owner && graphics_owner!=current_process) return AURORA_EPERM;
  }
  if(request>=AURORA_SYSCALL_MIDI_NOTE_ON && request<=AURORA_SYSCALL_MIDI_SEQUENCE_BUSY) {
    if(midi_owner && midi_owner!=current_process) return AURORA_EPERM;
  }
  if(request>=AURORA_SYSCALL_FS_LIST && request<=AURORA_SYSCALL_FS_INSTALL && shell_fs_busy)
    return AURORA_EBUSY;
  switch (request) {
  case AURORA_SYSCALL_FS_LIST: return kernel_fs_list(arg0,arg1,arg2);
  case AURORA_SYSCALL_FS_OPEN: return kernel_fs_open(arg0);
  case AURORA_SYSCALL_FS_READ: return kernel_fs_read(arg0);
  case AURORA_SYSCALL_FS_SAVE: return kernel_fs_save(arg0,arg1,arg2);
  case AURORA_SYSCALL_FS_INFO: return kernel_fs_info(arg0,arg1);
  case AURORA_SYSCALL_FS_INSTALL: return kernel_fs_install(arg0,arg1,arg2);
  case AURORA_SYSCALL_ABI_VERSION:
    return 3;
  case AURORA_SYSCALL_CONSOLE_PUTCHAR:
    /* Suppress only user stdout, not SYSTEM's exit/fault notifications. */
    if (!user->background) console_putchar(arg0 & 0xffu);
    return AURORA_OK;
  case AURORA_SYSCALL_KEYBOARD_SUBSCRIBE:
    if(user->background) return AURORA_EBUSY;
    if (arg2 != AURORA_IVT_KEYBOARD ||
        (arg0 != 0u && (!is_user_code_address(arg0) ||
                        (arg1 != 0u && !is_user_code_address(arg1)))))
      return AURORA_EFAULT;
    discard_events(0xa5u);
    user->subscribed_vector = arg0 == 0u ? 0u : arg2;
    user->callback =
        (aurora_keyboard_callback_t)(unsigned long)arg0;
    user->callback_trampoline = arg0 == 0u ? 0u : arg1;
    /* Polling words belong to the pre-subscription input mode.  Never let
     * them leak into a later callback-driven input transaction. */
    if (arg0 != 0u) {
      if(user_console) telnet_read=telnet_write;
      else keyboard_read=keyboard_write;
    }
    return AURORA_OK;
  case AURORA_SYSCALL_KEYBOARD_NEXT:
    if(user->background) return AURORA_EBUSY;
    if (user->callback) return AURORA_EBUSY;
    if(user_console) {
      if(telnet_read==telnet_write) return AURORA_EAGAIN;
      arg0=telnet_queue[telnet_read]; telnet_read=(telnet_read+1u)%TELNET_QUEUE_SIZE;
      return (aurora_status_t)arg0;
    }
    if (keyboard_read == keyboard_write)
      return AURORA_EAGAIN;
    arg0 = keyboard_queue[keyboard_read];
    keyboard_read = (keyboard_read + 1u) % KEYBOARD_QUEUE_SIZE;
    return (aurora_status_t)arg0;
  case AURORA_SYSCALL_YIELD:
    return AURORA_OK;
  case AURORA_SYSCALL_BACKGROUND: {
    if (!current_process || arg0 || arg1 || arg2) return AURORA_EINVAL;
    if (user->background) return AURORA_OK;
    if (user->callback_active || user->event_defer_depth || background[user_console])
      return AURORA_EBUSY;
    /* No context migration: only terminal ownership changes, with IE clear.
     * Abandon foreground input before allowing SYSTEM to consume new keys. */
    user->background=1u;
    background[user_console]=current_process;
    foreground[user_console]=0u;
    user->callback=(aurora_keyboard_callback_t)0;
    user->callback_trampoline=user->subscribed_vector=0u;
    user->telnet_callback=(aurora_telnet_callback_t)0;
    user->telnet_trampoline=0u;
    user->event_read=user->event_write;
    user->wake_pending=0u;
    if(user_console) telnet_read=telnet_write;
    else keyboard_read=keyboard_write;
    input_swallow_lf[user_console]=1u;
    console_prompt[user_console]=1u;
    console_banner[user_console]=0u;
    return AURORA_OK;
  }
  case AURORA_SYSCALL_SLEEP:
    if (!current_process || arg0 > 0x7fffffffu || arg1 || arg2)
      return AURORA_EINVAL;
    if (user->callback_active || user->event_defer_depth) return AURORA_EBUSY;
    return AURORA_OK;
  case AURORA_SYSCALL_WAIT_EVENT:
    if (!current_process) return AURORA_EINVAL;
    if (user->callback_active || user->event_defer_depth) return AURORA_EBUSY;
    if (!user->callback && !user->telnet_callback)
      return AURORA_EINVAL;
    return AURORA_OK;
  case AURORA_SYSCALL_EVENT_DROPS:
    return (aurora_status_t)user->event_drops;
  case AURORA_SYSCALL_DEFER_EVENTS:
    if (user->event_defer_depth == 0xffffffffu) return AURORA_EINVAL;
    ++user->event_defer_depth;
    return AURORA_OK;
  case AURORA_SYSCALL_RESUME_EVENTS:
    if (user->event_defer_depth == 0u) return AURORA_EINVAL;
    --user->event_defer_depth;
    return AURORA_OK;
  case AURORA_SYSCALL_CONSOLE_PUTINT:
    if (!user->background) kernel_console_put_int(arg0);
    return AURORA_OK;
  case AURORA_SYSCALL_CONSOLE_PUTSTRING:
    return kernel_console_put_string(arg0, user->background);
  case AURORA_SYSCALL_CONSOLE_WRITE:
    return kernel_console_write(arg0, arg1, user->background);
  case AURORA_SYSCALL_TIME_MS:
    return (aurora_status_t)aurora.Read(AURORA_TIMER_DEVICE);
  case AURORA_SYSCALL_GRAPHICS_FILL_RECT: {
    aurora_status_t status=kernel_graphics_legacy_rect(arg0,arg1,arg2);
    if(status==AURORA_OK) graphics_owner=current_process;
    return status;
  }
  case AURORA_SYSCALL_GRAPHICS_RECT16: {
    aurora_status_t status=kernel_graphics_fill_rect(arg0,arg1,arg2);
    if(status==AURORA_OK) graphics_owner=current_process;
    return status;
  }
  case AURORA_SYSCALL_GRAPHICS_BUSY:
    return aurora.Read(AURORA_GRAPHICS_STATUS_DEVICE) != 0u;
  case AURORA_SYSCALL_GRAPHICS_BITMAP: {
    aurora_status_t status=kernel_graphics_bitmap(arg0,arg1,arg2,0u);
    if(status==AURORA_OK) graphics_owner=current_process;
    return status;
  }
  case AURORA_SYSCALL_GRAPHICS_IMAGE: {
    aurora_status_t status=kernel_graphics_bitmap(arg0,arg1,arg2,1u);
    if(status==AURORA_OK) graphics_owner=current_process;
    return status;
  }
  case AURORA_SYSCALL_MIDI_NOTE_ON:
    if (midi_count[0] || midi_count[1]) return AURORA_EBUSY;
    if(arg0>127u || arg1>127u) return AURORA_EINVAL;
    midi_owner=current_process;
    return kernel_midi_note_on(arg0, arg1);
  case AURORA_SYSCALL_MIDI_NOTE_OFF:
    if (midi_count[0] || midi_count[1]) return AURORA_EBUSY;
    return kernel_midi_note_off(arg0);
  case AURORA_SYSCALL_MIDI_PROGRAM:
    if(arg0>127u) return AURORA_EINVAL;
    midi_owner=current_process;
    return kernel_midi_program(arg0);
  case AURORA_SYSCALL_MIDI_ALL_NOTES_OFF:
    return kernel_midi_all_notes_off();
  case AURORA_SYSCALL_MIDI_SEQUENCE: {
    aurora_status_t status=kernel_midi_sequence(arg0,arg1,arg2);
    if(status==AURORA_OK) midi_owner=current_process;
    return status;
  }
  case AURORA_SYSCALL_MIDI_SEQUENCE_BUSY:
    kernel_midi_service();
    return (midi_count[0] != 0u) + 2u * (midi_count[1] != 0u);
  case AURORA_SYSCALL_TELNET_PUTCHAR:
  case AURORA_SYSCALL_TELNET_NEXT:
  case AURORA_SYSCALL_TELNET_SUBSCRIBE:
  case AURORA_SYSCALL_TELNET_WRITE:
    return AURORA_EBUSY; /* Device reserved for remote system console. */
  case AURORA_SYSCALL_CALLBACK_RETURN:
    if (!current_process || !user->callback_active)
      return AURORA_EINVAL;
    return AURORA_OK;
  case AURORA_SYSCALL_PROCESS_EXIT:
    if (!current_process)
      return AURORA_EINVAL;
    finish_user_exit(arg0);
    return AURORA_OK;
  default:
    return AURORA_ENOSYS;
  }
}

/* Every syscall now saves the same 18-word frame as a hardware IRQ.
 * Select by the saved REQUEST, never by a magic return value (TimeMs is u32).
 * The C helper finishes on its current stack before the naked stub switches. */
__attribute__((noinline, optnone, used)) static aurora_word_t
finish_user_syscall(aurora_status_t result, aurora_word_t stack_top) {
  volatile aurora_process_context *user = &process_table[current_process];
  volatile aurora_process_context *system = &process_table[PROCESS_SYSTEM];
  aurora_word_t frame = stack_top - AURORA_INTERRUPT_FRAME_WORDS;
  aurora_word_t request = *(volatile aurora_word_t *)(unsigned long)(frame + 2u);
  if (request == AURORA_SYSCALL_CALLBACK_RETURN && result == AURORA_OK)
    return take_callback_restore_stack_top();
  *(volatile aurora_word_t *)(unsigned long)(frame + 2u) = (aurora_word_t)result;
  if (result == AURORA_OK && current_process == PROCESS_SYSTEM &&
      request == AURORA_SYSCALL_YIELD)
    return aurora_schedule_from_tick(stack_top,0u);
  if (result != AURORA_OK || !current_process) return stack_top;
  if (request == AURORA_SYSCALL_PROCESS_EXIT) {
    current_process = PROCESS_SYSTEM;
    return system->ksp;
  }
  if (request == AURORA_SYSCALL_RESUME_EVENTS && !user->event_defer_depth) {
    /* The caller has finished its critical region; pending work may run now. */
    user->callback_budget = 1u;
    aurora_word_t top = prepare_pending_event(stack_top);
    return top != 0u ? top : stack_top;
  }
  aurora_word_t sleep_ms = 0u;
  if (request == AURORA_SYSCALL_SLEEP)
    sleep_ms = *(volatile aurora_word_t *)(unsigned long)(frame + 3u); /* saved r1 */
  if (request == AURORA_SYSCALL_WAIT_EVENT ||
      (request == AURORA_SYSCALL_SLEEP && sleep_ms != 0u)) {
    if (user->wake_pending || user->event_read != user->event_write) {
      user->wake_pending = 0u;
      user->callback_cooldown = 0u;
      user->callback_budget = 1u;
      aurora_word_t top = prepare_pending_event(stack_top);
      return top != 0u ? top : stack_top;
    }
    user->sleep_ms = sleep_ms;
    if (sleep_ms != 0u) user->sleep_start_ms = aurora.Read(AURORA_TIMER_DEVICE);
    user->waiting = 1u;
  } else if (request != AURORA_SYSCALL_YIELD && request != AURORA_SYSCALL_SLEEP &&
             request != AURORA_SYSCALL_BACKGROUND) {
    return stack_top;
  }
  if (!system->valid) {
    user->waiting = 0u;
    user->sleep_ms = 0u;
    return stack_top;
  }
  return return_to_system(stack_top);
}

__attribute__((naked, noinline)) void aurora_syscall_entry(void) {
  __asm__ volatile(
    "  di\n"
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
    "  call kernel_syscall_dispatch\n"
    "  osp r1\n"
    "  call finish_user_syscall\n"
    ".globl aurora_restore_frame\n"
    "aurora_restore_frame:\n"
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
