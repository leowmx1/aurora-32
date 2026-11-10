#include <aurora.h>
AURORA_KERNEL_PROGRAM;
__asm__(".globl __aurora_rom_entry\n__aurora_rom_entry:\n  jmp _start\n");
#define GR_BASE 0x09000000u
#define GR_DATA 0x09600000u
#define GR_SCALE 0x09700000u
#define GR_MODE 0x09800000u
#define GR_STAT 0x09900000u
#define TIMER 0x08000000u
#include "glyph.h"

static aurora_u32 job;
/* x,y,OUTPUT width,height,foreground,background,mode,scale.
 * Source stride = output width * max(scale,1) = 48 in every bitmap.
 * All dimensions divide exactly, so this test has no rounding ambiguity. */
static const aurora_u32 jobs[25][8]={
  {0,0,480,240,0x98e8,0,0,1},
  {16,24,48,72,1,0,1,0},
  {96,24,48,72,1,0,1,1},
  {176,24,24,36,1,0,1,2},
  {256,24,16,24,1,0,1,3},
  {336,24,12,18,1,0,1,4},
  {416,24,8,12,1,0,1,6},
  {16,136,24,72,3,0,0,1}, {40,136,24,72,5,0,0,1},
  {16,136,48,72,2,0,2,0},
  {96,136,24,72,3,0,0,1}, {120,136,24,72,5,0,0,1},
  {96,136,48,72,2,0,2,1},
  {176,136,12,36,3,0,0,1}, {188,136,12,36,5,0,0,1},
  {176,136,24,36,2,0,2,2},
  {256,136,8,24,3,0,0,1}, {264,136,8,24,5,0,0,1},
  {256,136,16,24,2,0,2,3},
  {336,136,6,18,3,0,0,1}, {342,136,6,18,5,0,0,1},
  {336,136,12,18,2,0,2,4},
  {416,136,4,12,3,0,0,1}, {420,136,4,12,5,0,0,1},
  {416,136,8,12,2,0,2,6}
};
__attribute__((noreturn)) static void idle(void) {
  for(;;) aurora.Nop();
}
static void wait_done(void) {
  aurora_u32 start=aurora.Read(TIMER);
  while(aurora.Read(GR_STAT)) {
    if(aurora.Read(TIMER)-start>=10000u) {
      aurora.io.PutString("Scale test TIMEOUT. No further device writes.\n");
      idle();
    }
    aurora.Nop();
  }
}
__attribute__((noinline,optnone)) static void draw(void) {
  const aurora_u32 *p=jobs[job];
  wait_done();
  aurora.Write(GR_STAT,0);
  for(aurora_u32 i=0;i<6;++i) aurora.Write(GR_BASE+(i<<20u),p[i]);
  aurora.Write(GR_SCALE,p[7]); /* Write raw 0: the hardware must default to 1. */
  aurora.Write(GR_MODE,p[6]);
  aurora.Write(GR_STAT,1);
  wait_done();
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  aurora.DisableInterrupt();
  aurora.io.PutString("Graphics SCALE test: kernel only, 480x240.\n"
    "One 48x72 A uploaded ONCE (108 words).\n"
    "Columns left to right: 0, 1, 2, 3, 4, 6.\n"
    "Top: mode 1. Bottom: mode 2 on green/yellow.\n");
  job=0; draw();
  /* No CPU resampling and no smaller font tables. Reuse this RAM for all jobs. */
  for(aurora_u32 i=0;i<108u;++i) aurora.Write(GR_DATA+i,letter_a[i]);
  for(job=1;job<25u;++job) draw();
  aurora.Write(GR_SCALE,1); /* Leave a neutral scale for a subsequent ROM load. */
  aurora.io.PutString("SCALE DONE. 0 must equal 1. No redraw.\n"
    "Output sizes: 48x72, 48x72, 24x36, 16x24, 12x18, 8x12.\n");
  idle();
}
