#include <aurora.h>
#include <aurora_system.h>
#include <aurora_kbqueue.h>

AURORA_USER_PROGRAM;
#define LINE_CAPACITY 96u
static int line[LINE_CAPACITY];
static unsigned int line_length, arg_start;
/* Keep flags word-sized; avoid unsupported optimizer-created i8 globals. */
static volatile int skip_lf, discard_line;
static aurora_io_buffer text;
static volatile aurora_word_t net_queue[64] __attribute__((section(".noinit")));
static volatile aurora_word_t net_read, net_write, net_overflow;
static aurora_word_t net_sequence, net_tx[64];
#define NET_LINE_CAPACITY 256u
static aurora_word_t net_line[NET_LINE_CAPACITY], net_line_length;
static volatile int net_skip_lf, net_discard_line;

static void telnet_callback(aurora_word_t event) {
  aurora_word_t next = (net_write + 1u) & 63u;
  if (next == net_read) { net_overflow = 1u; return; }
  net_queue[net_write] = event;
  net_write = next;
}

static void prompt(void) { system.io.PutString("aurora> "); }
static int space(int c) { return c == ' ' || c == '\t'; }

static int command_is(unsigned int start, unsigned int end, const char *name) {
  for (unsigned int i = start; i < end; ++i) {
    int c = line[i];
    if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    if (!*name || c != *name++) return 0;
  }
  return *name == 0;
}

static void flush_text(void) {
  system.io.buffer.Flush(&text);
  if (text.error) { system.io.buffer.ReportError(&text); system.Exit(1); }
}

static void send_text(void) {
  unsigned int count = 0u;
  for (unsigned int i = arg_start; i < line_length; ++i) {
    net_tx[count++] = (aurora_word_t)line[i];
    if (count == 64u) {
      if (system.telnet.Write(net_tx, count) != (int)count) goto failed;
      count = 0u;
    }
  }
  if (count && system.telnet.Write(net_tx, count) != (int)count) goto failed;
  if (system.telnet.PutChar('\r') != AURORA_OK || system.telnet.PutChar('\n') != AURORA_OK) goto failed;
  system.io.PutString("Sent to device (delivery not confirmed).\n");
  return;
failed:
  system.io.PutString("Telnet write failed.\n");
}

static void show_net_line(void) {
  system.io.buffer.Begin(&text);
  system.io.buffer.String(&text, "\n[net] ");
  if (net_discard_line)
    system.io.buffer.String(&text, "Line discarded (too long or input lost).");
  else
    for (aurora_word_t i = 0u; i < net_line_length; ++i)
      system.io.buffer.Char(&text, net_line[i]);
  system.io.buffer.String(&text, "\naurora> ");
  if (!discard_line) for (unsigned int i = 0; i < line_length; ++i) system.io.buffer.Char(&text, (aurora_word_t)line[i]);
  flush_text();
}

static void receive_text(void) {
  if (net_overflow) {
    net_overflow = 0u;
    net_read = net_write;
    net_line_length = 0u;
    net_skip_lf = 0;
    net_discard_line = 1;
  }
  for (unsigned int n = 0; n < 63u; ++n) {
    aurora_word_t read = net_read;
    if (read == net_write) break;
    aurora_word_t event = net_queue[read];
    net_read = (read + 1u) & 63u;
    aurora_word_t seq = event & 0x00ffff00u;
    if ((event & 0xff000000u) != 0xa6000000u || !seq || seq == net_sequence) continue;
    net_sequence = seq;
    aurora_word_t c = event & 0xffu;
    /* CRLF is one delimiter even across callbacks and foreground passes. */
    if (c == '\n' && net_skip_lf) { net_skip_lf = 0; continue; }
    net_skip_lf = 0;
    if (c == '\r' || c == '\n') {
      net_skip_lf = c == '\r';
      show_net_line();
      net_line_length = 0u;
      net_discard_line = 0;
    } else if (!net_discard_line) {
      if (net_line_length == NET_LINE_CAPACITY) {
        net_line_length = 0u;
        net_discard_line = 1;
      } else {
        /* Never execute remote commands or terminal control characters. */
        net_line[net_line_length++] = c >= 32u && c <= 126u ? c : '.';
      }
    }
  }
}

static void exit_command(void) {
  unsigned int i = arg_start, value = 0u;
  int negative = 0;
  if (i == line_length) { system.Exit(0); return; }
  if (line[i] == '+' || line[i] == '-') { negative = line[i] == '-'; ++i; }
  if (i == line_length || line[i] < '0' || line[i] > '9') goto invalid;
  while (i < line_length && line[i] >= '0' && line[i] <= '9') {
    unsigned int digit = (unsigned int)(line[i++] - '0');
    if (value > 214748364u || (value == 214748364u && digit > 7u + (unsigned int)negative)) goto invalid;
    value = value * 10u + digit;
  }
  while (i < line_length && space(line[i])) ++i;
  if (i != line_length) goto invalid;
  int result = 0;
  if (value != 0u) result = negative ? -1 - (int)(value - 1u) : (int)value;
  system.Exit(result);
  return;
invalid:
  system.io.PutString("Usage: exit [code], signed 32-bit integer.\n");
}

static void execute_line(void) {
  unsigned int start = 0u;
  while (start < line_length && space(line[start])) ++start;
  if (start == line_length) return;
  unsigned int end = start;
  while (end < line_length && !space(line[end])) ++end;
  arg_start = end;
  while (arg_start < line_length && space(line[arg_start])) ++arg_start;
  if (command_is(start, end, "echo")) {
    system.io.buffer.Begin(&text);
    for (unsigned int i = arg_start; i < line_length; ++i) system.io.buffer.Char(&text, (aurora_word_t)line[i]);
    system.io.buffer.Char(&text, '\n'); flush_text();
  } else if (command_is(start, end, "send")) {
    send_text();
  } else if (command_is(start, end, "exit")) {
    exit_command();
  } else if (command_is(start, end, "help")) {
    if (arg_start != line_length) { system.io.PutString("Usage: help\n"); return; }
    system.io.PutString("help         Show commands\necho TEXT    Print text\nsend TEXT    Telnet send with CRLF\nuptime       System time in ms\nclear        Clear screen\nabout        System information\nexit [CODE]  Exit with signed code\n");
  } else if (command_is(start, end, "uptime")) {
    if (arg_start != line_length) { system.io.PutString("Usage: uptime\n"); return; }
    system.io.buffer.Begin(&text);
    system.io.buffer.Unsigned(&text, system.TimeMs());
    system.io.buffer.String(&text, " ms\n"); flush_text();
  } else if (command_is(start, end, "clear")) {
    if (arg_start != line_length) { system.io.PutString("Usage: clear\n"); return; }
    system.io.PutChar('\f');
  } else if (command_is(start, end, "about")) {
    if (arg_start != line_length) { system.io.PutString("Usage: about\n"); return; }
    system.io.PutString("Aurora-32 mini terminal\nUser mode; IRQ keyboard; tick scheduling.\nBuilt-in commands only. No filesystem or program loader.\n");
  } else system.io.PutString("Unknown command. Type help.\n");
}

static void terminal_key(int c) {
  if (c == '\n' && skip_lf) { skip_lf = 0; return; }
  skip_lf = 0;
  if (c == '\r' || c == '\n') {
    skip_lf = c == '\r';
    system.io.PutChar('\n');
    if (discard_line) system.io.PutString("Line discarded; please try again.\n");
    else execute_line();
    line_length = 0u; discard_line = 0; prompt(); return;
  }
  if (discard_line) return;
  if (c == 8 || c == 127) {
    if (line_length) {
      --line_length;
      /* Digital Terminal deletes the preceding character on BS. */
      system.io.PutChar('\b');
    }
    return;
  }
  if (c < 32 || c > 126) return;
  if (line_length == LINE_CAPACITY - 1u) {
    discard_line = 1;
    system.io.PutString("\nLine too long. Press Enter to reset.\n");
    return;
  }
  line[line_length++] = c;
  system.io.PutChar((char)c);
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  line_length = 0u; skip_lf = discard_line = 0;
  net_read = net_write = net_overflow = net_sequence = 0u;
  net_line_length = 0u;
  net_skip_lf = net_discard_line = 0;
  if (kbqueue.Init() != AURORA_OK) {
    system.io.PutString("Keyboard subscription failed.\n");
    system.Exit(1);
  }
  if (system.telnet.Subscribe(telnet_callback) != AURORA_OK) {
    system.io.PutString("Telnet subscription failed.\n"); system.Exit(1);
  }
  system.io.PutString("\fAurora-32 mini terminal\nType help. Enter submits; Backspace edits.\n");
  prompt();
  for (;;) {
    receive_text();
    int c = kbqueue.Next();
    if (c == AURORA_KBQUEUE_OVERFLOW) {
      line_length = 0u; skip_lf = 0; discard_line = 1;
      system.io.PutString("\nInput lost. Press Enter to reset.\n");
    } else if (c >= 0) terminal_key(c);
    else aurora.Nop();
  }
}
