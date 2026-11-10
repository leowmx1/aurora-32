#include <aurora.h>
#include "aurora_system.h"
AURORA_USER_PROGRAM;
static volatile int action, callback_status, seen, checks;
static volatile unsigned int canary;
static aurora_io_buffer out;
static const aurora_word_t text[4] = {'~', '\f', '\b', '\n'};
static void key(aurora_word_t event) {
  ++seen;
  callback_status=system.Background(); /* Must not detach a live callback. */
  action=(int)(event&255u);
}
static void check(int ok) {
  ++checks;
  if(!ok) {
    system.io.PutString("BACKGROUND FAIL check="); system.io.PutInt(checks);
    system.io.PutString(" callback="); system.io.PutInt(callback_status);
    system.io.PutString(" seen="); system.io.PutInt(seen); system.io.PutChar('\n');
    system.Exit(1);
  }
}
static void check_output_sink(void) {
  check(system.io.PutChar('~')==0);
  check(system.io.PutInt(-987654321)==0);
  check(system.io.PutString("BACKGROUND STDOUT MUST BE SILENT\n")==0);
  check(system.io.Write(text,4u)==4);
  check(system.io.Write(text,0u)==0);
  check(system.io.Write(text,65u)==AURORA_EINVAL);
  check(system.io.Write((const aurora_word_t *)0,1u)==AURORA_EFAULT);
  check(system.io.PutString((const char *)0)==AURORA_EFAULT);
  system.io.buffer.Begin(&out);
  for(int i=0;i<100;++i) system.io.buffer.Char(&out,'~');
  system.io.buffer.Flush(&out);
  check(out.error==0 && out.used==0 && out.batch==2);
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  volatile unsigned int local=0x13579bdfu;
  canary=0x2468ace0u;
  int subscribed=system.io.Subscribe(key);
  if(subscribed==AURORA_EBUSY) {
    check(system.Background()==0);
    check_output_sink();
    system.io.PutString("BG READY\n");
    for(;;) system.Sleep(1234);
  }
  check(subscribed==0);
  check(aurora_syscall(AURORA_SYSCALL_BACKGROUND,1,0,0)==AURORA_EINVAL);
  system.DeferEvents(); check(system.Background()==AURORA_EBUSY); system.ResumeEvents();
  system.io.PutString("FG READY\n");
  for(;;) {
    system.DeferEvents();
    int a=action; action=0;
    system.ResumeEvents();
    if(a=='b') {
      check(callback_status==AURORA_EBUSY && seen>0);
      int result=system.Background();
      if(result==AURORA_EBUSY) { system.io.PutString("BACKGROUND BUSY\n"); continue; }
      check(result==0 && local==0x13579bdfu && canary==0x2468ace0u);
      check(system.Background()==0); /* Idempotent; no extra slot. */
      check(system.io.GetChar()==AURORA_EBUSY && system.io.Subscribe(key)==AURORA_EBUSY);
      check(system.WaitEvent()==AURORA_EINVAL);
      system.Sleep(10);
      check(local==0x13579bdfu && canary==0x2468ace0u);
      check_output_sink();
      system.io.PutString("DETACHED context OK\n");
      for(;;) system.Sleep(2345);
    }
    system.WaitEvent();
  }
}
