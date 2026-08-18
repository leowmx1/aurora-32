#include <aurora.h>

/* 保留 RAM 最后的三个字作为程序状态。 */
#define INPUT_COUNT aurora.ram[0xFFFFFD]
#define INPUT_DONE  aurora.ram[0xFFFFFE]
#define HAS_DIGIT   aurora.ram[0xFFFFFF]

aurora_interrupt("keyboard")
void handle_input(void);

__attribute__((noreturn))
void _start(void) {
    aurora.SetUpStackPointer();
    aurora.DisableInterrupt();

    INPUT_COUNT = 0;
    INPUT_DONE = 0;
    HAS_DIGIT = 0;
    aurora.ram[0] = 0;

    aurora.EnableInterrupt();

    while (!INPUT_DONE) {
    }

    /* IRET 会重新开中断，因此在主程序中关闭。 */
    aurora.DisableInterrupt();

    int count = INPUT_COUNT;
    int sorted = 0;

    while (!sorted) {
        int swapped = 0;

        for (int i = 0; i + 1 < count; i++) {
            if (aurora.ram[i] > aurora.ram[i + 1]) {
                int temp = aurora.ram[i];
                aurora.ram[i] = aurora.ram[i + 1];
                aurora.ram[i + 1] = temp;
                swapped = 1;
            }
        }

        if (!swapped) {
            sorted = 1;
        }
    }

    aurora.io.PutChar('\n');

    for (int i = 0; i < count; i++) {
        int value = aurora.ram[i];
        int old_sp_value = aurora.GetStackPointer();
        while (value > 0) {
            char c = value % 10 + '0';
            aurora.stack.Push(c);
            value /= 10;
        }
        while (aurora.GetStackPointer() > old_sp_value) {
            aurora.io.PutChar(aurora.stack.Pop());
        }
        if (aurora.ram[i] == 0) {
            aurora.io.PutChar('0');
        }
        if (i + 1 < count) {
            if (aurora.ram[i] == aurora.ram[i + 1]) {
                aurora.io.PutChar('=');
            } else {
                aurora.io.PutChar('<');
            }
        }
    }

    aurora.Halt();
}

void handle_input(void) {
    char c = aurora.io.GetChar();

    if (INPUT_DONE) {
        return;
    }

    if (c == '\n' || c == '\r') {
        /* 避免 CRLF 被计算成两个空数字。 */
        if (HAS_DIGIT) {
            INPUT_COUNT++;
            HAS_DIGIT = 0;
            aurora.ram[INPUT_COUNT] = 0;
        }
        aurora.io.PutChar(c);
        return;
    }

    if (c == ']') {
        /* 最后一个数字后可以不输入回车。 */
        if (HAS_DIGIT) {
            INPUT_COUNT++;
            HAS_DIGIT = 0;
        }

        INPUT_DONE = 1;
        return;
    }

    if (c >= '0' && c <= '9') {
        int index = INPUT_COUNT;

        aurora.ram[index] =
            aurora.ram[index] * 10 + (c - '0');

        aurora.io.PutChar(c);
        HAS_DIGIT = 1;
    }
}