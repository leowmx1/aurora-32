#ifndef TINYC_BOOTSTRAP_H
#define TINYC_BOOTSTRAP_H
/* LLVM seed only: OS bindings, no compiler or filesystem algorithm. */
#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
/* Aurora strings are physically word characters, while seed Clang retains a
 * nominal char* type. Tiny C represents both with int*; permit this bridge. */
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
/* Keep this small lexer switch out of an unsupported seed-backend ROTL fold. */
__attribute__((optnone)) int escape(void);
static volatile unsigned tc_read, tc_write;
static unsigned tc_keys[256] __attribute__((section(".noinit")));
static void tc_key(unsigned event) {
    unsigned next=(tc_write+1u)&255u;
    if(next==tc_read || aurora_syscall(16,0,0,0)) aurora_process_exit(2);
    tc_keys[tc_write]=aurora_event_character(event); tc_write=next;
}
static int getchar(void) {
    while(tc_read==tc_write) aurora_syscall(15,0,0,0);
    int c=(int)tc_keys[tc_read]; tc_read=(tc_read+1u)&255u; return c;
}
static int sys(int id,int a,int b,int c) { return aurora_syscall((unsigned)id,(unsigned)a,(unsigned)b,(unsigned)c); }
static int putchar(int c) { return sys(1,c,0,0); }
static int putint(int n) { return sys(5,n,0,0); }
static int putstr(const char *s) { return sys(6,(int)(unsigned long)s,0,0); }
static int exit(int code) { aurora_process_exit(code); return 0; }
int main(void);
__attribute__((noreturn)) void _start(void) {
    __aurora_set_up_stack_pointer();
    if(aurora_keyboard_subscribe(tc_key)!=AURORA_OK) aurora_process_exit(2);
    aurora_process_exit(main());
}
#endif
