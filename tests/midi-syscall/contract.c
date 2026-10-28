#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
static volatile aurora_word_t keys;
static volatile aurora_word_t notes[3] = {0u, 60u, 127u};
static void check(int ok, int code) {
  if (!ok) {
    system.io.PutString("MIDI FAIL ");
    system.io.PutInt(code);
    system.Exit(code);
  }
}
static void key(aurora_word_t event) {
  check(aurora_event_character(event) == 'k', 30);
  check(system.midi.NoteOn(67u, 90u) == 0, 31);
  check(system.midi.NoteOff(67u) == 0, 32);
  ++keys;
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  check(system.midi.NoteOn(128u, 100u) == AURORA_EINVAL, 1);
  check(system.midi.NoteOn(60u, 128u) == AURORA_EINVAL, 2);
  check(system.midi.NoteOff(-1) == AURORA_EINVAL, 3);
  check(system.midi.Program(128u) == AURORA_EINVAL, 4);
  check(aurora_syscall(22, -1, 1, 0) == AURORA_EINVAL, 5);
  check(aurora_syscall(22, 1, -1, 0) == AURORA_EINVAL, 6);
  check(aurora_syscall(23, 128, 0, 0) == AURORA_EINVAL, 7);
  check(aurora_syscall(24, -1, 0, 0) == AURORA_EINVAL, 8);
  check(aurora_syscall(21, 0, 0, 0) == AURORA_ENOSYS, 9);
  check(system.midi.Program(0u) == 0, 10);
  check(system.midi.NoteOn(notes[0], 127u) == 0, 11);
  check(system.midi.NoteOff(notes[0]) == 0, 12);
  check(system.midi.NoteOn(notes[2], 1u) == 0, 13);
  check(system.midi.NoteOn(notes[2], 0u) == 0, 14);
  check(system.midi.Program(127u) == 0, 15);
  check(system.midi.NoteOn(notes[1], 100u) == 0, 16);
  check(system.midi.NoteOn(notes[1], 101u) == 0, 17);
  check(system.midi.NoteOn(64u, 80u) == 0, 18);
  check(system.midi.AllNotesOff() == 0, 19);
  check(system.midi.AllNotesOff() == 0, 20);
  check(system.io.Subscribe(key) == 0, 21);
  system.io.PutString("MIDI CALLBACK READY\n");
  while (keys == 0u) system.WaitEvent();
  check(system.midi.NoteOn(60u, 100u) == 0, 22);
  check(system.midi.NoteOn(64u, 80u) == 0, 23);
  system.io.PutString("MIDI EXIT READY\n");
  system.Exit(0);
  for (;;) system.Yield();
}
