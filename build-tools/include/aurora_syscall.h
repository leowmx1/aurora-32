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
typedef void (*aurora_telnet_callback_t)(aurora_word_t event);

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
  AURORA_IVT_TICK = 0x03000007u,
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
  /* r1 contains the signed 32-bit process exit code.  This never returns. */
  AURORA_SYSCALL_PROCESS_EXIT = 8,
  /* r1=user word buffer, r2=character count (0..64); result=count. */
  AURORA_SYSCALL_CONSOLE_WRITE = 9,
  /* Raw unsigned milliseconds modulo 2^32, not a signed status code. */
  AURORA_SYSCALL_TIME_MS = 10,
  AURORA_SYSCALL_TELNET_PUTCHAR = 11,
  AURORA_SYSCALL_TELNET_NEXT = 12,
  AURORA_SYSCALL_TELNET_SUBSCRIBE = 13,
  AURORA_SYSCALL_TELNET_WRITE = 14,
  AURORA_SYSCALL_WAIT_EVENT = 15,
  AURORA_SYSCALL_EVENT_DROPS = 16,
  AURORA_SYSCALL_DEFER_EVENTS = 17,
  AURORA_SYSCALL_RESUME_EVENTS = 18,
  /* r1=x+256*y, r2=width+256*height, r3=hardware color value.
   * Non-blocking: 0 means accepted, not completed. */
  AURORA_SYSCALL_GRAPHICS_FILL_RECT = 19,
  /* 0=idle, 1=busy. No acknowledgement or other side effect. */
  AURORA_SYSCALL_GRAPHICS_BUSY = 20,
  /* 21 (former graphics subscription) is retired; returns ENOSYS. */
  /* Single hardware-selected MIDI channel; all parameters are 0..127. */
  AURORA_SYSCALL_MIDI_NOTE_ON = 22,  /* r1=note, r2=velocity; 0=release */
  AURORA_SYSCALL_MIDI_NOTE_OFF = 23, /* r1=note */
  AURORA_SYSCALL_MIDI_PROGRAM = 24,  /* r1=zero-based instrument */
  AURORA_SYSCALL_MIDI_ALL_NOTES_OFF = 25,
  /* r1=word array [note,velocity,hold_ms,gap_ms] repeated r2 times (1..64).
   * r3=0 once, 1 loop background, 2 foreground effect; note128=rest. */
  AURORA_SYSCALL_MIDI_SEQUENCE = 26,
  /* Bit0=background exists, bit1=effect exists. No completion callback. */
  AURORA_SYSCALL_MIDI_SEQUENCE_BUSY = 27,
  /* r1=eight-word UROM/URAM descriptor, r2=1 opaque/2 transparent, r3=0.
   * Returns 0 when accepted, not when drawing has completed. */
  AURORA_SYSCALL_GRAPHICS_BITMAP = 28,
  /* r1=milliseconds (0..INT32_MAX), r2=r3=0. Event-interruptible.
   * 0 yields; positive values block until tick timeout or a subscribed event. */
  AURORA_SYSCALL_SLEEP = 29,
};

#define AURORA_GRAPHICS_WIDTH 160u
#define AURORA_GRAPHICS_HEIGHT 100u

/* Use static/global storage, never a user-stack local descriptor.
 * Row-major, bit0=top-left; width*height <=64. No scaling or tiling. */
typedef struct aurora_graphics_bitmap {
  aurora_word_t x, y, width, height;
  aurora_word_t color, background;
  aurora_word_t data_low, data_high;
} aurora_graphics_bitmap;

#define AURORA_CONSOLE_WRITE_MAX 64u

enum {
  AURORA_OK = 0,
  AURORA_EINVAL = -1,
  AURORA_EAGAIN = -2,
  AURORA_ENOSYS = -3,
  AURORA_EFAULT = -4,
  AURORA_EBUSY = -5,
};

#endif
