#ifndef AURORA_KERNEL_CONSOLE_SCREEN_H
#define AURORA_KERNEL_CONSOLE_SCREEN_H
/* Kernel-owned 160x100 console wallpaper. Same 6x10 bitmap font as games.
 * No active user is runnable while this renderer owns the graphics device. */
enum { LOGO_BACKGROUND = 0x98e8u, LOGO_TEXT = 0xf39au, LOGO_ACCENT = 0xb6d6u };
static const aurora_word_t logo_glyphs[9][2] = {
  {0xffcf330cu,0x0cf3cf3fu}, /* A */
  {0xf3cf3cf3u,0x0fffcf3cu}, /* U */
  {0xcfcf33cfu,0x0cf3cf33u}, /* R */
  {0xf3cf3fffu,0x0fffcf3cu}, /* O */
  {0xcfcf33cfu,0x0cf3cf33u}, /* R */
  {0xffcf330cu,0x0cf3cf3fu}, /* A */
  {0xff000000u,0x0000000fu}, /* - */
  {0xffc30fffu,0x0fffc30fu}, /* 3 */
  {0xffc30fffu,0x0fff0c3fu}, /* 2 */
};
static int console_screen_wait(void) {
  aurora_word_t start = aurora.Read(AURORA_TIMER_DEVICE), polls = 0u;
  while (aurora.Read(AURORA_GRAPHICS_STATUS_DEVICE)) {
    if (aurora.Read(AURORA_TIMER_DEVICE) - start >= 10000u || ++polls >= 1000000u) {
      aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE, 0u);
      kernel_boot_status("Graphics timeout; console remains available."); return 0;
    }
    aurora.Nop(); /* IRQs enabled while waiting, never DI over busy wait. */
  }
  return 1;
}
static int console_screen_rect(aurora_word_t pos, aurora_word_t size, aurora_word_t color) {
  if (!console_screen_wait()) return 0;
  aurora.DisableInterrupt();
  aurora_status_t result = kernel_graphics_fill_rect(pos, size, color);
  aurora.EnableInterrupt(); return result == AURORA_OK;
}
static void console_screen_logo(void) {
  if (!console_screen_rect(0u, 160u + 256u * 100u, LOGO_BACKGROUND)) return;
  for (aurora_word_t i = 0; i < 9u; ++i) {
    if (!console_screen_wait()) return;
    aurora.DisableInterrupt();
    aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE, 0u);
    aurora.Write(AURORA_GRAPHICS_X_DEVICE, 45u + i * 8u);
    aurora.Write(AURORA_GRAPHICS_Y_DEVICE, 43u);
    aurora.Write(AURORA_GRAPHICS_W_DEVICE, 6u);
    aurora.Write(AURORA_GRAPHICS_H_DEVICE, 10u);
    aurora.Write(AURORA_GRAPHICS_COLOR_DEVICE, LOGO_TEXT);
    aurora.Write(AURORA_GRAPHICS_BG_DEVICE, LOGO_BACKGROUND);
    aurora.Write(AURORA_GRAPHICS_LOW_DEVICE, logo_glyphs[i][0]);
    aurora.Write(AURORA_GRAPHICS_HIGH_DEVICE, logo_glyphs[i][1]);
    aurora.Write(AURORA_GRAPHICS_MODE_DEVICE, 1u);
    aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE, 1u);
    aurora.EnableInterrupt();
  }
  if (!console_screen_rect(45u + 256u * 59u, 70u + 256u * 2u, LOGO_ACCENT)) return;
  (void)console_screen_wait();
}
#endif
