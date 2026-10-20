#ifndef AURORA_KBQUEUE_H
#define AURORA_KBQUEUE_H

#include "aurora_system.h"

/* These are queue results, not syscall error codes. Characters are 0..255. */
#define AURORA_KBQUEUE_EMPTY (-1)
#define AURORA_KBQUEUE_OVERFLOW (-2)
#define AURORA_KBQUEUE_CAPACITY 63u

/* One non-reentrant IRQ callback producer, one foreground consumer.
 * Header-only singleton for the current single-translation-unit user build.
 * The unused slot distinguishes full from empty. */
static volatile aurora_word_t __aurora_kbqueue_data[64]
    __attribute__((section(".noinit")));
static volatile aurora_word_t __aurora_kbqueue_read;
static volatile aurora_word_t __aurora_kbqueue_write;
static volatile aurora_word_t __aurora_kbqueue_overflow;
static aurora_word_t __aurora_kbqueue_sequence;

static void __aurora_kbqueue_callback(aurora_word_t event) {
  aurora_word_t write = __aurora_kbqueue_write;
  aurora_word_t next = (write + 1u) & 63u;
  if (next == __aurora_kbqueue_read) {
    __aurora_kbqueue_overflow = 1u;
    return;
  }
  __aurora_kbqueue_data[write] = event;
  __aurora_kbqueue_write = next;
}

/* Call once at startup, before consuming input. Owns the keyboard subscription;
 * do not replace it with another callback or mix in GetChar/device polling. */
static aurora_status_t __aurora_kbqueue_init(void) {
  __aurora_kbqueue_read = __aurora_kbqueue_write = 0u;
  __aurora_kbqueue_overflow = __aurora_kbqueue_sequence = 0u;
  return aurora_keyboard_subscribe(__aurora_kbqueue_callback);
}

/* Nonblocking. Overflow discards pending events; invalidate partial input.
 * This only reads the user-memory queue, never the keyboard device. */
static int __aurora_kbqueue_next(void) {
  if (__aurora_kbqueue_overflow) {
    __aurora_kbqueue_overflow = 0u;
    __aurora_kbqueue_read = __aurora_kbqueue_write;
    return AURORA_KBQUEUE_OVERFLOW;
  }
  for (unsigned n = 0; n < 64u; ++n) {
    aurora_word_t read = __aurora_kbqueue_read;
    if (read == __aurora_kbqueue_write) return AURORA_KBQUEUE_EMPTY;
    aurora_word_t event = __aurora_kbqueue_data[read];
    __aurora_kbqueue_read = (read + 1u) & 63u;
    if ((event & 0xff000000u) != 0xa5000000u) continue;
    aurora_word_t seq = event & 0x00ffff00u;
    if (seq == 0u || seq == __aurora_kbqueue_sequence) continue;
    __aurora_kbqueue_sequence = seq;
    return (int)(event & 0xffu);
  }
  return AURORA_KBQUEUE_EMPTY;
}

typedef struct aurora_kbqueue_api {
  aurora_status_t (*Init)(void);
  int (*Next)(void);
} aurora_kbqueue_api;

static const aurora_kbqueue_api kbqueue = {
  __aurora_kbqueue_init, __aurora_kbqueue_next
};

#endif
