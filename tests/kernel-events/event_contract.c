#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
static volatile aurora_word_t count, active, values[128], kinds[128];
static void check(int ok, int code) {
  if (!ok) { system.io.PutString("CONTRACT FAIL "); system.io.PutInt(code); system.Exit(code); }
}
static void receive(aurora_word_t event, aurora_word_t kind) {
  check(!active, 10);
  active = 1u;
  check(system.WaitEvent() == AURORA_EBUSY, 11);
  check(system.Yield() == AURORA_OK, 12);
  check(system.Yield() == AURORA_OK, 13);
  values[count] = aurora_event_character(event);
  kinds[count] = kind;
  ++count;
  active = 0u;
}
static void keyboard(aurora_word_t event) { receive(event, 1u); }
static void network(aurora_word_t event) { receive(event, 2u); }
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  check(system.WaitEvent() == AURORA_EINVAL, 1);
  check(system.ResumeEvents() == AURORA_EINVAL, 2);
  system.DeferEvents(); system.DeferEvents();
  check(system.io.Subscribe(keyboard) == AURORA_OK, 3);
  check(system.telnet.Subscribe(network) == AURORA_OK, 4);
  check(system.io.GetChar() == AURORA_EBUSY, 5);
  check(system.telnet.GetChar() == AURORA_EBUSY, 6);
  system.io.PutString("P0 ready\n");
  for (int i = 0; i < 8; ++i) system.Yield();
  check(count == 0u && system.EventDrops() == 7u, 20);
  system.ResumeEvents();
  check(count == 0u, 21);
  check(system.WaitEvent() == AURORA_EBUSY, 22);
  system.ResumeEvents();
  while (count < 63u) system.WaitEvent();
  for (aurora_word_t i = 0; i < 63u; ++i) {
    check(values[i] == i + 1u, 23);
    check(kinds[i] == i % 2u + 1u, 24);
  }
  system.DeferEvents();
  system.io.PutString("P1 ready\n");
  for (int i = 0; i < 8; ++i) system.Yield();
  system.io.Subscribe((aurora_keyboard_callback_t)0);
  system.io.Subscribe(keyboard);
  system.ResumeEvents();
  while (count < 68u) system.WaitEvent();
  for (aurora_word_t i = 0; i < 5u; ++i) {
    check(values[63u + i] == 102u + i * 2u, 25);
    check(kinds[63u + i] == 2u, 26);
  }
  check(system.EventDrops() == 7u, 27);
  system.io.PutString("P2 sleeping\n");
  while (count < 69u) system.WaitEvent();
  check(values[68] == 'Z' && kinds[68] == 1u, 28);
  system.io.PutString("Event contract PASS\n");
  system.Exit(0);
  for (;;) aurora.Nop();
}
