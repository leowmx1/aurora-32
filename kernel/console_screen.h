#ifndef AURORA_KERNEL_CONSOLE_SCREEN_H
#define AURORA_KERNEL_CONSOLE_SCREEN_H
/* Kernel-owned 480x240 console wallpaper. Large 24x32 glyphs, thin regular letterforms.
 * No active user is runnable while this renderer owns the graphics device. */
enum { LOGO_BACKGROUND = 0x98e8u, LOGO_TEXT = 0xf39au, LOGO_ACCENT = 0xb6d6u };
static const aurora_word_t logo_glyphs[9][24] = {
  {0x00000000u,0x00000000u,0x00000000u,0x00003c00u,0x7e00007eu,0x007e0000u,0x0000f700u,0xe70000e7u,0x01e38000u,0x8001c380u,0xc1c001c3u,0x0381c003u,0xe00381c0u,0x80e00780u,0x07ffe007u,0xf00ffff0u,0x00700fffu,0x1e00780eu,0x381e0038u,0x003c1c00u,0x0000003cu,0x00000000u,0x00000000u,0x00000000u}, /* A */
  {0x00000000u,0x00000000u,0x00000000u,0x700e0070u,0x00700e00u,0x0e00700eu,0x700e0070u,0x00700e00u,0x0e00700eu,0x700e0070u,0x00700e00u,0x0e00700eu,0x700e0070u,0x00700e00u,0x0e00700eu,0x700e0070u,0x00f00f00u,0x0781e007u,0xc003ffe0u,0x7f0001ffu,0x00000000u,0x00000000u,0x00000000u,0x00000000u}, /* U */
  {0x00000000u,0x00000000u,0x00000000u,0xe0007fe0u,0xffe001ffu,0x03c0e003u,0xe00780e0u,0x80e00780u,0x0780e007u,0xe00380e0u,0xffe003e0u,0x007fe001u,0xe0007fe0u,0xe0e000f0u,0x01e0e001u,0xe003c0e0u,0x80e003c0u,0x0780e003u,0xe00700e0u,0x00e00f00u,0x0000000eu,0x00000000u,0x00000000u,0x00000000u}, /* R */
  {0x00000000u,0x00000000u,0x00000000u,0x80007e00u,0xffc001ffu,0x07c1e003u,0xf00f00e0u,0x00700f00u,0x1e00780eu,0x381e0078u,0x00381e00u,0x1c00381eu,0x381e0038u,0x00781e00u,0x0e00781eu,0x700e0070u,0x00f00f00u,0x07c1e007u,0x8003ffc0u,0x7e0001ffu,0x00000000u,0x00000000u,0x00000000u,0x00000000u}, /* O */
  {0x00000000u,0x00000000u,0x00000000u,0xe0007fe0u,0xffe001ffu,0x03c0e003u,0xe00780e0u,0x80e00780u,0x0780e007u,0xe00380e0u,0xffe003e0u,0x007fe001u,0xe0007fe0u,0xe0e000f0u,0x01e0e001u,0xe003c0e0u,0x80e003c0u,0x0780e003u,0xe00700e0u,0x00e00f00u,0x0000000eu,0x00000000u,0x00000000u,0x00000000u}, /* R */
  {0x00000000u,0x00000000u,0x00000000u,0x00003c00u,0x7e00007eu,0x007e0000u,0x0000f700u,0xe70000e7u,0x01e38000u,0x8001c380u,0xc1c001c3u,0x0381c003u,0xe00381c0u,0x80e00780u,0x07ffe007u,0xf00ffff0u,0x00700fffu,0x1e00780eu,0x381e0038u,0x003c1c00u,0x0000003cu,0x00000000u,0x00000000u,0x00000000u}, /* A */
  {0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x8001ff80u,0xff8001ffu,0x00000001u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u,0x00000000u}, /* - */
  {0x00000000u,0x00000000u,0x00000000u,0xe0003f80u,0xffe000ffu,0x03c04001u,0x0003c000u,0x80000380u,0x03800003u,0x0003c000u,0xff0001e0u,0x007f0000u,0x0001ff00u,0x800003e0u,0x07000007u,0x00070000u,0x80000700u,0x03c00007u,0xe001ffe0u,0x3fe000ffu,0x00000000u,0x00000000u,0x00000000u,0x00000000u}, /* 3 */
  {0x00000000u,0x00000000u,0x00000000u,0xc0007f00u,0xffe000ffu,0x03c0e001u,0x00038040u,0x80000380u,0x03800007u,0x00038000u,0xc0000380u,0x01e00003u,0x0000f000u,0x3c000078u,0x001e0000u,0x80000f00u,0x03c00007u,0x0001e000u,0xe00fffe0u,0xffe00fffu,0x0000000fu,0x00000000u,0x00000000u,0x00000000u}, /* 2 */
};
static int console_screen_wait(void) {
  aurora_word_t start = aurora.Read(AURORA_TIMER_DEVICE), polls = 0u;
  while (aurora.Read(AURORA_GRAPHICS_STATUS_DEVICE)) {
    if (aurora.Read(AURORA_TIMER_DEVICE) - start >= 10000u || ++polls >= 1000000u) {
      aurora.DisableInterrupt();
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
static void console_screen_draw_logo(void) {
  if (!console_screen_rect(0u, 480u + 65536u * 240u, LOGO_BACKGROUND)) return;
  for (aurora_word_t i = 0; i < 9u; ++i) {
    if (!console_screen_wait()) return;
    aurora.DisableInterrupt();
    aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE, 0u);
    aurora.Write(AURORA_GRAPHICS_X_DEVICE, 108u + i * 30u);
    aurora.Write(AURORA_GRAPHICS_Y_DEVICE, 96u);
    aurora.Write(AURORA_GRAPHICS_W_DEVICE, 24u);
    aurora.Write(AURORA_GRAPHICS_H_DEVICE, 32u);
    aurora.Write(AURORA_GRAPHICS_COLOR_DEVICE, LOGO_TEXT);
    aurora.Write(AURORA_GRAPHICS_BG_DEVICE, LOGO_BACKGROUND);
    for (aurora_word_t word=0u; word<24u; ++word)
      aurora.Write(AURORA_GRAPHICS_DATA_DEVICE+word, logo_glyphs[i][word]);
    aurora.Write(AURORA_GRAPHICS_MODE_DEVICE, 1u);
    aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE, 1u);
    aurora.EnableInterrupt();
  }
  if (!console_screen_rect(108u + 65536u * 143u, 264u + 65536u * 2u, LOGO_ACCENT)) return;
  (void)console_screen_wait();
}
static void console_screen_logo(void) {
  aurora.DisableInterrupt();
  if(graphics_owner) { aurora.EnableInterrupt(); return; }
  graphics_owner=0xffffffffu;
  graphics_logo_needed=0u;
  aurora.EnableInterrupt();
  console_screen_draw_logo();
  aurora.DisableInterrupt();
  graphics_owner=0u;
  aurora.EnableInterrupt();
}
#endif
