#include <aurora.h>

AURORA_KERNEL_PROGRAM;

/* Standalone ROM image: reset always enters _start, not a helper. */
__asm__(
    ".globl __aurora_rom_entry\n"
    "__aurora_rom_entry:\n"
    "  jmp _start\n");

static void print_unsigned(aurora_u32 value) {
  aurora_u32 digits[10];
  aurora_u32 count = 0u;
  do {
    digits[count++] = value % 10u;
    value /= 10u;
  } while (value != 0u);
  while (count != 0u)
    aurora.io.PutChar('0' + digits[--count]);
}

/* Each call completes before any register for the next rectangle is changed. */
static aurora_u32 start_and_wait(void) {
  aurora_u32 start_ms = aurora.Read(0x08000000u);
  aurora.Write(0x09000008u, 0u); /* rectangle mode */
  aurora.Write(0x09000009u, 1u);
  while (aurora.Read(0x09000009u) != 0u)
    aurora.Nop();
  return aurora.Read(0x08000000u) - start_ms;
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  /* This hardware smoke test has no scheduler or installed IRQ handlers. */
  aurora.DisableInterrupt();

  /* Stop first so the engine cannot use partially updated parameters. */
  aurora.Write(0x09000009u, 0u);  /* status/control: stop */

  /* Centered rectangle on a 160x100 screen: x=40..119, y=25..74.
   * No background clear: pixels outside the rectangle must stay unchanged. */
  aurora.Write(0x09000000u, 40u);  /* x */
  aurora.Write(0x09000001u, 25u);  /* y */
  aurora.Write(0x09000002u, 80u);  /* width, in pixels */
  aurora.Write(0x09000003u, 50u);  /* height, in pixels */
  aurora.Write(0x09000004u, 4u);   /* Digital palette index 4: blue */
  aurora_u32 blue_ms = start_and_wait();

  /* Top left: x=5..29, y=5..19. No overlap with the blue rectangle. */
  aurora.Write(0x09000009u, 0u);
  aurora.Write(0x09000000u, 5u);
  aurora.Write(0x09000001u, 5u);
  aurora.Write(0x09000002u, 25u);
  aurora.Write(0x09000003u, 15u);
  aurora.Write(0x09000004u, 2u);   /* red */
  aurora_u32 red_ms = start_and_wait();

  /* Bottom right: x=125..154, y=80..94. All three must remain visible. */
  aurora.Write(0x09000009u, 0u);
  aurora.Write(0x09000000u, 125u);
  aurora.Write(0x09000001u, 80u);
  aurora.Write(0x09000002u, 30u);
  aurora.Write(0x09000003u, 15u);
  aurora.Write(0x09000004u, 3u);   /* green */
  aurora_u32 green_ms = start_and_wait();

  /* Print only after all three fills: no console delay between commands. */
  aurora.io.PutString("Fill complete: x=40 y=25 w=80 h=50 blue, ");
  print_unsigned(blue_ms);
  aurora.io.PutString(" ms.\n");
  aurora.io.PutString("Fill complete: x=5 y=5 w=25 h=15 red, ");
  print_unsigned(red_ms);
  aurora.io.PutString(" ms.\n");
  aurora.io.PutString("Fill complete: x=125 y=80 w=30 h=15 green, ");
  print_unsigned(green_ms);
  aurora.io.PutString(" ms.\n");

  /* Do not redraw blue: that would hide corruption caused by later fills.
   * Run this sequence once only, keeping the clock running afterward. */
  for (;;)
    aurora.Nop();
}
