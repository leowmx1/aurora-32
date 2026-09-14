#include <aurora.h>
#include <aurora_system.h>

AURORA_USER_PROGRAM;

#define QUEUE_SIZE 64u
#define LINE_SIZE 32u

/* Callback produces complete events; the foreground consumes them. */
static volatile aurora_word_t input_queue[QUEUE_SIZE]
    __attribute__((section(".noinit")));
static volatile aurora_word_t queue_read;
static volatile aurora_word_t queue_write;
static volatile aurora_word_t queue_overflow;
static volatile unsigned int idle_ticks;
static aurora_word_t last_sequence;
/* One character per hardware word, as in the calculator input buffer. */
static int line[LINE_SIZE];
static unsigned int line_length;
static int skip_lf;
static int phase; /* 0: lower bound, 1: upper bound, 2: guessing */
static int lower_bound;
static int upper_bound;
static int secret;
static int attempts;
static unsigned int random_state;

static void keyboard_callback(aurora_word_t event_word) {
  aurora_word_t write_index = queue_write;
  aurora_word_t next = (write_index + 1u) & (QUEUE_SIZE - 1u);
  if (next == queue_read) {
    queue_overflow = 1u;
    return;
  }
  input_queue[write_index] = event_word;
  queue_write = next;
}

/* Convert a 32-bit bit pattern without signed overflow, including INT_MIN. */
static int signed_word(unsigned int bits) {
  if (bits <= 2147483647u)
    return (int)bits;
  return -1 - (int)(~bits);
}

/* Optional sign and surrounding spaces; reject malformed/overflowed input. */
static int parse_integer(const int *text, int *result) {
  unsigned int index = 0u;
  unsigned int magnitude = 0u;
  int negative = 0;
  while (text[index] == ' ' || text[index] == '\t')
    ++index;
  if (text[index] == '-' || text[index] == '+') {
    negative = text[index] == '-';
    ++index;
  }
  if (text[index] < '0' || text[index] > '9')
    return 0;
  while (text[index] >= '0' && text[index] <= '9') {
    unsigned int digit = (unsigned int)(text[index] - '0');
    if (magnitude > 214748364u)
      return 0;
    if (magnitude == 214748364u) {
      if (digit > 7u + (unsigned int)negative)
        return 0;
    }
    magnitude = magnitude * 10u + digit;
    ++index;
  }
  while (text[index] == ' ' || text[index] == '\t')
    ++index;
  if (text[index] != 0)
    return 0;
  if (negative)
    *result = signed_word(0u - magnitude);
  else
    *result = (int)magnitude;
  return 1;
}

static unsigned int random_word(void) {
  random_state = random_state * 1664525u + 1013904223u;
  return random_state;
}

static int choose_secret(int low, int high) {
  /* Unsigned subtraction also handles ranges crossing zero. A zero span
   * represents all 2^32 values, so it must never be used as a divisor. */
  unsigned int span = (unsigned int)high - (unsigned int)low + 1u;
  unsigned int sample = random_word();
  if (span != 0u) {
    unsigned int threshold = (0u - span) % span;
    while (sample < threshold)
      sample = random_word();
    sample %= span;
  }
  return signed_word((unsigned int)low + sample);
}

static void prompt(void) {
  if (phase == 0)
    system.io.PutString("Lower bound > ");
  else if (phase == 1)
    system.io.PutString("Upper bound > ");
  else
    system.io.PutString("Guess > ");
}

__attribute__((noreturn)) static void quit(void) {
  system.io.PutString("Quit.\n");
  system.Exit(0);
  for (;;)
    aurora.Nop();
}

static void submit_line(void) {
  int value;
  line[line_length] = 0;
  if (line_length == 1u && (line[0] == 'q' || line[0] == 'Q'))
    quit();
  if (!parse_integer(line, &value)) {
    system.io.PutString("Invalid integer. Use -2147483648 to 2147483647.\n");
    prompt();
    return;
  }
  if (phase == 0) {
    lower_bound = value;
    phase = 1;
  } else if (phase == 1) {
    if (value < lower_bound) {
      system.io.PutString("Upper bound must be >= lower bound.\n");
      prompt();
      return;
    }
    upper_bound = value;
    secret = choose_secret(lower_bound, upper_bound);
    attempts = 0;
    phase = 2;
    system.io.PutString("Range: [");
    system.io.PutInt(lower_bound);
    system.io.PutString(", ");
    system.io.PutInt(upper_bound);
    system.io.PutString("]\n");
  } else {
    if (value < lower_bound || value > upper_bound) {
      system.io.PutString("Outside the chosen range.\n");
      prompt();
      return;
    }
    if (attempts < 2147483647)
      ++attempts;
    if (value < secret)
      system.io.PutString("Too low.\n");
    else if (value > secret)
      system.io.PutString("Too high.\n");
    else {
      system.io.PutString("Correct! The number was ");
      system.io.PutInt(secret);
      system.io.PutString(". Attempts: ");
      system.io.PutInt(attempts);
      system.io.PutString("\n");
      system.Exit(0);
      for (;;)
        aurora.Nop();
    }
  }
  prompt();
}

static void process_character(unsigned int c) {
  if (c == '\n' && skip_lf) {
    skip_lf = 0;
    return;
  }
  skip_lf = 0;
  if (c == '\r' || c == '\n') {
    skip_lf = c == '\r';
    system.io.PutChar('\n');
    submit_line();
    line_length = 0u;
    return;
  }
  if (c == 8u || c == 127u) {
    if (line_length != 0u) {
      --line_length;
      system.io.PutString("\b \b");
    }
    return;
  }
  if (c < 32u || c > 126u)
    return;
  if (line_length == LINE_SIZE - 1u) {
    system.io.PutString("\nInput too long; line cleared.\n");
    line_length = 0u;
    prompt();
    return;
  }
  line[line_length++] = (int)c;
  system.io.PutChar((char)c);
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  queue_read = 0u;
  queue_write = 0u;
  queue_overflow = 0u;
  idle_ticks = 0u;
  last_sequence = 0u;
  line_length = 0u;
  skip_lf = 0;
  phase = 0;
  random_state = 0x6d2b79f5u;
  if (aurora_keyboard_subscribe(keyboard_callback) != AURORA_OK) {
    system.io.PutString("Keyboard subscription failed.\n");
    system.Exit(1);
  }
  system.io.PutString("Guess the number!\nSet an inclusive integer range.\n"
                      "Enter submits; Backspace edits; Q + Enter quits.\n");
  prompt();
  for (;;) {
    if (queue_overflow) {
      queue_overflow = 0u;
      queue_read = queue_write;
      line_length = 0u;
      skip_lf = 0;
      system.io.PutString("\nInput queue full; please re-enter this line.\n");
      prompt();
    }
    aurora_word_t read_index = queue_read;
    if (read_index == queue_write) {
      ++idle_ticks;
      aurora.Nop();
      continue;
    }
    aurora_word_t event = input_queue[read_index];
    queue_read = (read_index + 1u) & (QUEUE_SIZE - 1u);
    if ((event & 0xff000000u) != 0xa5000000u)
      continue;
    aurora_word_t sequence = event & 0x00ffff00u;
    if (sequence == 0u || sequence == last_sequence)
      continue;
    last_sequence = sequence;
    random_state = random_state * 1664525u + idle_ticks + event;
    process_character(event & 0xffu);
  }
}
