#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;

/* A quiet, finite task suitable for either terminal's background slot. */
static volatile aurora_word_t quit, progress, guard;
static void key(aurora_word_t event) {
  aurora_word_t c=aurora_event_character(event);
  if(c=='q' || c=='Q') quit=1u;
}
__attribute__((noinline,optnone)) static void wait_and_check(aurora_word_t expected) {
  volatile aurora_word_t saved=expected;
  system.Sleep(1000u);
  if(saved!=expected || guard!=expected) {
    system.io.PutString("Process demo: context check failed.\n");
    system.Exit(1);
  }
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  guard=system.TimeMs()+12345u;
  aurora_status_t subscription=system.io.Subscribe(key);
  if(subscription==AURORA_EBUSY)
    system.io.PutString("Process demo: background task, 60 seconds.\n");
  else if(subscription==0)
    system.io.PutString("Process demo: 60 seconds; Q quits early.\n");
  else system.Exit(1);
  aurora_word_t start=system.TimeMs(), expected=guard;
  while(!quit && system.TimeMs()-start<60000u) {
    wait_and_check(expected);
    ++progress;
  }
  system.Exit(0);
  for(;;) system.Yield();
}
