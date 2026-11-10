#include <aurora.h>
AURORA_USER_PROGRAM;

#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)
#ifndef TEST_OPCODE
#error TEST_OPCODE is required
#endif
void _start(void) {
  __asm__ volatile(STRINGIFY(TEST_OPCODE) " r0" : : : "r0", "memory");
  for (;;) aurora.Nop();
}
