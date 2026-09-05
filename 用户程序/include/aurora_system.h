#ifndef AURORA_SYSTEM_H
#define AURORA_SYSTEM_H

#include "aurora_user.h"

typedef struct aurora_system_io_api {
  aurora_status_t (*PutChar)(char character);
  aurora_status_t (*GetChar)(void);
  aurora_status_t (*PutInt)(int value);
  aurora_status_t (*PutString)(const char *text);
} aurora_system_io_api;

typedef struct aurora_system_api {
  aurora_system_io_api io;
} aurora_system_api;

static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_system_put_char(char character) {
  return aurora_syscall(AURORA_SYSCALL_CONSOLE_PUTCHAR,
                        (aurora_word_t)(unsigned int)character, 0, 0);
}

/* Non-blocking: returns 0..255, or AURORA_EAGAIN if no key is queued. */
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_system_get_char(void) {
  return aurora_syscall(AURORA_SYSCALL_KEYBOARD_NEXT, 0, 0, 0);
}

static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_system_put_int(int value) {
  return aurora_syscall(AURORA_SYSCALL_CONSOLE_PUTINT,
                        (aurora_word_t)value, 0, 0);
}

static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_system_put_string(const char *text) {
  return aurora_syscall(AURORA_SYSCALL_CONSOLE_PUTSTRING,
                        (aurora_word_t)(unsigned long)text, 0, 0);
}

static const aurora_system_api system = {{
    __aurora_system_put_char,
    __aurora_system_get_char,
    __aurora_system_put_int,
    __aurora_system_put_string,
}};

#endif
