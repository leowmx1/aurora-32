#ifndef AURORA_SYSCALL_H
#define AURORA_SYSCALL_H

/*
 * Shared kernel/user ABI.  Every value is a 32-bit Aurora hardware word.
 * Software INT takes the direct ROM word address of the syscall entry; the
 * AURORA_IVT_* constants below are hardware interrupt vector addresses.
 */
typedef unsigned int aurora_word_t;
typedef int aurora_status_t;
typedef void (*aurora_keyboard_callback_t)(aurora_word_t character);

/* Compatibility with an Aurora compiler built before program-mode markers. */
#ifndef AURORA_KERNEL_PROGRAM
#define AURORA_KERNEL_PROGRAM
#define AURORA_USER_PROGRAM
#endif

enum {
  AURORA_IVT_KEYBOARD = 0x03000001u,
  AURORA_IVT_TELNET = 0x03000002u,
  AURORA_IVT_USER_MEMORY_FAULT = 0x03000004u,
  AURORA_IVT_USER_ILLEGAL_INSTRUCTION = 0x03000005u,
  AURORA_IVT_TIMER = 0x03000007u,
};

enum {
  AURORA_SYSCALL_ABI_VERSION = 0,
  AURORA_SYSCALL_CONSOLE_PUTCHAR = 1,
  AURORA_SYSCALL_KEYBOARD_SUBSCRIBE = 2,
  AURORA_SYSCALL_KEYBOARD_NEXT = 3,
  AURORA_SYSCALL_YIELD = 4,
  AURORA_SYSCALL_CONSOLE_PUTINT = 5,
  AURORA_SYSCALL_CONSOLE_PUTSTRING = 6,
  /* Used only by the user-mode callback return trampoline. */
  AURORA_SYSCALL_CALLBACK_RETURN = 7,
};

enum {
  AURORA_OK = 0,
  AURORA_EINVAL = -1,
  AURORA_EAGAIN = -2,
  AURORA_ENOSYS = -3,
  AURORA_EFAULT = -4,
};

#endif
