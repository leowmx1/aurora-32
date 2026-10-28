#include <aurora.h>
#include <aurora_system.h>

AURORA_USER_PROGRAM;

/* Static storage: syscall descriptors must not live on the user stack. */
static const aurora_graphics_bitmap opaque_f =
    {56u,40u,8u,8u,2u,5u,0x1f01013fu,0x00010101u};
static const aurora_graphics_bitmap transparent_f =
    {104u,40u,8u,8u,2u,5u,0x1f01013fu,0x00010101u};

static void fail(aurora_status_t error) {
  system.io.PutString("Graphics error: ");
  system.io.PutInt(error);
  system.io.PutChar('\n');
  system.Exit(1);
}

static void wait_idle(void) {
  aurora_word_t start = system.TimeMs();
  for (;;) {
    aurora_status_t busy = system.graphics.Busy();
    if (busy < 0) fail(busy);
    if (busy == 0) return;
    if (system.TimeMs() - start >= 10000u) {
      system.io.PutString("Graphics timeout (10 s).\n");
      system.Exit(1);
    }
    system.Yield();
  }
}

static void rectangle(aurora_word_t x, aurora_word_t y,
                      aurora_word_t w, aurora_word_t h, aurora_word_t color) {
  wait_idle();
  aurora_status_t result = system.graphics.FillRect(x, y, w, h, color);
  if (result != AURORA_OK) fail(result);
  wait_idle();
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  rectangle(40u, 25u, 80u, 50u, 4u);
  rectangle(5u, 5u, 25u, 15u, 2u);
  rectangle(125u, 80u, 30u, 15u, 3u);
  aurora_status_t result = system.graphics.Bitmap(&opaque_f);
  if (result != AURORA_OK) fail(result);
  wait_idle();
  result = system.graphics.BitmapTransparent(&transparent_f);
  if (result != AURORA_OK) fail(result);
  wait_idle();
  /* This rectangle also verifies that mode returns to zero after bitmaps. */
  rectangle(56u, 52u, 8u, 2u, 3u);
  system.io.PutString("Graphics complete: rectangles, opaque F, transparent F.\n");
  system.Exit(0);
  for (;;) system.Yield();
}
