#include <aurora.h>

#define MAX_VALUES 1024

/* These objects are shared with the keyboard ISR and must stay volatile. */
volatile int input_count;
volatile int input_done;
volatile int has_digit;
volatile int input_negative;
volatile int current_value_wide;
volatile int values[MAX_VALUES];

aurora_interrupt("keyboard")
void handle_input(void);

static void finish_number(void) {
    if (!has_digit) {
        input_negative = 0;
        return;
    }

    for (int i = 0; i < current_value_wide; i++) {
        aurora.io.PutChar('\b');
    }
    aurora.io.PutString("Number ");
    aurora.io.PutInt(input_count + 1);
    aurora.io.PutString(": ");
    aurora.io.PutInt(values[input_count]);
    aurora.io.PutString("\n");

    if (input_count < MAX_VALUES) {
        input_count++;
    }

    has_digit = 0;
    input_negative = 0;
    current_value_wide = 0;

    if (input_count < MAX_VALUES) {
        values[input_count] = 0;
    }
}

__attribute__((noreturn))
void _start(void) {
    aurora.SetUpStackPointer();
    aurora.DisableInterrupt();

    input_count = 0;
    input_done = 0;
    has_digit = 0;
    input_negative = 0;
    current_value_wide = 0;
    values[0] = 0;

    aurora.io.PutString("Enter integers to sort, one per line. End with ']'.\n");

    aurora.EnableInterrupt();

    /* Volatile is essential: the keyboard ISR changes input_done. */
    while (!input_done) {
    }

    /* IRET enables interrupts again, so stop keyboard writes while sorting. */
    aurora.DisableInterrupt();

    aurora.io.PutString("Sorting...\n");

    int count = input_count;
    for (int end = count - 1; end > 0; end--) {
        int swapped = 0;

        for (int i = 0; i < end; i++) {
            int left = values[i];
            int right = values[i + 1];

            if (left > right) {
                values[i] = right;
                values[i + 1] = left;
                swapped = 1;
            }
        }

        if (!swapped) {
            break;
        }
    }

    aurora.io.PutString("Sorted values:\n");
    for (int i = 0; i < count; i++) {
        aurora.io.PutInt(values[i]);

        if (i + 1 < count) {
            if (values[i] == values[i + 1]) {
                aurora.io.PutChar('=');
            } else {
                aurora.io.PutChar('<');
            }
        }
    }
    aurora.io.PutString("\n");
    aurora.Halt();
}

void handle_input(void) {
    char c = aurora.io.GetChar();

    if (input_done) {
        return;
    }

    if (c == ']') {
        finish_number();
        input_done = 1;
        return;
    }

    if (c == '\n' || c == '\r') {
        /* An empty LF after CR, or any other empty line, adds no value. */
        finish_number();
        return;
    }

    if (c == '-' && !has_digit && !input_negative) {
        input_negative = 1;
        current_value_wide++;
        aurora.io.PutChar(c);
        return;
    }

    if (c >= '0' && c <= '9' && input_count < MAX_VALUES) {
        int digit = c - '0';
        int value = values[input_count];

        if (input_negative) {
            values[input_count] = value * 10 - digit;
        } else {
            values[input_count] = value * 10 + digit;
        }

        has_digit = 1;
        current_value_wide++;
        aurora.io.PutChar(c);
    }
}
