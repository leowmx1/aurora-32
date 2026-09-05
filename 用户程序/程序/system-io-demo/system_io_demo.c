#include <aurora.h>
#include <aurora_system.h>

AURORA_USER_PROGRAM;

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();

  system.io.PutString("Hello from system.io: ");
  system.io.PutInt(32);
  system.io.PutChar('\n');

  for (;;) {
    aurora_status_t character = system.io.GetChar();
    if (character >= 0 && character <= 255)
      system.io.PutChar((char)character);
  }
}
