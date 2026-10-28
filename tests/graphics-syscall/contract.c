#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
static volatile aurora_word_t keys;
static volatile aurora_word_t args[5] = {40u, 25u, 80u, 50u, 4u};
static void check(int ok, int code) {
  if (!ok) {
    system.io.PutString("GRAPHICS FAIL ");
    system.io.PutInt(code);
    system.Exit(code);
  }
}
static void key(aurora_word_t event) {
  check(aurora_event_character(event) == 'k', 40);
  check(system.graphics.Busy() == 1, 41);
  check(system.graphics.FillRect(0, 0, 1, 1, 2) == AURORA_EBUSY, 42);
  ++keys;
}
static void wait_idle(void) {
  aurora_word_t start = system.TimeMs();
  for (;;) {
    int busy = system.graphics.Busy();
    check(busy >= 0, 43);
    if (!busy) return;
    if (system.TimeMs() - start >= 10000u) {
      system.io.PutString("Graphics timeout (10 s).\n");
      system.Exit(1);
    }
    system.Yield();
  }
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  check(aurora_syscall(AURORA_SYSCALL_ABI_VERSION, 0, 0, 0) == 3, 1);
  check(system.graphics.Busy() == 0, 2);
  check(aurora_syscall(21, 0, 0, 0x03000003u) == AURORA_ENOSYS, 28);
  check(system.WaitEvent() == AURORA_EINVAL, 29);
  check(system.graphics.FillRect(-1, 0, 1, 1, 4) == AURORA_EINVAL, 3);
  check(system.graphics.FillRect(0, -1, 1, 1, 4) == AURORA_EINVAL, 4);
  check(system.graphics.FillRect(0, 0, 0, 1, 4) == AURORA_EINVAL, 5);
  check(system.graphics.FillRect(0, 0, 1, 0, 4) == AURORA_EINVAL, 6);
  check(system.graphics.FillRect(159, 99, 2, 1, 4) == AURORA_EINVAL, 7);
  check(system.graphics.FillRect(159, 99, 1, 2, 4) == AURORA_EINVAL, 8);
  check(system.graphics.FillRect(0, 0, 0xffffffffu, 1, 4) == AURORA_EINVAL, 9);
  check(system.graphics.FillRect(160, 0, 1, 1, 4) == AURORA_EINVAL, 10);
  check(system.graphics.FillRect(0, 100, 1, 1, 4) == AURORA_EINVAL, 11);
  /* Bypass wrappers: kernel must independently reject malformed packets. */
  check(aurora_syscall(19, 65536, 257, 4) == AURORA_EINVAL, 12);
  check(aurora_syscall(19, 0, 65536, 4) == AURORA_EINVAL, 13);
  check(aurora_syscall(19, 160, 257, 4) == AURORA_EINVAL, 14);
  check(aurora_syscall(19, 25600, 257, 4) == AURORA_EINVAL, 15);
  check(aurora_syscall(19, 0, 256, 4) == AURORA_EINVAL, 16);
  check(aurora_syscall(19, 0, 1, 4) == AURORA_EINVAL, 17);
  check(aurora_syscall(19, 159, 258, 4) == AURORA_EINVAL, 18);
  check(aurora_syscall(19, 25344, 513, 4) == AURORA_EINVAL, 19);
  check(system.io.Subscribe(key) == 0, 20);
  check(system.graphics.FillRect(0, 0, 160, 100, 0) == 0, 21);
  check(system.graphics.FillRect(1, 1, 1, 1, 2) == AURORA_EBUSY, 22);
  wait_idle();
  check(system.graphics.FillRect(args[0], args[1], args[2], args[3], args[4]) == 0, 23);
  wait_idle();
  check(system.graphics.FillRect(5, 5, 25, 15, 2) == 0, 24);
  wait_idle();
  check(system.graphics.FillRect(125, 80, 30, 15, 3) == 0, 25);
  wait_idle();
  check(system.graphics.FillRect(159, 99, 1, 1, 4) == 0, 26);
  wait_idle();
  check(keys == 5, 27);
  system.io.PutString("GRAPHICS CONTRACT PASS\n");
  system.Exit(0);
  for (;;) system.Yield();
}
