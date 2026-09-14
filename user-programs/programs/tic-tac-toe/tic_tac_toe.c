#include <aurora.h>
#include <aurora_system.h>

AURORA_USER_PROGRAM;

#define BOARD_SIZE 5
#define BOARD_CELLS 25
#define INPUT_QUEUE_SIZE 32

enum {
  DRAW_NORMAL = 0,
  DRAW_INVALID_COORDINATE = 1,
  DRAW_CELL_OCCUPIED = 2,
};

/* The keyboard callback is the producer and _start is the consumer. */
static volatile aurora_word_t input_queue[INPUT_QUEUE_SIZE]
    __attribute__((section(".noinit")));
static volatile aurora_word_t input_queue_read;
static volatile aurora_word_t input_queue_write;
static volatile aurora_word_t last_irq_sequence;
static volatile aurora_word_t keyboard_event_radix = 256u;

static volatile int board[BOARD_CELLS] __attribute__((section(".noinit")));
static volatile int current_player;
static volatile int move_count;
static volatile int pending_row;
static volatile int pending_column;

__attribute__((noinline, optnone)) static void
keyboard_callback(aurora_word_t event_word) {
  aurora_word_t write_index = input_queue_write;
  aurora_word_t next = (write_index + 1u) & (INPUT_QUEUE_SIZE - 1u);
  if (next == input_queue_read)
    return;
  input_queue[write_index] = event_word;
  input_queue_write = next;
}

static char piece_for_player(int player) {
  if (player == 1)
    return 'X';
  return '@';
}

static void draw_board(void) {
  system.io.PutChar('\f');
  system.io.PutString("  1 2 3 4 5\n");

  for (int row = 0; row < BOARD_SIZE; ++row) {
    system.io.PutChar((char)('A' + row));
    for (int column = 0; column < BOARD_SIZE; ++column) {
      int cell = board[row * BOARD_SIZE + column];
      system.io.PutChar(' ');
      if (cell == 0)
        system.io.PutChar(' ');
      else
        system.io.PutChar(piece_for_player(cell));
    }
    system.io.PutChar('\n');
  }
}

static void draw_turn(int message) {
  draw_board();
  if (message == DRAW_INVALID_COORDINATE)
    system.io.PutString("Invalid coordinate. Use A1-E5 or 1A-5E.\n");
  else if (message == DRAW_CELL_OCCUPIED)
    system.io.PutString("That cell is occupied.\n");

  system.io.PutString("Player ");
  system.io.PutInt(current_player);
  system.io.PutString(" > ");
}

__attribute__((noinline, optnone)) static int
line_is_winner(int start, int step, int player) {
  int index = start;
  for (int i = 0; i < BOARD_SIZE; ++i) {
    if (board[index] != player)
      return 0;
    index += step;
  }
  return 1;
}

__attribute__((noinline, optnone)) static int has_winning_line(int player) {
  for (int row = 0; row < BOARD_SIZE; ++row) {
    if (line_is_winner(row * BOARD_SIZE, 1, player))
      return 1;
  }
  for (int column = 0; column < BOARD_SIZE; ++column) {
    if (line_is_winner(column, BOARD_SIZE, player))
      return 1;
  }
  if (line_is_winner(0, BOARD_SIZE + 1, player))
    return 1;
  if (line_is_winner(BOARD_SIZE - 1, BOARD_SIZE - 1, player))
    return 1;
  return 0;
}

__attribute__((noreturn)) static void finish_game(int winner) {
  draw_board();
  if (winner != 0) {
    system.io.PutString("Player ");
    system.io.PutInt(winner);
    system.io.PutString(" wins!\n");
  } else {
    system.io.PutString("Draw!\n");
  }
  system.Exit(0);
  for (;;)
    aurora.Nop();
}

__attribute__((noinline, optnone)) static void submit_move(int row,
                                                          int column) {
  int index = row * BOARD_SIZE + column;
  if (board[index] != 0) {
    draw_turn(DRAW_CELL_OCCUPIED);
    return;
  }

  board[index] = current_player;
  ++move_count;
  if (has_winning_line(current_player))
    finish_game(current_player);
  if (move_count == BOARD_CELLS)
    finish_game(0);

  if (current_player == 1)
    current_player = 2;
  else
    current_player = 1;
  draw_turn(DRAW_NORMAL);
}

__attribute__((noinline, optnone)) static void
process_character(aurora_word_t character) {
  int c = (int)(character & 0xffu);

  if (c == '\r' || c == '\n')
    return;
  if (c >= 'a' && c <= 'e')
    c = c - 'a' + 'A';

  if (pending_row < 0 && pending_column < 0) {
    if (c >= 'A' && c <= 'E') {
      pending_row = c - 'A';
      system.io.PutChar((char)c);
      return;
    }
    if (c >= '1' && c <= '5') {
      pending_column = c - '1';
      system.io.PutChar((char)c);
      return;
    }
    draw_turn(DRAW_INVALID_COORDINATE);
    return;
  }

  if (pending_row >= 0 && c >= '1' && c <= '5') {
    int row = pending_row;
    pending_row = -1;
    system.io.PutChar((char)c);
    submit_move(row, c - '1');
    return;
  }

  if (pending_column >= 0 && c >= 'A' && c <= 'E') {
    int column = pending_column;
    pending_column = -1;
    system.io.PutChar((char)c);
    submit_move(c - 'A', column);
    return;
  }

  pending_row = -1;
  pending_column = -1;
  draw_turn(DRAW_INVALID_COORDINATE);
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();

  input_queue_read = 0u;
  input_queue_write = 0u;
  last_irq_sequence = 0u;
  current_player = 1;
  move_count = 0;
  pending_row = -1;
  pending_column = -1;
  for (int i = 0; i < BOARD_CELLS; ++i)
    board[i] = 0;

  (void)aurora_keyboard_subscribe(keyboard_callback);
  draw_turn(DRAW_NORMAL);

  for (;;) {
    aurora_word_t read_index = input_queue_read;
    if (read_index == input_queue_write) {
      aurora.Nop();
      continue;
    }

    aurora_word_t event_word = input_queue[read_index];
    input_queue_read = (read_index + 1u) & (INPUT_QUEUE_SIZE - 1u);
    if ((event_word & 0xff000000u) != 0xA5000000u)
      continue;

    aurora_word_t sequence =
        (event_word & 0x00ffff00u) / keyboard_event_radix;
    if (sequence == 0u || sequence == last_irq_sequence)
      continue;
    last_irq_sequence = sequence;
    process_character(event_word & 0xffu);
  }
}
