#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
static aurora_word_t buffer[4096] __attribute__((section(".noinit")));
static void check(int ok,int code) {
  if(!ok) { system.io.PutString("CONSOLE FAIL "); system.io.PutInt(code); system.Exit(1); }
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  check(system.io.PutString((const char *)0)==AURORA_EFAULT,1);
  check(system.io.PutString((const char *)0x82000000u)==AURORA_EFAULT,2);
  check(system.io.Write(buffer,65u)==AURORA_EINVAL,3);
  check(system.io.Write((const aurora_word_t *)0x813fffffu,2u)==AURORA_EFAULT,4);
  check(system.io.Write((const aurora_word_t *)0xffffffffu,0u)==0,5);
  volatile aurora_word_t *last=(volatile aurora_word_t *)0x813fffffu;
  aurora_word_t saved=*last;
  *last=0u; check(system.io.PutString((const char *)last)==0,6);
  *last='X'; check(system.io.PutString((const char *)last)==AURORA_EFAULT,7);
  *last=saved;
  for(aurora_word_t i=0;i<4096u;++i) buffer[i]='a';
  check(system.io.PutString((const char *)buffer)==AURORA_EINVAL,8);
  buffer[4095]=0u; check(system.io.PutString((const char *)buffer)==0,9);
  system.io.PutString("\nCONSOLE CONTRACT PASS\n"); system.Exit(0);
  for(;;) system.Yield();
}
