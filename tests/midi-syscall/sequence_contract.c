#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
static volatile aurora_word_t done;
static aurora_word_t background[16] = {
  60,40,300,100, 62,40,300,100, 64,40,300,100, 65,40,300,100
};
static aurora_word_t effect[8] = {84,90,100,20, 88,90,100,20};
static aurora_word_t large[256];
static void check(int ok, int code) {
  if (!ok) { system.io.PutString("SEQUENCE FAIL "); system.io.PutInt(code); system.Exit(code); }
}
static void key(aurora_word_t event) {
  if (aurora_event_character(event) == 'e') {
    effect[4] = 129u;
    check(system.midi.Sequence(effect, 2, 2) == AURORA_EINVAL, 20);
    effect[4] = 88u;
    check(system.midi.Sequence(effect, 2, 2) == 0, 21);
    effect[0] = 10u; /* Kernel must have copied, not retained this pointer. */
  } else done = 1u;
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  check(system.midi.Sequence(background, 0, 0) == AURORA_EINVAL, 1);
  check(system.midi.Sequence(background, 65, 0) == AURORA_EINVAL, 2);
  check(system.midi.Sequence(background, 4, 3) == AURORA_EINVAL, 3);
  check(aurora_syscall(26, 0x82000000u, 1, 0) == AURORA_EFAULT, 4);
  check(aurora_syscall(26, 0xfffffffcu, 2, 0) == AURORA_EFAULT, 5);
  check(aurora_syscall(26, 0x81fffffeu, 1, 0) == AURORA_EFAULT, 6);
  for (int i = 0; i < 64; ++i) {
    large[i*4] = 128; large[i*4+1] = 0; large[i*4+2] = 1; large[i*4+3] = 0;
  }
  large[255] = 60001u;
  check(system.midi.Sequence(large, 64, 0) == AURORA_EINVAL, 7);
  large[255] = 0u;
  large[253] = 128u;
  check(system.midi.Sequence(large, 64, 0) == AURORA_EINVAL, 8);
  large[253] = 0u; large[254] = 0u;
  check(system.midi.Sequence(large, 64, 0) == AURORA_EINVAL, 9);
  large[254] = 1u;
  check(system.midi.Sequence(large, 64, 0) == 0, 10);
  check(system.midi.AllNotesOff() == 0, 11);
  check(system.midi.SequenceBusy() == 0, 12);
  check(system.io.Subscribe(key) == 0, 13);
  check(system.midi.Sequence(background, 4, 1) == 0, 14);
  background[0] = 33u;
  check(system.midi.NoteOn(50, 100) == AURORA_EBUSY, 15);
  check(system.midi.NoteOff(60) == AURORA_EBUSY, 16);
  check(system.graphics.FillRect(0, 0, 160, 100, 4) == 0, 17);
  system.io.PutString("SEQUENCE READY\n");
  /* No audio polling, Yield or timer reads: ticks must play the copied tracks
   * while user computation and the independent graphics device are busy. */
  while (!done) __asm__ volatile("nop");
  check(system.midi.AllNotesOff() == 0, 18);
  check(system.midi.SequenceBusy() == 0, 19);
  while (system.graphics.Busy() != 0) system.Yield();
  system.io.PutString("SEQUENCE PASS\n");
  system.Exit(0);
  for (;;) system.Yield();
}
