#ifndef AURORA_USER_H
#define AURORA_USER_H

#include "aurora_syscall.h"

/* Supplied by build.ps1 after it has located aurora_syscall_entry in ROM. */
#ifndef AURORA_SYSCALL_ENTRY_ADDRESS
#error "Build user programs with build.ps1 so the syscall ROM address is known"
#endif

#define AURORA_STRINGIFY_IMPL(value) #value
#define AURORA_STRINGIFY(value) AURORA_STRINGIFY_IMPL(value)

/* UROM always starts at word address 0x08000000.  Keep its first instruction
 * independent of C function emission order: helpers such as the callback
 * trampoline are allowed to appear before _start in the object file.  Aurora
 * assembly symbols are byte based, hence 0x20000000 bytes = 0x08000000 words. */
__asm__(
    ".globl __aurora_urom_entry\n"
    "__aurora_urom_entry:\n"
    "  jmp _start+536870912\n");

/*
 * A hardware-delivered callback returns here with an ordinary RET. The INT
 * asks the kernel to discard the callback context and restore the complete
 * user context that was interrupted. A valid request never returns here.
 */
__attribute__((naked, noinline, used, noreturn)) static void
__aurora_callback_return_trampoline(void) {
  __asm__ volatile(
      "  ldi r0, 0x00000007\n"
      "  int " AURORA_STRINGIFY(AURORA_SYSCALL_ENTRY_ADDRESS) "\n"
      ".L__aurora_callback_return_failed:\n"
      "  nop\n"
      "  jmp .L__aurora_callback_return_failed\n");
}

/*
 * The named register variables make this exactly match the Aurora ABI:
 * r0=request/return value, r1-r3=arguments.  INT uses the direct ROM
 * address of aurora_syscall_entry; IRET resumes here in user mode with r0.
 */
static __inline__ __attribute__((always_inline)) aurora_status_t
aurora_syscall(aurora_word_t request, aurora_word_t arg0,
               aurora_word_t arg1, aurora_word_t arg2) {
  register aurora_word_t r0 __asm__("r0") = request;
  register aurora_word_t r1 __asm__("r1") = arg0;
  register aurora_word_t r2 __asm__("r2") = arg1;
  register aurora_word_t r3 __asm__("r3") = arg2;
  /* INT atomically clears IE while entering the kernel, so user code must
   * not execute DI before making a system call. */
  __asm__ volatile("int " AURORA_STRINGIFY(AURORA_SYSCALL_ENTRY_ADDRESS)
                   : "+r"(r0)
                   : "r"(r1), "r"(r2), "r"(r3)
                   : "memory");
  return (aurora_status_t)r0;
}

static __inline__ __attribute__((always_inline)) aurora_status_t
aurora_console_putchar(aurora_word_t character) {
  return aurora_syscall(AURORA_SYSCALL_CONSOLE_PUTCHAR, character, 0, 0);
}

static __inline__ __attribute__((always_inline)) aurora_status_t
aurora_keyboard_subscribe(aurora_keyboard_callback_t callback) {
  return aurora_syscall(AURORA_SYSCALL_KEYBOARD_SUBSCRIBE,
                        (aurora_word_t)(unsigned long)callback,
                        (aurora_word_t)(unsigned long)
                            __aurora_callback_return_trampoline,
                        AURORA_IVT_KEYBOARD);
}

/* Subscribe a no-return user dispatch entry.  The entry is reached directly
 * by IRET and must finish with AURORA_SYSCALL_CALLBACK_RETURN; because it has
 * no caller, the kernel does not put a synthetic return address on USP. */
static __inline__ __attribute__((always_inline)) aurora_status_t
aurora_keyboard_subscribe_entry(aurora_keyboard_callback_t entry) {
  return aurora_syscall(AURORA_SYSCALL_KEYBOARD_SUBSCRIBE,
                        (aurora_word_t)(unsigned long)entry, 0u,
                        AURORA_IVT_KEYBOARD);
}

/* Return one queued keyboard word, or AURORA_EAGAIN when the queue is empty. */
static __inline__ __attribute__((always_inline)) aurora_status_t
aurora_keyboard_next(void) {
  return aurora_syscall(AURORA_SYSCALL_KEYBOARD_NEXT, 0, 0, 0);
}

/* Compatibility polling helper.  New programs normally subscribe once and
 * let the kernel enter the callback asynchronously; this remains useful when
 * no callback is registered and queued input is wanted explicitly. */
static __inline__ __attribute__((always_inline)) void
aurora_keyboard_dispatch_pending(aurora_keyboard_callback_t callback) {
  for (;;) {
    aurora_status_t character = aurora_keyboard_next();
    if (character == AURORA_EAGAIN)
      return;
    if (character < 0)
      return;
    callback((aurora_word_t)character);
  }
}

#endif
