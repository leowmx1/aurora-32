#include <aurora.h>

AURORA_KERNEL_PROGRAM;

/* ROM reset always enters _start, regardless of function emission order. */
__asm__(
    ".globl __aurora_rom_entry\n"
    "__aurora_rom_entry:\n"
    "  jmp _start\n");

#define MAX_TOKEN 512
#define UMINUS 127
#define UPLUS 126

/* These objects are shared with the keyboard ISR and must stay volatile. */
volatile int token_count;
volatile int input_done;
volatile int has_digit;
volatile int current_value;
volatile int last_ans = 0;
volatile int has_ans = 0;
volatile int err_flag;
volatile int tokens[MAX_TOKEN*2];
volatile int post_expr[MAX_TOKEN*2];

aurora_interrupt("keyboard")
void handle_input(void);

/* Bare-metal compatibility ISR: acknowledge each timer tick and return
 * without changing the interrupted program's register state. */
__attribute__((naked, noinline))
void handle_timer(void) {
    __asm__ volatile(
        "push r0\n"
        "ldi r0, 0x07000000\n"
        "ild r0, r0\n"
        "pop r0\n"
        "iret\n");
}

__attribute__((noinline, optnone))
static int priority(int op) {
    if (op == '+' || op == '-') {
        return 1;
    } else if (op == '*' || op == '/') {
        return 2;
    } else if (op == UMINUS || op == UPLUS) {
        return 3;
    } else {
        return 0;
    }
}

/* Keep this compound decision out of _start's optimized SelectionDAG. */
__attribute__((noinline, optnone))
static int should_pop_operator(int stack_words, int top_op, int top_priority,
                               int current_priority) {
    if (stack_words <= 0) {
        return 0;
    }
    if (top_op == '(') {
        return 0;
    }
    if ((top_priority <= current_priority) && (top_op != UMINUS && top_op != UPLUS)) {
        return 0;
    }
    if ((top_priority < current_priority) && (top_op == UMINUS || top_op == UPLUS)) {
        return 0;
    }

    return 1;
}

__attribute__((noreturn))
void _start(void) {
    while (1) {
        aurora.SetUpStackPointer();
        aurora.DisableInterrupt();
        aurora.interrupt.Register(0x03000007u, handle_timer);

        token_count = -1;
        input_done = 0;
        has_digit = 0;
        current_value = 0;
        err_flag = 0;

        aurora.io.PutString("Enter expression, end with <Enter>:\n>> ");

        aurora.EnableInterrupt();

        /* Volatile is essential: the keyboard ISR changes input_done. */
        while (!input_done) {
        }

        /* IRET enables interrupts again, so stop keyboard writes while sorting. */
        aurora.DisableInterrupt();

        if (token_count < 0) {
            aurora.io.PutString("InputError: No input received.\n");
            err_flag = 1;
            continue;
        }

        int original_stack_pointer = aurora.stack.GetPointer();
        int post_expr_pointer = 0;

        for (int i = 0; i <= token_count && !err_flag; i++) {
            if (tokens[i*2] == 0x1) {
                post_expr[post_expr_pointer*2] = 0x1;
                post_expr[post_expr_pointer*2+1] = tokens[i*2+1];
                post_expr_pointer++;
            } else if (tokens[i*2] == 0x2) {
                if (tokens[i*2+1] == '+' || tokens[i*2+1] == '-' || tokens[i*2+1] == '*' || tokens[i*2+1] == '/' || tokens[i*2+1] == UPLUS || tokens[i*2+1] == UMINUS) {
                    int current_op = tokens[i*2+1];
                    int current_op_priority = priority(current_op);
                    if (aurora.stack.GetPointer() == original_stack_pointer) {
                        aurora.stack.Push(current_op);
                        continue;
                    }
                    int top_op = aurora.stack.Pop();
                    aurora.stack.Push(top_op);
                    int top_op_priority = priority(top_op);
                    while (should_pop_operator(aurora.stack.GetPointer() -
                                                original_stack_pointer,
                                            top_op, top_op_priority,
                                            current_op_priority)) {
                        post_expr[post_expr_pointer*2] = 0x2;
                        post_expr[post_expr_pointer*2+1] = aurora.stack.Pop();
                        post_expr_pointer++;
                        if (aurora.stack.GetPointer() == original_stack_pointer) {
                            break;
                        }
                        top_op = aurora.stack.Pop();
                        aurora.stack.Push(top_op);
                        top_op_priority = priority(top_op);
                    }
                    aurora.stack.Push(current_op);
                    
                } else if (tokens[i*2+1] == '(') {
                    aurora.stack.Push(tokens[i*2+1]);
                } else if (tokens[i*2+1] == ')') {
                    int top_op = aurora.stack.Pop();
                    while (top_op != '(' && aurora.stack.GetPointer() > original_stack_pointer) {
                        if (aurora.stack.GetPointer() == original_stack_pointer) {
                            aurora.io.PutString("SyntaxError: Unexpected ')'.\n");
                            err_flag = 1;
                            break;
                        }
                        post_expr[post_expr_pointer*2] = 0x2;
                        post_expr[post_expr_pointer*2+1] = top_op;
                        post_expr_pointer++;
                        top_op = aurora.stack.Pop();
                    }
                    if (top_op != '(') {
                        aurora.io.PutString("SyntaxError: Unexpected ')'.\n");
                        err_flag = 1;
                        break;
                    }
                }
            }
        }
        
        while (aurora.stack.GetPointer() > original_stack_pointer && !err_flag) {
            int top_op = aurora.stack.Pop();
            post_expr[post_expr_pointer*2] = 0x2;
            post_expr[post_expr_pointer*2+1] = top_op;
            post_expr_pointer++;
        }

        for (int i = 0; i < post_expr_pointer && !err_flag; i++) {
            if (post_expr[i*2] == 0x1) {
                aurora.stack.Push(post_expr[i*2+1]);
            } else if (post_expr[i*2] == 0x2) {
                int b = aurora.stack.Pop();
                int a = aurora.stack.Pop();
                int result;
                switch (post_expr[i*2+1]) {
                    case '+':
                        result = a + b;
                        break;
                    case '-':
                        result = a - b;
                        break;
                    case '*':
                        result = a * b;
                        break;
                    case '/':
                        if (b == 0) {
                            aurora.io.PutString("MathError: Division by zero.\n");
                            err_flag = 1;
                            break;
                        }
                        result = a / b;
                        break;
                    case UMINUS:
                        aurora.stack.Push(a);
                        result = -b;
                        break;
                    case '(':
                        aurora.io.PutString("SyntaxError: Expected ')'.\n");
                        err_flag = 1;
                        break;
                    default:
                        aurora.io.PutString("MathError: Unknown operator ('");
                        aurora.io.PutChar(post_expr[i*2+1]);
                        aurora.io.PutString("').\n");
                        err_flag = 1;
                        break;
                }
                aurora.stack.Push(result);
            }
        }
        if (!err_flag) {
            aurora.io.PutString("Result: ");
            last_ans = aurora.stack.Pop();
            aurora.io.PutInt(last_ans);
            aurora.io.PutChar('\n');

            has_ans = 1;
        }
        

        for (int i = 0; i < token_count; i++) {
            tokens[i] = 0x0;
            post_expr[i] = 0x0;
        }
    }

    aurora.Halt();
}

void handle_input(void) {
    if (input_done) {
        return;
    }

    char c = aurora.io.GetChar();

    if (c == 'Q') {
        aurora.io.PutString("Quit.");
        aurora.Halt();
    }

    if (c == ' ') {
        aurora.io.PutChar(c);
        return;
    }

    if (c == '~') {
        if (!has_ans) {
            aurora.io.PutString("\nWarning: There're no history answers.\n");
        }
        current_value = last_ans;
        has_digit = 1;
        aurora.io.PutChar(c);
        return;
    }

    if (c == '\n' || c == '\r') {
        if (has_digit) {
            token_count++;
            tokens[token_count*2] = 0x1;
            tokens[token_count*2+1] = current_value;
        }
        input_done = 1;
        aurora.io.PutString("\n");
        return;
    }

    if (c == '*' || c == '/') {
        if (token_count < 0) {
            token_count++;
            tokens[token_count*2] = 0x1;
            tokens[token_count*2+1] = current_value;
            token_count++;
            tokens[token_count*2] = 0x2;
            tokens[token_count*2+1] = c;

            current_value = 0;
            has_digit = 0;
            aurora.io.PutChar(c);
            return;
        }
        if (!(tokens[token_count*2] == 0x2 && tokens[token_count*2+1] == ')')) {
            token_count++;
            tokens[token_count*2] = 0x1;
            tokens[token_count*2+1] = current_value;
        }
        token_count++;
        tokens[token_count*2] = 0x2;
        tokens[token_count*2+1] = c;

        current_value = 0;
        has_digit = 0;
        aurora.io.PutChar(c);
        return;
    }

    if (c == '+' || c == '-') {
        if (has_digit) {
            token_count++;
            tokens[token_count*2] = 0x1;
            tokens[token_count*2+1] = current_value;
            token_count++;
            tokens[token_count*2] = 0x2;
            tokens[token_count*2+1] = c;

            current_value = 0;
            has_digit = 0;
            aurora.io.PutChar(c);
            return;
        } else {
            if (token_count < 0) {
                token_count++;
                tokens[token_count*2] = 0x2;
                if (c == '-') {tokens[token_count*2+1] = UMINUS;} else {tokens[token_count*2+1] = UPLUS;}
                aurora.io.PutChar(c);
                return;
            }
            if (tokens[token_count*2+1] == ')' && tokens[token_count*2] == 0x2) {
                token_count++;
                tokens[token_count*2] = 0x2;
                tokens[token_count*2+1] = c;

                current_value = 0;
                has_digit = 0;
                aurora.io.PutChar(c);
                return;
            }
            token_count++;
            tokens[token_count*2] = 0x2;
            if (c == '-') {tokens[token_count*2+1] = UMINUS;} else {tokens[token_count*2+1] = UPLUS;}
        }
        aurora.io.PutChar(c);
        return;
    }

    if (c == '(') {
        token_count++;
        tokens[token_count*2] = 0x2;
        tokens[token_count*2+1] = c;

        current_value = 0;
        has_digit = 0;
        aurora.io.PutChar(c);
        return;
    }

    if (c == ')') {
        if (!(tokens[token_count*2] == 0x2 && tokens[token_count*2+1] == ')')) {
            token_count++;
            tokens[token_count*2] = 0x1;
            tokens[token_count*2+1] = current_value;
        }
        token_count++;
        tokens[token_count*2] = 0x2;
        tokens[token_count*2+1] = c;

        current_value = 0;
        has_digit = 0;
        aurora.io.PutChar(c);
        return;
    }

    if (c >= '0' && c <= '9') {
        int digit = c - '0';
        current_value = current_value * 10 + digit;

        has_digit = 1;
        aurora.io.PutChar(c);
        return;
    }
}
