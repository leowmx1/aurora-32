#ifndef AURORA_SYSTEM_H
#define AURORA_SYSTEM_H

#include "aurora_user.h"

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

static __inline__ __attribute__((always_inline, noreturn)) void
__aurora_system_exit(int exit_code) {
  aurora_process_exit(exit_code);
}

/* One character per hardware word; use a global/static UROM/URAM buffer.
 * No terminator is needed. At most 64 characters per call, low 8 bits each.
 * Splitting a frame into chunks allows interrupts between system calls. */
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_system_write(const aurora_word_t *text, aurora_word_t count) {
  return aurora_syscall(AURORA_SYSCALL_CONSOLE_WRITE,
                        (aurora_word_t)(unsigned long)text, count, 0);
}

static __inline__ __attribute__((always_inline)) aurora_word_t
__aurora_system_time_ms(void) {
  return (aurora_word_t)aurora_syscall(AURORA_SYSCALL_TIME_MS, 0, 0, 0);
}

static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_telnet_putchar(char c) {
  return aurora_syscall(AURORA_SYSCALL_TELNET_PUTCHAR, (aurora_word_t)(unsigned int)c, 0, 0);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_telnet_getchar(void) {
  return aurora_syscall(AURORA_SYSCALL_TELNET_NEXT, 0, 0, 0);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_telnet_write(const aurora_word_t *text, aurora_word_t count) {
  return aurora_syscall(AURORA_SYSCALL_TELNET_WRITE,
                        (aurora_word_t)(unsigned long)text, count, 0);
}

#include "aurora_buffer.h"

typedef struct aurora_system_io_api {
  aurora_status_t (*PutChar)(char character);
  aurora_status_t (*GetChar)(void);
  aurora_status_t (*PutInt)(int value);
  aurora_status_t (*PutString)(const char *text);
  aurora_status_t (*Write)(const aurora_word_t *text, aurora_word_t count);
  aurora_system_buffer_api buffer;
} aurora_system_io_api;

typedef struct aurora_system_telnet_api {
  aurora_status_t (*PutChar)(char character);
  aurora_status_t (*GetChar)(void);
  aurora_status_t (*Write)(const aurora_word_t *text, aurora_word_t count);
  aurora_status_t (*Subscribe)(aurora_telnet_callback_t callback);
} aurora_system_telnet_api;

typedef struct aurora_system_api {
  aurora_system_io_api io;
  void (*Exit)(int exit_code);
  aurora_word_t (*TimeMs)(void);
  aurora_system_telnet_api telnet;
} aurora_system_api;

static const aurora_system_api system = {{
    __aurora_system_put_char,
    __aurora_system_get_char,
    __aurora_system_put_int,
    __aurora_system_put_string,
    __aurora_system_write,
    {__aurora_buffer_begin, __aurora_buffer_char, __aurora_buffer_string,
     __aurora_buffer_unsigned, __aurora_buffer_flush, __aurora_buffer_report_error},
}, __aurora_system_exit, __aurora_system_time_ms,
{__aurora_telnet_putchar, __aurora_telnet_getchar, __aurora_telnet_write, aurora_telnet_subscribe}};

#endif
