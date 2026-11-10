#include <aurora.h>
#ifdef TEST_KERNEL
AURORA_KERNEL_PROGRAM;
#else
AURORA_USER_PROGRAM;
#endif
#ifndef TEST_WORDS
#define TEST_WORDS 1
#endif
static volatile aurora_u32 data[TEST_WORDS] __attribute__((section(".noinit")));
void _start(void) {
  data[0] = 7;
  data[TEST_WORDS-1] = 8;
  for (;;) aurora.Nop();
}
