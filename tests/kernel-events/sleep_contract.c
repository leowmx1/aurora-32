#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
static volatile aurora_word_t keys, nets;
static const aurora_word_t tune[8] = {60,80,300,0, 64,80,300,0};
static void check(int ok, int code) {
  if (!ok) { system.io.PutString("SLEEP FAIL "); system.io.PutInt(code); system.Exit(code); }
}
static void key(aurora_word_t event) {
  check(system.Sleep(1) == AURORA_EBUSY, 80);
  check(system.Sleep(0) == AURORA_EBUSY, 81);
  ++keys;
}
static void net(aurora_word_t event) { ++nets; }
static void timed(aurora_word_t ms) {
  aurora_word_t start = system.TimeMs();
  check(system.Sleep(ms) == 0, 20);
  check(system.TimeMs() - start >= ms, 21);
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  check(system.Sleep(0) == 0, 1);
  check(system.Sleep(0x80000000u) == AURORA_EINVAL, 2);
  check(system.Sleep(0xffffffffu) == AURORA_EINVAL, 3);
  check(aurora_syscall(29, 1, 1, 0) == AURORA_EINVAL, 4);
  check(aurora_syscall(29, 1, 0, 1) == AURORA_EINVAL, 5);
  system.DeferEvents();
  check(system.Sleep(1) == AURORA_EBUSY, 6);
  system.ResumeEvents();
  system.io.PutString("TIMER\n");
  timed(100); /* No subscriptions needed. */
  check(system.io.Subscribe(key) == 0, 7);
  check(system.telnet.Subscribe(net) == 0, 8);
  system.io.PutString("KEY\n");
  aurora_word_t start = system.TimeMs();
  check(system.Sleep(1000) == 0, 9);
  check(keys == 1 && system.TimeMs() - start < 500u, 10);
  system.io.PutString("CANCEL\n");
  check(system.midi.Sequence(tune, 2, 0) == 0, 17);
  timed(1200); /* Old 1000ms deadline must not survive the earlier key wake. */
  check(system.midi.SequenceBusy() == 0, 18);
  system.io.PutString("NET\n");
  start = system.TimeMs();
  check(system.Sleep(0x7fffffffu) == 0, 11);
  check(nets == 1 && system.TimeMs() - start < 500u, 12);
  system.io.PutString("RACE\n");
  while (keys != 2u) system.Yield();
  start = system.TimeMs();
  check(system.Sleep(1000) == 0, 13); /* Callback already ran: consume wake credit. */
  check(system.TimeMs() - start < 500u, 14);
  system.io.PutString("WAIT\n");
  check(system.WaitEvent() == 0, 15); /* Must remain blocked until another event. */
  check(keys == 3u, 16);
  system.io.PutString("SLEEP CONTRACT PASS\n");
  system.Exit(0);
  for (;;) system.Yield();
}
