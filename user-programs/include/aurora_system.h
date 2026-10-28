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

/* Asynchronous submission; no implicit clear, clipping or retry. Validate
 * before packing so negative/large coordinates cannot wrap into valid ones.
 * The kernel independently validates the raw syscall arguments too. */
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_graphics_fill_rect(aurora_word_t x, aurora_word_t y,
                            aurora_word_t width, aurora_word_t height,
                            aurora_word_t color) {
  if (x >= AURORA_GRAPHICS_WIDTH || y >= AURORA_GRAPHICS_HEIGHT)
    return AURORA_EINVAL;
  if (width == 0u || height == 0u ||
      width > AURORA_GRAPHICS_WIDTH - x ||
      height > AURORA_GRAPHICS_HEIGHT - y) return AURORA_EINVAL;
  return aurora_syscall(AURORA_SYSCALL_GRAPHICS_FILL_RECT,
                        x + y * 256u, width + height * 256u, color);
}

static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_graphics_busy(void) {
  return aurora_syscall(AURORA_SYSCALL_GRAPHICS_BUSY, 0u, 0u, 0u);
}

/* The kernel validates/copies the packet; wrappers do not dereference it.
 * Data is consumed before return, so the caller may reuse its packet after
 * acceptance. The graphics engine itself remains asynchronous. */
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_graphics_bitmap(const aurora_graphics_bitmap *bitmap) {
  return aurora_syscall(AURORA_SYSCALL_GRAPHICS_BITMAP,
                        (aurora_word_t)(unsigned long)bitmap, 1u, 0u);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_graphics_bitmap_transparent(const aurora_graphics_bitmap *bitmap) {
  return aurora_syscall(AURORA_SYSCALL_GRAPHICS_BITMAP,
                        (aurora_word_t)(unsigned long)bitmap, 2u, 0u);
}

typedef struct aurora_system_graphics_api {
  aurora_status_t (*FillRect)(aurora_word_t x, aurora_word_t y,
                              aurora_word_t width, aurora_word_t height,
                              aurora_word_t color);
  aurora_status_t (*Busy)(void);
  aurora_status_t (*Bitmap)(const aurora_graphics_bitmap *bitmap);
  aurora_status_t (*BitmapTransparent)(const aurora_graphics_bitmap *bitmap);
} aurora_system_graphics_api;

/* Non-blocking musical commands, not audio samples. Duration is controlled
 * by the caller with TimeMs/Yield, followed by NoteOff. */
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_midi_note_on(aurora_word_t note, aurora_word_t velocity) {
  return aurora_syscall(AURORA_SYSCALL_MIDI_NOTE_ON, note, velocity, 0u);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_midi_note_off(aurora_word_t note) {
  return aurora_syscall(AURORA_SYSCALL_MIDI_NOTE_OFF, note, 0u, 0u);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_midi_program(aurora_word_t program) {
  return aurora_syscall(AURORA_SYSCALL_MIDI_PROGRAM, program, 0u, 0u);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_midi_all_notes_off(void) {
  return aurora_syscall(AURORA_SYSCALL_MIDI_ALL_NOTES_OFF, 0u, 0u, 0u);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_midi_sequence(const aurora_word_t *steps, aurora_word_t count,
                       aurora_word_t flags) {
  return aurora_syscall(AURORA_SYSCALL_MIDI_SEQUENCE,
                       (aurora_word_t)(unsigned long)steps, count, flags);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_midi_sequence_busy(void) {
  return aurora_syscall(AURORA_SYSCALL_MIDI_SEQUENCE_BUSY, 0u, 0u, 0u);
}
typedef struct aurora_system_midi_api {
  aurora_status_t (*NoteOn)(aurora_word_t note, aurora_word_t velocity);
  aurora_status_t (*NoteOff)(aurora_word_t note);
  aurora_status_t (*Program)(aurora_word_t program);
  aurora_status_t (*AllNotesOff)(void);
  aurora_status_t (*Sequence)(const aurora_word_t *steps, aurora_word_t count,
                              aurora_word_t flags);
  aurora_status_t (*SequenceBusy)(void);
} aurora_system_midi_api;

/* WaitEvent blocks only outside callbacks and deferred-delivery regions.
 * Yield remains runnable. Neither executes a privileged user instruction. */
#define AURORA_SYSTEM_EVENT_WRAPPER(name, request) \
static __inline__ __attribute__((always_inline)) aurora_status_t \
name(void) { return aurora_syscall(request, 0, 0, 0); }
AURORA_SYSTEM_EVENT_WRAPPER(__aurora_wait_event, AURORA_SYSCALL_WAIT_EVENT)
AURORA_SYSTEM_EVENT_WRAPPER(__aurora_yield, AURORA_SYSCALL_YIELD)
AURORA_SYSTEM_EVENT_WRAPPER(__aurora_defer_events, AURORA_SYSCALL_DEFER_EVENTS)
AURORA_SYSTEM_EVENT_WRAPPER(__aurora_resume_events, AURORA_SYSCALL_RESUME_EVENTS)
#undef AURORA_SYSTEM_EVENT_WRAPPER
/* An event may return early; recompute deadlines after each wake. No subscription
 * required. Like WaitEvent, forbidden inside callbacks/deferred regions.
 * Returns OK on timeout/event/yield, EINVAL for ms>INT32_MAX, EBUSY if unsafe. */
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_sleep(aurora_word_t ms) {
  return aurora_syscall(AURORA_SYSCALL_SLEEP, ms, 0u, 0u);
}
static __inline__ __attribute__((always_inline)) aurora_word_t
__aurora_event_drops(void) {
  return (aurora_word_t)aurora_syscall(AURORA_SYSCALL_EVENT_DROPS, 0, 0, 0);
}

typedef struct aurora_system_io_api {
  aurora_status_t (*PutChar)(char character);
  aurora_status_t (*GetChar)(void);
  aurora_status_t (*PutInt)(int value);
  aurora_status_t (*PutString)(const char *text);
  aurora_status_t (*Write)(const aurora_word_t *text, aurora_word_t count);
  aurora_system_buffer_api buffer;
  aurora_status_t (*Subscribe)(aurora_keyboard_callback_t callback);
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
  aurora_status_t (*WaitEvent)(void);
  aurora_status_t (*Yield)(void);
  aurora_word_t (*EventDrops)(void);
  aurora_status_t (*DeferEvents)(void);
  aurora_status_t (*ResumeEvents)(void);
  aurora_system_graphics_api graphics;
  aurora_system_midi_api midi;
  aurora_status_t (*Sleep)(aurora_word_t ms);
} aurora_system_api;

static const aurora_system_api system = {{
    __aurora_system_put_char,
    __aurora_system_get_char,
    __aurora_system_put_int,
    __aurora_system_put_string,
    __aurora_system_write,
    {__aurora_buffer_begin, __aurora_buffer_char, __aurora_buffer_string,
     __aurora_buffer_unsigned, __aurora_buffer_flush, __aurora_buffer_report_error},
    aurora_keyboard_subscribe,
}, __aurora_system_exit, __aurora_system_time_ms,
{__aurora_telnet_putchar, __aurora_telnet_getchar, __aurora_telnet_write, aurora_telnet_subscribe},
__aurora_wait_event, __aurora_yield, __aurora_event_drops,
__aurora_defer_events, __aurora_resume_events,
{__aurora_graphics_fill_rect, __aurora_graphics_busy,
 __aurora_graphics_bitmap, __aurora_graphics_bitmap_transparent},
{__aurora_midi_note_on, __aurora_midi_note_off,
 __aurora_midi_program, __aurora_midi_all_notes_off,
 __aurora_midi_sequence, __aurora_midi_sequence_busy}, __aurora_sleep};

#endif
