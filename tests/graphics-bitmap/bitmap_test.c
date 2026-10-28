#include <aurora.h>

AURORA_KERNEL_PROGRAM;

/* Reset PC=0 must enter the startup code, not a C helper. */
__asm__(".globl __aurora_rom_entry\n"
        "__aurora_rom_entry:\n"
        "  jmp _start\n");

#define GR_BASE 0x09000000u
#define GR_STAT 0x09000009u
#define TIMER 0x08000000u
#define JOBS 14u

/* x,y,w,h,c,bgc,dataLow,dataHigh,mode.
 * Bit i maps to (i % w, i / w), low word first; bitmap area <= 64.
 * Rectangle mode ignores bitmap data and background color. */
static const aurora_u32 jobs[JOBS][9] = {
  {0,0,160,100, 0,2,0,0,0},                 /* 0: white background */
  {16,12,24,24, 4,2,0,0,0},                /* 1: blue rectangle */
  {48,12,24,24, 3,2,0,0,0},                /* 2: opaque bitmap backing */
  {96,12,12,24, 3,2,0,0,0},                /* 3: transparent backing left */
  {108,12,12,24, 5,2,0,0,0},               /* 4: transparent backing right */
  {56,20,8,8, 2,4,0x1f01013fu,0x00010101u,1}, /* 5: red F / blue background */
  {104,20,8,8, 2,4,0x1f01013fu,0x00010101u,2},/* 6: F, keep green/yellow */
  {16,56,8,8, 2,4,0xffffffffu,0,1},        /* 7: top red, bottom blue */
  {40,56,8,8, 3,4,0,0xffffffffu,1},        /* 8: top blue, bottom green */
  {64,56,8,8, 1,0,0x80000000u,1,1},       /* 9: bit31/32 across word boundary */
  {84,56,5,7, 1,0,1,4,1},                 /* 10: bit0/34, non-eight row stride */
  {104,20,8,8, 1,1,0,0,2},                /* 11: transparent zero = no change */
  {96,56,24,16, 3,2,0,0,0},               /* 12: back to rectangle mode */
  {159,99,1,1, 2,4,1,0,2}                 /* 13: last pixel, bit0 only */
};

__attribute__((noreturn)) static void idle(void) {
  /* Keep the circuit clock running; do not redraw or HALT. */
  for (;;) aurora.Nop();
}

static void print_unsigned(aurora_u32 value) {
  if (value >= 10u) aurora.io.PutChar('0' + value / 10u);
  aurora.io.PutChar('0' + value % 10u);
}

static void wait_done(aurora_u32 job) {
  aurora_u32 start = aurora.Read(TIMER);
  while (aurora.Read(GR_STAT) != 0u) {
    if (aurora.Read(TIMER) - start >= 10000u) {
      aurora.io.PutString("Bitmap smoke TIMEOUT at job ");
      print_unsigned(job);
      aurora.io.PutString(". No more commands.\n");
      idle();
    }
    aurora.Nop();
  }
}

__attribute__((noinline, optnone)) static void draw_job(aurora_u32 job) {
  wait_done(job);
  aurora.Write(GR_STAT, 0u);
  for (aurora_u32 reg = 0u; reg < 9u; ++reg)
    aurora.Write(GR_BASE + reg, jobs[job][reg]);
  aurora.Write(GR_STAT, 1u);
  wait_done(job);
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  aurora.DisableInterrupt();
  /* Independent kernel-only ROM: no OS, UROM, ISR, scheduling or MIDI needed. */
  aurora.Write(GR_STAT, 0u);
  for (aurora_u32 job = 0u; job < JOBS; ++job) draw_job(job);
  aurora.io.PutString("Bitmap smoke DONE. Check screen.\n"
                      "Top: blue rectangle; opaque F; transparent F.\n"
                      "Bottom: low/high words; bit31/32; 5x7 corners; rectangle.\n"
                      "Bottom-right pixel is red. No redraw.\n");
  idle();
}
