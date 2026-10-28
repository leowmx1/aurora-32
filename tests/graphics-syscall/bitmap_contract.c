#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;

/* Const and mutable packets; the backend may materialize both in URAM. */
static const aurora_graphics_bitmap f = {56,20,8,8,2,4,0x1f01013fu,0x00010101u};
static aurora_graphics_bitmap tile;
static aurora_word_t saved_tail[8];
static volatile aurora_word_t callbacks;
static void check(int ok, int code) {
  if (!ok) {
    system.io.PutString("BITMAP FAIL "); system.io.PutInt(code); system.Exit(code);
  }
}
static void key(aurora_word_t event) {
  check(aurora_event_character(event) == 'k', 90);
  check(system.graphics.Bitmap(&f) == AURORA_EBUSY, 91);
  check(system.graphics.FillRect(0,0,1,1,2) == AURORA_EBUSY, 92);
  ++callbacks;
}
static void wait_idle(void) {
  aurora_word_t start = system.TimeMs();
  for (;;) {
    int busy = system.graphics.Busy();
    check(busy >= 0, 93);
    if (!busy) return;
    if (system.TimeMs() - start >= 10000u) {
      system.io.PutString("Bitmap timeout (10 s).\n"); system.Exit(1);
    }
    system.Yield();
  }
}
static void invalid(aurora_word_t address) {
  check(aurora_syscall(28, address, 1, 0) == AURORA_EFAULT, 10);
}
static void bitmap(aurora_word_t mode) {
  int result;
  if (mode == 1u) result = system.graphics.Bitmap(&tile);
  else result = system.graphics.BitmapTransparent(&tile);
  check(result == 0, 30);
  /* Kernel must not retain the descriptor for deferred consumption. */
  tile.data_low = 0; tile.data_high = 0; tile.color = 1;
  wait_idle();
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  invalid(0); invalid(0x01000000u); invalid(0x09000000u);
  invalid(0x82000000u); invalid(0xfffffffcu);
  invalid(0x80fffff9u); invalid(0x81fffff9u);
  check(aurora_syscall(28, (aurora_word_t)&f, 0, 0) == AURORA_EINVAL, 11);
  check(aurora_syscall(28, (aurora_word_t)&f, 3, 0) == AURORA_EINVAL, 12);
  check(aurora_syscall(28, (aurora_word_t)&f, 1, 1) == AURORA_EINVAL, 13);
  tile.width = 8; tile.height = 9;
  check(system.graphics.Bitmap(&tile) == AURORA_EINVAL, 14);
  tile.width = 0; tile.height = 1;
  check(system.graphics.Bitmap(&tile) == AURORA_EINVAL, 15);
  tile.width = 1; tile.height = 0;
  check(system.graphics.Bitmap(&tile) == AURORA_EINVAL, 16);
  tile.height = 1; tile.x = 0xffffffffu;
  check(system.graphics.Bitmap(&tile) == AURORA_EINVAL, 17);
  tile.x = 159; tile.width = 2;
  check(system.graphics.Bitmap(&tile) == AURORA_EINVAL, 18);
  tile.x = 0; tile.y = 99; tile.height = 2;
  check(system.graphics.Bitmap(&tile) == AURORA_EINVAL, 19);
  tile.y = 0; tile.width = 0xffffffffu;
  check(system.graphics.Bitmap(&tile) == AURORA_EINVAL, 20);
  check(system.io.Subscribe(key) == 0, 21);
  check(system.graphics.FillRect(0,0,160,100,0) == 0, 22); wait_idle();
  check(system.graphics.FillRect(96,12,12,24,3) == 0, 23); wait_idle();
  check(system.graphics.FillRect(108,12,12,24,5) == 0, 24); wait_idle();
  check(system.graphics.Bitmap(&f) == 0, 25); wait_idle();
  tile.x = 104; tile.y = 20; tile.width = 8; tile.height = 8;
  tile.color = 2; tile.background = 4;
  tile.data_low = 0x1f01013fu; tile.data_high = 0x00010101u;
  bitmap(2);
  /* All-zero transparent mask must preserve the just-drawn F and backing. */
  bitmap(2);
  tile.x = 16; tile.y = 56; tile.color = 2;
  tile.data_low = 0xffffffffu; tile.data_high = 0; bitmap(1);
  tile.x = 40; tile.color = 3;
  tile.data_low = 0; tile.data_high = 0xffffffffu; bitmap(1);
  tile.x = 64; tile.color = 1; tile.background = 0;
  tile.data_low = 0x80000000u; tile.data_high = 1; bitmap(1);
  tile.x = 84; tile.width = 5; tile.height = 7; tile.color = 1;
  tile.data_low = 1; tile.data_high = 4; bitmap(1);
  /* Returning to rectangle mode must ignore stale bitmap/color state. */
  check(system.graphics.FillRect(96,56,24,16,3) == 0, 26); wait_idle();
  tile.x = 159; tile.y = 99; tile.width = 1; tile.height = 1;
  tile.color = 2; tile.data_low = 1; tile.data_high = 0; bitmap(2);
  /* Exact end-of-bank packets: eight words ending at the last URAM word. */
  /* Globals/strings are allocated downward from URAM's end: preserve them. */
  for (aurora_word_t i = 0; i < 8u; ++i)
    saved_tail[i] = aurora.Read(0x81fffff8u + i);
  for (aurora_word_t i = 0; i < 8u; ++i)
    aurora.Write(0x81fffff8u + i, ((aurora_word_t *)&f)[i]);
  check(aurora_syscall(28, 0x81fffff8u, 1, 0) == 0, 27); wait_idle();
  for (aurora_word_t i = 0; i < 8u; ++i)
    aurora.Write(0x81fffff8u + i, saved_tail[i]);
  system.io.PutString("BITMAP CONTRACT PASS\n");
  system.Exit(0);
  for (;;) system.Yield();
}
