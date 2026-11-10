#ifndef AURORA_KERNEL_SHUTDOWN_H
#define AURORA_KERNEL_SHUTDOWN_H
/* Emergency renderer: IRQs stay OFF from cleanup through HALT.
 * Never use the console renderer here: its EI could resume a failed user.
 * GPU and uptime timer advance independently of CPU interrupt delivery. */
enum { STOP_BLUE=0x8015u, STOP_WHITE=0xffffu, STOP_BLACK=0x8000u };
static volatile aurora_word_t stop_graphics_failed;
static const aurora_word_t stop_font[36][6] = {
  {0xf0000000u,0xc30c1980u,0x3643c430u,0x0c30c33cu,0x00f01983u,0x00000000u}, /* 0 */
  {0x60000000u,0x006c0780u,0x06006006u,0x60060060u,0x03fc0600u,0x00000000u}, /* 1 */
  {0xf8000000u,0x01801880u,0x18018018u,0x300600c0u,0x03fc0180u,0x00000000u}, /* 2 */
  {0xf8000000u,0x01801800u,0x0f008018u,0x00300180u,0x00fc1803u,0x00000000u}, /* 3 */
  {0xc0000000u,0x01a01e01u,0x1981981bu,0xfe18418cu,0x01801807u,0x00000000u}, /* 4 */
  {0xf8000000u,0x80080081u,0x1800f800u,0x00300300u,0x007c1801u,0x00000000u}, /* 5 */
  {0xe0000000u,0xc0180301u,0x31c1fc00u,0x0c30c30cu,0x00f01983u,0x00000000u}, /* 6 */
  {0xfc000000u,0x01803003u,0x0c00c018u,0x30060060u,0x00180300u,0x00000000u}, /* 7 */
  {0xf0000000u,0xc30c1980u,0x0f019810u,0x0c30c198u,0x00f838c3u,0x00000000u}, /* 8 */
  {0xf0000000u,0xc10c1880u,0x38c30c30u,0x80100378u,0x00780c01u,0x00000000u}, /* 9 */
  {0x60000000u,0x00d00f00u,0x18819809u,0x043fc108u,0x06062043u,0x00000000u}, /* A */
  {0xfc000000u,0xc30c18c0u,0x0fc18c10u,0x0c30c38cu,0x00fc18c3u,0x00000000u}, /* B */
  {0xe0000000u,0xc00c2181u,0x00c00c00u,0x0c00c00cu,0x01f02180u,0x00000000u}, /* C */
  {0xfc000000u,0xc30c18c0u,0x20c30c30u,0x0c30c30cu,0x007c18c3u,0x00000000u}, /* D */
  {0xf8000000u,0x80080081u,0x1f800800u,0x08008008u,0x01f80080u,0x00000000u}, /* E */
  {0xf8000000u,0x80080081u,0x1f800800u,0x08008008u,0x00080080u,0x00000000u}, /* F */
  {0xf0000000u,0xc00c2181u,0x3c400400u,0x0c30c304u,0x03f03183u,0x00000000u}, /* G */
  {0x0c000000u,0xc30c30c3u,0x3fc30c30u,0x0c30c30cu,0x030c30c3u,0x00000000u}, /* H */
  {0xfc000000u,0x00600601u,0x06006006u,0x60060060u,0x01fc0600u,0x00000000u}, /* I */
  {0xf8000000u,0x01801801u,0x18018018u,0x80180180u,0x00780c81u,0x00000000u}, /* J */
  {0x0c000000u,0xc0cc18c3u,0x03c02c06u,0xcc06c02cu,0x030c18c0u,0x00000000u}, /* K */
  {0x18000000u,0x80180180u,0x01801801u,0x18018018u,0x03f80180u,0x00000000u}, /* L */
  {0x0c000000u,0x429c38c3u,0x26425429u,0x04204264u,0x06062062u,0x00000000u}, /* M */
  {0x0c000000u,0xc31c31c3u,0x36c32c33u,0x8c34c34cu,0x030c38c3u,0x00000000u}, /* N */
  {0xf0000000u,0x430c1980u,0x20620430u,0x0c304206u,0x00f01983u,0x00000000u}, /* O */
  {0xfc000000u,0xc30c18c0u,0x18c30c30u,0x0c00c0fcu,0x000c00c0u,0x00000000u}, /* P */
  {0xf0000000u,0x430c1980u,0x20620430u,0x0c304204u,0x00f019c3u,0x3c044006u}, /* Q */
  {0xfc000000u,0xc10c18c0u,0x0fc18c10u,0x8c18c0ccu,0x030c30c1u,0x00000000u}, /* R */
  {0xf0000000u,0xc00c1180u,0x0f003800u,0x00300180u,0x00f81843u,0x00000000u}, /* S */
  {0xfc000000u,0x00600603u,0x06006006u,0x60060060u,0x00600600u,0x00000000u}, /* T */
  {0x0c000000u,0xc30c30c3u,0x30c30c30u,0x0c30c30cu,0x00f01883u,0x00000000u}, /* U */
  {0x06000000u,0xc30c2066u,0x19810830u,0xf0090198u,0x00600f00u,0x00000000u}, /* V */
  {0x06000000u,0x42042066u,0x26426420u,0x942d4264u,0x038c39c2u,0x00000000u}, /* W */
  {0x0c000000u,0x019810c3u,0x0600700fu,0x981d80f0u,0x030e30c1u,0x00000000u}, /* X */
  {0x06000000u,0x810c30c6u,0x0f009019u,0x60060060u,0x00600600u,0x00000000u}, /* Y */
  {0xfc000000u,0x01801003u,0x0600400cu,0x18030030u,0x03fc0080u,0x00000000u}, /* Z */
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
  aurora_word_t x=48u;
  while(*text && x+12u<=480u) {
    aurora_word_t c=*text++, index=36u;
    if(c>='0' && c<='9') index=c-'0';
    else if(c>='A' && c<='Z') index=c-'A'+10u;
    if(index<36u) {
      if(!stop_wait()) return;
      aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE,0u);
      aurora.Write(AURORA_GRAPHICS_X_DEVICE,x);
      aurora.Write(AURORA_GRAPHICS_Y_DEVICE,y);
      aurora.Write(AURORA_GRAPHICS_W_DEVICE,12u);
      aurora.Write(AURORA_GRAPHICS_H_DEVICE,16u);
      aurora.Write(AURORA_GRAPHICS_COLOR_DEVICE,STOP_WHITE);
      aurora.Write(AURORA_GRAPHICS_BG_DEVICE,STOP_BLUE);
      for(aurora_word_t word=0u;word<6u;++word)
        aurora.Write(AURORA_GRAPHICS_DATA_DEVICE+word,stop_font[index][word]);
      aurora.Write(AURORA_GRAPHICS_MODE_DEVICE,1u);
      aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE,1u);
    }
    x+=16u;
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
  if(current_process) shell_console=user_console;
  for(aurora_word_t pid=1u;pid<PROCESS_COUNT;++pid) mark_user_not_runnable(pid);
  kernel_midi_all_notes_off();
  aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE,0u);
  graphics_owner=midi_owner=0u;
  current_process=PROCESS_SYSTEM;
  stop_graphics_failed=0u;
}
__attribute__((noreturn)) static void kernel_shutdown(void) {
  stop_begin();
  stop_rect(0u,480u+65536u*240u,STOP_BLACK);
  stop_halt();
}
__attribute__((noreturn)) static void kernel_blue_screen(aurora_word_t vector,int user) {
  stop_begin();
  stop_rect(0u,480u+65536u*240u,STOP_BLUE);
  /* Large :(, constructed from seven small white rectangles. */
  stop_rect(72u+65536u*26u,8u+65536u*8u,STOP_WHITE);
  stop_rect(72u+65536u*52u,8u+65536u*8u,STOP_WHITE);
  stop_rect(108u+65536u*16u,10u+65536u*8u,STOP_WHITE);
  stop_rect(100u+65536u*24u,8u+65536u*10u,STOP_WHITE);
  stop_rect(94u+65536u*34u,8u+65536u*18u,STOP_WHITE);
  stop_rect(100u+65536u*52u,8u+65536u*10u,STOP_WHITE);
  stop_rect(108u+65536u*62u,10u+65536u*8u,STOP_WHITE);
  stop_text(100u,user?"USER PROCESS FAULT":"KERNEL FAULT");
  stop_text(130u,vector==AURORA_IVT_USER_MEMORY_FAULT?"MEMORY VIOLATION":"ILLEGAL INSTRUCTION");
  stop_text(160u,vector==AURORA_IVT_USER_MEMORY_FAULT?"STOP 03000004":"STOP 03000005");
  stop_text(200u,"HALTED  PRESS RESET");
  kernel_boot_status("System halted. Reset to restart.");
  stop_halt(); /* leave the completed blue screen visible, do not clear black */
}
#endif
