#include <aurora.h>
#include <aurora_user.h>

AURORA_USER_PROGRAM;

__attribute__((noinline)) static void print_character(aurora_word_t character) {
  (void)aurora_console_putchar(character);
}

__attribute__((noinline, optnone)) static void on_key(
    aurora_word_t character) {
  volatile aurora_word_t saved_character = character;
  print_character('[');
  print_character(saved_character);
  print_character(']');
  print_character('\n');
}

__attribute__((noreturn)) void _start(void) {
  /* After the initial IRET this initializes the active user stack (USP). */
  aurora.SetUpStackPointer();
  (void)aurora_keyboard_subscribe(on_key);

  (void)aurora_console_putchar('[');
  (void)aurora_console_putchar('r');
  (void)aurora_console_putchar('e');
  (void)aurora_console_putchar('a');
  (void)aurora_console_putchar('d');
  (void)aurora_console_putchar('y');
  (void)aurora_console_putchar(']');
  (void)aurora_console_putchar('\n');

  /* No polling and no repeated software INT: the timer supplies preemption,
   * and a keyboard hardware IRQ enters on_key asynchronously. */
  for (;;)
    aurora.Nop();
}
