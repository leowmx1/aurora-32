#include <aurora.h>

AURORA_KERNEL_PROGRAM;
__asm__(".globl __aurora_rom_entry\n"
        "__aurora_rom_entry:\n"
        "  jmp _start\n");

#define GR_BASE 0x09000000u
#define GR_DATA 0x09600000u
#define GR_STAT 0x09900000u
#define TIMER 0x08000000u
#include "text_data.h"
#define BASE_JOB_COUNT 14u

/* Bit zero is the leftmost pixel. Enlarge 5x7 glyphs by 7 horizontally,
 * 6 vertically: 35x42 = 1470 bits = 46 RAM words, with no row padding. */
/* Prepacked so the hardware test spends no time rasterizing pixels on CPU. */
static const aurora_u32 data_a[46] = {
  0x0fffff80u,0x7ffffc00u,0xffffe000u,0xffff0003u,0xfff8001fu,0xffc000ffu,
  0x01fc07ffu,0x0fffc000u,0x7ffe0000u,0xfff00000u,0xff800003u,0xfc00001fu,
  0xe00000ffu,0x000007ffu,0x00003fffu,0x0001fff8u,0x000fffc0u,0x007ffe00u,
  0x03fff000u,0xffff8000u,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,
  0xffffffffu,0xffffffffu,0x00007fffu,0x0003fff0u,0x001fff80u,0x00fffc00u,
  0x07ffe000u,0x3fff0000u,0xfff80000u,0xffc00001u,0xfe00000fu,0xf000007fu,
  0x800003ffu,0x00001fffu,0x0000fffcu,0x0007ffe0u,0x003fff00u,0x01fff800u,
  0x0fffc000u,0x7ffe0000u,0xfff00000u,0xff800003u
};
static const aurora_u32 data_f[46] = {
  0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,
  0x01ffffffu,0x0fe00000u,0x7f000000u,0xf8000000u,0xc0000003u,0x0000001fu,
  0x000000feu,0x000007f0u,0x00003f80u,0x0001fc00u,0x000fe000u,0x007f0000u,
  0x03f80000u,0xffc00000u,0xfe03ffffu,0xf01fffffu,0x80ffffffu,0x07ffffffu,
  0x3ffffffcu,0xffffffe0u,0x00007f01u,0x0003f800u,0x001fc000u,0x00fe0000u,
  0x07f00000u,0x3f800000u,0xfc000000u,0xe0000001u,0x0000000fu,0x0000007fu,
  0x000003f8u,0x00001fc0u,0x0000fe00u,0x0007f000u,0x003f8000u,0x01fc0000u,
  0x0fe00000u,0x7f000000u,0xf8000000u,0xc0000003u
};
static const aurora_u32 data_edges[35] = {
  0xffffffffu,0xffffffffu,0x80000003u,0x80000001u,0x80000007u,0x80000001u,
  0x8000000du,0x80000001u,0x80000019u,0x80000001u,0x80000031u,0x80000001u,
  0x80000061u,0x80000001u,0x800000c1u,0x80000001u,0x80000181u,0x80000001u,
  0x80000301u,0x80000001u,0x80000601u,0x80000001u,0x80000c01u,0x80000001u,
  0x80001801u,0x80000001u,0x80003001u,0x80000001u,0x80006001u,0x80000001u,
  0x8000c001u,0x80000001u,0xffff8001u,0xffffffffu,0xffffffffu
};
static aurora_u32 job;
/* x,y,w,h,foreground,background,mode,shape. A table also avoids outgoing
 * stack arguments unsupported by some versions of the Aurora backend. */
static const aurora_u32 jobs[BASE_JOB_COUNT][8] = {
  {0,0,480,240,0,0,0,0},
  {16,88,35,42,2,4,1,1},
  {80,84,28,50,3,0,0,0}, {108,84,30,50,5,0,0,0},
  {90,88,35,42,1,4,2,2}, {90,88,35,42,2,2,2,0},
  {16,202,65,17,1,0,1,3},
  {12,158,84,26,2,0,0,0}, {108,158,84,26,3,0,0,0},
  {204,158,84,26,4,0,0,0}, {300,158,168,26,5,0,0,0},
  {276,22,192,38,1,0,0,0}, {12,68,456,1,4,0,0,0},
  {420,202,48,18,3,0,0,0}
};

__attribute__((noreturn)) static void idle(void) {
  for (;;) aurora.Nop(); /* Preserve picture and keep the circuit clock alive. */
}

static void wait_done(void) {
  aurora_u32 start = aurora.Read(TIMER);
  while (aurora.Read(GR_STAT)) {
    if (aurora.Read(TIMER)-start >= 10000u) {
      aurora.io.PutString("Data RAM test TIMEOUT, job ");
      if (job>=10u) aurora.io.PutChar('0'+job/10u);
      aurora.io.PutChar('0'+job%10u);
      aurora.io.PutString(". No further writes.\n");
      idle();
    }
    aurora.Nop();
  }
}

/* shape: 0=zero; 1=A; 2=F; 3=65x17 border + word-boundary markers.
 * Unused bits in the last word, and the next RAM word, are deliberately 1:
 * the renderer must stop at w*h, not draw trailing/padded bits. */
__attribute__((noinline, optnone)) static void upload(aurora_u32 shape,
                                                    aurora_u32 w,
                                                    aurora_u32 h) {
  aurora_u32 pixels=w*h;
  aurora_u32 words=(pixels+31u)/32u;
  for (aurora_u32 n=0u;n<words;++n) {
    aurora_u32 data=0u;
    if (shape==1u) data=data_a[n];
    if (shape==2u) data=data_f[n];
    if (shape==3u) data=data_edges[n];
    if (shape>=4u) data=text_data[text_offsets[shape-4u]+n];
    if (n+1u==words && pixels%32u) data|=0xffffffffu<<(pixels%32u);
    aurora.Write(GR_DATA+n,data);
  }
  aurora.Write(GR_DATA+words,0xffffffffu);
}

__attribute__((noinline, optnone)) static void draw(void) {
  const aurora_u32 *p=job<BASE_JOB_COUNT?jobs[job]:extra_jobs[job-BASE_JOB_COUNT];
  wait_done();
  aurora.Write(GR_STAT,0u);
  for (aurora_u32 r=0u;r<6u;++r)
    aurora.Write(GR_BASE+(r<<20u),p[r]);
  if (p[6]) upload(p[7],p[2],p[3]);
  /* Offset 7 is unused. Never write the former datahigh register. */
  aurora.Write(GR_BASE+(8u<<20u),p[6]);
  aurora.Write(GR_STAT,1u);
  wait_done();
  ++job;
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  aurora.DisableInterrupt();
  job=0u;
  aurora.io.PutString("Graphics 480x240 test: kernel only, mode 1/2.\n");
  /* Do not cancel an in-flight device job if ROM is changed without reset. */
  while (job<BASE_JOB_COUNT+EXTRA_JOB_COUNT) draw();
  aurora.io.PutString("Data RAM test DONE. No redraw.\n"
                      "Small text: 6x8 cells. Large letters and rectangles.\n"
                      "Bottom row: 80 digits. Black frame on all four edges.\n"
                      "Pixel (479,239) red. Compare the preview.\n");
  idle();
}
