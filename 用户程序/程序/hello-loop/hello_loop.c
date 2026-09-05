#include <aurora.h>
#include <aurora_user.h>

AURORA_USER_PROGRAM;

__attribute__((noinline)) static void print_character(aurora_word_t character) {
  (void)aurora_console_putchar(character);
}

__attribute__((noreturn, optnone)) void _start(void) {
  aurora.SetUpStackPointer();
  volatile aurora_word_t character;

  for (;;) {
    character = 'H'; print_character(character);
    character = 'e'; print_character(character);
    character = 'l'; print_character(character);
    character = 'l'; print_character(character);
    character = 'o'; print_character(character);
    character = ','; print_character(character);
    character = ' '; print_character(character);
    character = 'W'; print_character(character);
    character = 'o'; print_character(character);
    character = 'r'; print_character(character);
    character = 'l'; print_character(character);
    character = 'd'; print_character(character);
    character = '!'; print_character(character);
    character = '\f'; print_character(character);
  }
}
