#include <aurora.h>
#include <aurora_system.h>

AURORA_USER_PROGRAM;

#define MAX_TOKEN 512
#define UMINUS 127
#define UPLUS 126
#define INPUT_COMPLETE 0x13579BDF

/* Preserve the original stack algorithm without mixing its data with the C
 * call stack on USP.  Every entry is one Aurora 32-bit word. */
#define STACK_POINTER() calculation_stack_pointer
#define STACK_PUSH(value)                                                     \
  (calculation_stack[calculation_stack_pointer++] = (int)(value))
#define STACK_POP() (calculation_stack[--calculation_stack_pointer])

/* These objects are shared with the asynchronous user callback. */
volatile int token_count;
volatile int input_done;
volatile int has_digit;
volatile int current_value;
volatile int last_ans = 0;
volatile int has_ans = 0;
volatile int err_flag;
/* Suppress every trailing CR/LF IRQ after one accepted Enter.  The first
 * ordinary character starts the next input transaction. */
volatile int discard_line_endings;
volatile int tokens[MAX_TOKEN * 2] __attribute__((section(".noinit")));
volatile int post_expr[MAX_TOKEN * 2] __attribute__((section(".noinit")));
volatile int calculation_stack[MAX_TOKEN * 2]
    __attribute__((section(".noinit")));
volatile int calculation_stack_pointer;
volatile aurora_word_t keyboard_event_radix = 256u;

__attribute__((noinline, optnone)) static void
keyboard_callback(aurora_word_t input_character);
__attribute__((noinline, optnone)) static void
process_input(aurora_word_t input_character);

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
  /* Binary +, -, *, / are left-associative: an operator with equal
   * precedence must leave the stack before the new operator is pushed. */
  if (top_priority < current_priority) {
    return 0;
  }
  /* Consecutive unary signs are right-associative. */
  if ((top_priority == current_priority) &&
      (top_op == UMINUS || top_op == UPLUS)) {
    return 0;
  }
  return 1;
}

static void reset_input(void) {
  token_count = -1; input_done = has_digit = current_value = err_flag = 0;
  calculation_stack_pointer = 0;
  system.io.PutString("Enter expression, end with <Enter>:\n>> ");
}

/* Original infix-to-postfix and software-stack evaluator, now serialized
 * with parsing inside the non-nested kernel-delivered callback. */
static void evaluate_expression(void) {
  do {
    if (token_count < 0) {
      system.io.PutString("InputError: No input received.\n");
      err_flag = 1;
      continue;
    }

#ifdef AURORA_CALCULATOR_TRACE
    system.io.PutString("{P:tc=");
    system.io.PutInt(token_count);
    system.io.PutString(",t0=");
    system.io.PutInt(tokens[0]);
    system.io.PutString(",v0=");
    system.io.PutInt(tokens[1]);
    system.io.PutString(",sp=");
    system.io.PutInt(calculation_stack_pointer);
    system.io.PutString("}");
#endif

    int original_stack_pointer = STACK_POINTER();
    int post_expr_pointer = 0;

    for (int i = 0; i <= token_count && !err_flag; i++) {
      if (tokens[i * 2] == 0x1) {
        post_expr[post_expr_pointer * 2] = 0x1;
        post_expr[post_expr_pointer * 2 + 1] = tokens[i * 2 + 1];
        post_expr_pointer++;
      } else if (tokens[i * 2] == 0x2) {
        if (tokens[i * 2 + 1] == '+' || tokens[i * 2 + 1] == '-' ||
            tokens[i * 2 + 1] == '*' || tokens[i * 2 + 1] == '/' ||
            tokens[i * 2 + 1] == UPLUS || tokens[i * 2 + 1] == UMINUS) {
          int current_op = tokens[i * 2 + 1];
          int current_op_priority = priority(current_op);
          if (STACK_POINTER() == original_stack_pointer) {
            STACK_PUSH(current_op);
            continue;
          }
          int top_op = STACK_POP();
          STACK_PUSH(top_op);
          int top_op_priority = priority(top_op);
          while (should_pop_operator(STACK_POINTER() - original_stack_pointer,
                                     top_op, top_op_priority,
                                     current_op_priority)) {
            post_expr[post_expr_pointer * 2] = 0x2;
            post_expr[post_expr_pointer * 2 + 1] = STACK_POP();
            post_expr_pointer++;
            if (STACK_POINTER() == original_stack_pointer) {
              break;
            }
            top_op = STACK_POP();
            STACK_PUSH(top_op);
            top_op_priority = priority(top_op);
          }
          STACK_PUSH(current_op);
        } else if (tokens[i * 2 + 1] == '(') {
          STACK_PUSH(tokens[i * 2 + 1]);
        } else if (tokens[i * 2 + 1] == ')') {
          int top_op = STACK_POP();
          while (top_op != '(' &&
                 STACK_POINTER() > original_stack_pointer) {
            if (STACK_POINTER() == original_stack_pointer) {
              system.io.PutString("SyntaxError: Unexpected ')'.\n");
              err_flag = 1;
              break;
            }
            post_expr[post_expr_pointer * 2] = 0x2;
            post_expr[post_expr_pointer * 2 + 1] = top_op;
            post_expr_pointer++;
            top_op = STACK_POP();
          }
          if (top_op != '(') {
            system.io.PutString("SyntaxError: Unexpected ')'.\n");
            err_flag = 1;
            break;
          }
        }
      }
    }

    while (STACK_POINTER() > original_stack_pointer && !err_flag) {
      int top_op = STACK_POP();
      post_expr[post_expr_pointer * 2] = 0x2;
      post_expr[post_expr_pointer * 2 + 1] = top_op;
      post_expr_pointer++;
    }

#ifdef AURORA_CALCULATOR_TRACE
    system.io.PutString("{R:pp=");
    system.io.PutInt(post_expr_pointer);
    system.io.PutString(",p0=");
    system.io.PutInt(post_expr[0]);
    system.io.PutString(",v0=");
    system.io.PutInt(post_expr[1]);
    system.io.PutString(",sp=");
    system.io.PutInt(calculation_stack_pointer);
    system.io.PutString("}");
#endif

    for (int i = 0; i < post_expr_pointer && !err_flag; i++) {
      if (post_expr[i * 2] == 0x1) {
        STACK_PUSH(post_expr[i * 2 + 1]);
      } else if (post_expr[i * 2] == 0x2) {
        int operation = post_expr[i * 2 + 1];
        /* Unary operators consume one operand.  The bare-metal version could
         * temporarily pop below its saved hardware-SP marker and push that
         * word back.  Doing the same with an array index is out-of-bounds and
         * corrupts adjacent globals, so express the identical unary operation
         * directly on the software stack. */
        if (operation == UMINUS || operation == UPLUS) {
          if (STACK_POINTER() <= original_stack_pointer) {
            system.io.PutString("SyntaxError: Missing operand.\n");
            err_flag = 1;
            break;
          }
          int operand = STACK_POP();
          STACK_PUSH(operation == UMINUS ? -operand : operand);
          continue;
        }
        if (STACK_POINTER() - original_stack_pointer < 2) {
          system.io.PutString("SyntaxError: Missing operand.\n");
          err_flag = 1;
          break;
        }
        int b = STACK_POP();
        int a = STACK_POP();
        int result;
        switch (operation) {
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
            system.io.PutString("MathError: Division by zero.\n");
            err_flag = 1;
            break;
          }
          result = a / b;
          break;
        case '(':
          system.io.PutString("SyntaxError: Expected ')'.\n");
          err_flag = 1;
          break;
        default:
          system.io.PutString("MathError: Unknown operator ('");
          system.io.PutChar((char)operation);
          system.io.PutString("').\n");
          err_flag = 1;
          break;
        }
        STACK_PUSH(result);
      }
    }

#ifdef AURORA_CALCULATOR_TRACE
    system.io.PutString("{E:sp=");
    system.io.PutInt(calculation_stack_pointer);
    system.io.PutString(",base=");
    system.io.PutInt(original_stack_pointer);
    system.io.PutString(",err=");
    system.io.PutInt(err_flag);
    system.io.PutString("}");
#endif

    if (!err_flag &&
        STACK_POINTER() - original_stack_pointer != 1) {
      system.io.PutString("SyntaxError: Invalid expression.\n");
      err_flag = 1;
    }

    if (!err_flag) {
      system.io.PutString("Result: ");
      last_ans = STACK_POP();
      system.io.PutInt(last_ans);
      system.io.PutChar('\n');
      has_ans = 1;
    }

    for (int i = 0; i < token_count; i++) {
      tokens[i] = 0x0;
      post_expr[i] = 0x0;
    }
  } while (0);
}

__attribute__((noinline, optnone)) static void
keyboard_callback(aurora_word_t event) {
  if (system.EventDrops() != 0u) {
    system.io.PutString("\nInput event queue overflow. Exiting safely.\n");
    system.Exit(1);
  }
#ifdef AURORA_CALCULATOR_TRACE
  system.io.PutChar('{');
  system.io.PutInt((int)((event & 0x00ffff00u) / keyboard_event_radix));
  system.io.PutChar('}');
#endif
  process_input(aurora_event_character(event));
  if (input_done == INPUT_COMPLETE) {
    evaluate_expression();
    reset_input();
  }
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  system.DeferEvents();
  if (system.io.Subscribe(keyboard_callback) != AURORA_OK) system.Exit(1);
  reset_input();
  system.ResumeEvents();
  for (;;) system.WaitEvent();
}

__attribute__((noinline, optnone)) static void
process_input(aurora_word_t input_character) {
  aurora_word_t c = input_character & 0xffu;

  /* Keyboards may report Enter as CR, LF, CR/LF, or repeated IRQs.  Once a
   * line has ended, discard all newline codes until an ordinary character
   * proves that the next expression has started. */
  if (discard_line_endings) {
    if (c == '\n' || c == '\r')
      return;
    discard_line_endings = 0;
  }

  if (input_done == INPUT_COMPLETE)
    return;

  if (c == 'Q') {
    system.io.PutString("\nQuit.");
    system.Exit(0);
  }

  if (c == ' ') {
    system.io.PutChar(c);
    return;
  }

  if (c == '~') {
    if (!has_ans)
      system.io.PutString("\nWarning: There're no history answers.\n");
    current_value = last_ans;
    has_digit = 1;
    system.io.PutChar(c);
    return;
  }

  if (c == '\n' || c == '\r') {
    discard_line_endings = 1;
    if (has_digit) {
      token_count++;
      tokens[token_count * 2] = 0x1;
      tokens[token_count * 2 + 1] = current_value;
    }
    input_done = INPUT_COMPLETE;
    system.io.PutString("\n");
    return;
  }

  if (c == '*' || c == '/') {
    if (token_count < 0) {
      token_count++;
      tokens[token_count * 2] = 0x1;
      tokens[token_count * 2 + 1] = current_value;
      token_count++;
      tokens[token_count * 2] = 0x2;
      tokens[token_count * 2 + 1] = c;

      current_value = 0;
      has_digit = 0;
      system.io.PutChar(c);
      return;
    }
    if (!(tokens[token_count * 2] == 0x2 &&
          tokens[token_count * 2 + 1] == ')')) {
      token_count++;
      tokens[token_count * 2] = 0x1;
      tokens[token_count * 2 + 1] = current_value;
    }
    token_count++;
    tokens[token_count * 2] = 0x2;
    tokens[token_count * 2 + 1] = c;

    current_value = 0;
    has_digit = 0;
    system.io.PutChar(c);
    return;
  }

  if (c == '+' || c == '-') {
    if (has_digit) {
      token_count++;
      tokens[token_count * 2] = 0x1;
      tokens[token_count * 2 + 1] = current_value;
      token_count++;
      tokens[token_count * 2] = 0x2;
      tokens[token_count * 2 + 1] = c;

      current_value = 0;
      has_digit = 0;
      system.io.PutChar(c);
      return;
    } else {
      if (token_count < 0) {
        token_count++;
        tokens[token_count * 2] = 0x2;
        if (c == '-')
          tokens[token_count * 2 + 1] = UMINUS;
        else
          tokens[token_count * 2 + 1] = UPLUS;
        system.io.PutChar(c);
        return;
      }
      if (tokens[token_count * 2 + 1] == ')' &&
          tokens[token_count * 2] == 0x2) {
        token_count++;
        tokens[token_count * 2] = 0x2;
        tokens[token_count * 2 + 1] = c;

        current_value = 0;
        has_digit = 0;
        system.io.PutChar(c);
        return;
      }
      token_count++;
      tokens[token_count * 2] = 0x2;
      if (c == '-')
        tokens[token_count * 2 + 1] = UMINUS;
      else
        tokens[token_count * 2 + 1] = UPLUS;
    }
    system.io.PutChar(c);
    return;
  }

  if (c == '(') {
    token_count++;
    tokens[token_count * 2] = 0x2;
    tokens[token_count * 2 + 1] = c;

    current_value = 0;
    has_digit = 0;
    system.io.PutChar(c);
    return;
  }

  if (c == ')') {
    if (!(tokens[token_count * 2] == 0x2 &&
          tokens[token_count * 2 + 1] == ')')) {
      token_count++;
      tokens[token_count * 2] = 0x1;
      tokens[token_count * 2 + 1] = current_value;
    }
    token_count++;
    tokens[token_count * 2] = 0x2;
    tokens[token_count * 2 + 1] = c;

    current_value = 0;
    has_digit = 0;
    system.io.PutChar(c);
    return;
  }

  if (c >= '0' && c <= '9') {
    int digit = c - '0';
    current_value = current_value * 10 + digit;
    has_digit = 1;
    system.io.PutChar(c);
    return;
  }
}
