#include <aurora.h>
int a[200];
int input_end = 0;
void input(void);
void sort(void);
void output(void);
aurora_interrupt("keyboard")
void handle_input(void);

__attribute__((noreturn))
void _start(void) {
    aurora.SetUpStackPointer();
    aurora.EnableInterrupt();
    input();
    aurora.DisableInterrupt();
    sort();
    output();
    aurora.Halt();
}

void input(void) {
    while (!input_end){
        aurora.EnableInterrupt();
    }
    aurora.DisableInterrupt();
    return;
}
void sort(void) {
    int sorted = 0;
    while (!sorted) {
        sorted = 1;
        for (int i = 0; i < 199; i++) {
            if (a[i] > a[i + 1]) {
                int temp = a[i];
                a[i] = a[i + 1];
                a[i + 1] = temp;
                sorted = 0;
            }
        }
    }
    return;
}
void output(void) {
    for (int i = 0; i < 200; i++) {
        aurora.io.PutChar(a[i]);
    }
    return;
}
void handle_input(void) {
    static int i = 0;
    a[i] = aurora.io.GetChar();
    i++;
    if (i == 200) {
        input_end = 1;
    }
    return;
}