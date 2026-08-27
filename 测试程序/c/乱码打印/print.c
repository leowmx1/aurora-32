#include <aurora.h>

#define MAX_VALUES 1024

/* These objects are shared with the keyboard ISR and must stay volatile. */
volatile int str[MAX_VALUES];
char printable[] = {
    '8', 's', '!', 'G', 'z', ')', '3', 'X', 'q', ';', 'N', ' ', 'U', 'm', 'A',
    '/', '7', 'k', '?', 'Y', '`', '^', 'f', '4', '\\', 'd', 'o', 'R', ':', 'I',
    '#', 'c', 'J', '0', 'P', '>', 'b', 'L', '1', '|', 'v', 'H', 't', '5', 'Z',
    '.', 'W', 'S', '&', 'a', '<', '9', ']', '{', 'E', 'n', 'r', '2', '@', 'M',
    '$', '_', 'h', ',', 'O', 'i', 'Q', '~', 'F', 'g', '6', '}', '-', '"', '=',
    '(', 'B', '%', '*', 'w', 'K', 'l', 'y', 'D', '\'', 'C', '[', 'p', 'T', 'u',
    'e', 'j', '+'
};
volatile int count;
volatile int input_done;

aurora_interrupt("keyboard")
void handle_input(void);

__attribute__((noreturn))
void _start(void) {
    aurora.SetUpStackPointer();
    aurora.DisableInterrupt();

    count = 0;

    aurora.io.PutString("Please input target string:\n> ");

    aurora.EnableInterrupt();
    while (!input_done);
    aurora.io.PutChar('\n');

    for (int i = 0; i < count; i++) {
        int target_char = str[i];
        int temp[MAX_VALUES];
        for (int j = 0; j < 96; j++) {
            int current_char = printable[j];
            if (current_char == target_char) {
                for (int k = 0; k < i; k++) {
                    aurora.io.PutChar(temp[k]);
                }
                aurora.io.PutChar(target_char);
                temp[i] = target_char;
                aurora.io.PutChar('\n');
                break;
            } else {
                for (int k = 0; k < i; k++) {
                    aurora.io.PutChar(temp[k]);
                }
                aurora.io.PutChar(current_char);
                aurora.io.PutChar('\n');
            }
            for (int t = 0; t < 256; t++) {
                aurora.Nop();
            }
        }
    }
    
    aurora.Halt();
}

void handle_input(void) {
    if (input_done) {return;}

    int c = aurora.io.GetChar();
    if (c == '\n' || c == '\r') {
        input_done = 1;
        return;
    }
    str[count++] = c;
    aurora.io.PutChar(c);
    return;
}