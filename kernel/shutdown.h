#ifndef AURORA_KERNEL_SHUTDOWN_H
#define AURORA_KERNEL_SHUTDOWN_H
/* Emergency renderer: IRQs stay OFF from cleanup through HALT.
 * Never use the console renderer here: its EI could resume a failed user.
 * GPU and uptime timer advance independently of CPU interrupt delivery. */
enum { STOP_BLUE=0x8015u, STOP_WHITE=0xffffu, STOP_BLACK=0x8000u };
static volatile aurora_word_t stop_graphics_failed;
static const aurora_word_t stop_font[36][2] = {
{0xf3cf3fffu, 0x0fffcf3cu}, /* 0 */
  {0x0c3cf30cu, 0x0fff30c3u}, /* 1 */
  {0xffc30fffu, 0x0fff0c3fu}, /* 2 */
  {0xffc30fffu, 0x0fffc30fu}, /* 3 */
  {0xffcf3cf3u, 0x0c30c30fu}, /* 4 */
  {0xff0c3fffu, 0x0fffc30fu}, /* 5 */
  {0xff0c3fffu, 0x0fffcf3fu}, /* 6 */
  {0x30c30fffu, 0x0c30c30cu}, /* 7 */
  {0xffcf3fffu, 0x0fffcf3fu}, /* 8 */
  {0xffcf3fffu, 0x0fffc30fu}, /* 9 */
  {0xffcf330cu, 0x0cf3cf3fu}, /* A */
  {0xcfcf33cfu, 0x03cfcf33u}, /* B */
  {0xc30c3fffu, 0x0fff0c30u}, /* C */
  {0xf3cf33cfu, 0x03cfcf3cu}, /* D */
  {0xcf0c3fffu, 0x0fff0c33u}, /* E */
  {0xcf0c3fffu, 0x00c30c33u}, /* F */
  {0xf30c3fffu, 0x0fffcf3cu}, /* G */
  {0xffcf3cf3u, 0x0cf3cf3fu}, /* H */
  {0x0c30cfffu, 0x0fff30c3u}, /* I */
  {0x30c30c30u, 0x0fffcf3cu}, /* J */
  {0xcfcf3cf3u, 0x0cf3cf33u}, /* K */
  {0xc30c30c3u, 0x0fff0c30u}, /* L */
  {0xfffffcf3u, 0x0cf3cf3fu}, /* M */
  {0xfffffcf3u, 0x0cf3ffffu}, /* N */
  {0xf3cf3fffu, 0x0fffcf3cu}, /* O */
  {0xffcf3fffu, 0x00c30c3fu}, /* P */
  {0xf3cf3fffu, 0x0c30fffcu}, /* Q */
  {0xcfcf33cfu, 0x0cf3cf33u}, /* R */
  {0xff0c3fffu, 0x0fffc30fu}, /* S */
  {0x0c30cfffu, 0x030c30c3u}, /* T */
  {0xf3cf3cf3u, 0x0fffcf3cu}, /* U */
  {0xf3cf3cf3u, 0x030ccf3cu}, /* V */
  {0xffcf3cf3u, 0x0cf3ffffu}, /* W */
  {0x0ccf3cf3u, 0x0cf3cf33u}, /* X */
  {0x0ccf3cf3u, 0x030c30c3u}, /* Y */
  {0x0cc30fffu, 0x0fff0c33u}, /* Z */
};
static int stop_wait(void) {
  if(stop_graphics_failed) return 0;
  aurora_word_t start=aurora.Read(AURORA_TIMER_DEVICE), polls=0u;
  while(aurora.Read(AURORA_GRAPHICS_STATUS_DEVICE)) {
    if(aurora.Read(AURORA_TIMER_DEVICE)-start>=10000u || ++polls>=1000000u) {
      stop_graphics_failed=1u;
      aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE,0u);
      kernel_boot_status("Graphics unavailable during shutdown.");
      return 0;
    }
    aurora.Nop();
  }
  return 1;
}
static void stop_rect(aurora_word_t position,aurora_word_t size,aurora_word_t color) {
  if(!stop_wait()) return;
  (void)kernel_graphics_fill_rect(position,size,color);
}
__attribute__((noinline,optnone)) static void stop_text(aurora_word_t y,const char *text) {
  aurora_word_t x=10u;
  while(*text && x+6u<=160u) {
    aurora_word_t c=*text++, index=36u;
    if(c>='0' && c<='9') index=c-'0';
    else if(c>='A' && c<='Z') index=c-'A'+10u;
    if(index<36u) {
      if(!stop_wait()) return;
      aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE,0u);
      aurora.Write(AURORA_GRAPHICS_X_DEVICE,x);
      aurora.Write(AURORA_GRAPHICS_Y_DEVICE,y);
      aurora.Write(AURORA_GRAPHICS_W_DEVICE,6u);
      aurora.Write(AURORA_GRAPHICS_H_DEVICE,10u);
      aurora.Write(AURORA_GRAPHICS_COLOR_DEVICE,STOP_WHITE);
      aurora.Write(AURORA_GRAPHICS_BG_DEVICE,STOP_BLUE);
      aurora.Write(AURORA_GRAPHICS_LOW_DEVICE,stop_font[index][0]);
      aurora.Write(AURORA_GRAPHICS_HIGH_DEVICE,stop_font[index][1]);
      aurora.Write(AURORA_GRAPHICS_MODE_DEVICE,1u);
      aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE,1u);
    }
    x+=7u;
  }
}
__attribute__((noreturn)) static void stop_halt(void) {
  (void)stop_wait(); /* including the last bitmap/full-screen fill */
  aurora.DisableInterrupt();
  aurora.Halt();
  for(;;) aurora.Nop();
}
static void stop_begin(void) {
  aurora.DisableInterrupt();
  if(current_process==PROCESS_USER) shell_console=user_console;
  mark_user_not_runnable(); /* cancel MIDI, pending GPU work and user callbacks */
  current_process=PROCESS_SYSTEM;
  stop_graphics_failed=0u;
}
__attribute__((noreturn)) static void kernel_shutdown(void) {
  stop_begin();
  stop_rect(0u,160u+256u*100u,STOP_BLACK);
  stop_halt();
}
__attribute__((noreturn)) static void kernel_blue_screen(aurora_word_t vector,int user) {
  stop_begin();
  stop_rect(0u,160u+256u*100u,STOP_BLUE);
  /* Large :(, constructed from seven small white rectangles. */
  stop_rect(12u+256u*13u,4u+256u*4u,STOP_WHITE);
  stop_rect(12u+256u*26u,4u+256u*4u,STOP_WHITE);
  stop_rect(30u+256u*8u,5u+256u*4u,STOP_WHITE);
  stop_rect(26u+256u*12u,4u+256u*5u,STOP_WHITE);
  stop_rect(23u+256u*17u,4u+256u*9u,STOP_WHITE);
  stop_rect(26u+256u*26u,4u+256u*5u,STOP_WHITE);
  stop_rect(30u+256u*31u,5u+256u*4u,STOP_WHITE);
  stop_text(43u,user?"USER PROCESS FAULT":"KERNEL FAULT");
  stop_text(57u,vector==AURORA_IVT_USER_MEMORY_FAULT?"MEMORY VIOLATION":"ILLEGAL INSTRUCTION");
  stop_text(71u,vector==AURORA_IVT_USER_MEMORY_FAULT?"STOP 03000004":"STOP 03000005");
  stop_text(85u,"HALTED  PRESS RESET");
  kernel_boot_status("System halted. Reset to restart.");
  stop_halt(); /* leave the completed blue screen visible, do not clear black */
}
#endif
