#include <aurora.h>
#include <aurora_system.h>

AURORA_USER_PROGRAM;
#define SIZE 5
#define CELLS 25
#define BX 24
#define BY 16
#define TILE 16
/* Digital palette: 0x8000 + RGB555; warm wood and dark brown grid/text. */
#define BACKGROUND 0xe28du
#define INK 0x9ca3u
#define BLACK 1u
#define WHITE 0u
#define RED 2u
#define HIGHLIGHT 0x99d8u

static volatile int board[CELLS];
static volatile int current_player, move_count, pending_row, pending_column;
static int shown_player, shown_message;
static int winning_start, winning_step;
/* All game state after initialization is owned by non-nesting key callbacks. */
enum { NORMAL, BAD, OCCUPIED, WIN, DRAW, QUIT };
/* Original 3x5 font, expanded 2x offline to 6x10 (60 bits).
 * Bit0 is top-left; each glyph is one opaque bitmap, including its background. */
static const aurora_word_t font_bitmap[36][2] = {
  {0xf3cf3fffu, 0x0fffcf3cu}, /* 0 */
  {0x0c3cf30cu, 0x0fff30c3u}, /* 1 */
  {0xffc30fffu, 0x0fff0c3fu}, /* 2 */
  {0xffc30fffu, 0x0fffc30fu}, /* 3 */
  {0xffcf3cf3u, 0x0c30c30fu}, /* 4 */
  {0xff0c3fffu, 0x0fffc30fu}, /* 5 */
  {0xff0c3fffu, 0x0fffcf3fu}, /* 6 */
  {0x30c30fffu, 0x0c30c30cu}, /* 7 */
  {0xffcf3fffu, 0x0fffcf3fu}, /* 8 */
  {0xffcf3fffu, 0x0fffc30fu}, /* 9 */
  {0xffcf330cu, 0x0cf3cf3fu}, /* A */
  {0xcfcf33cfu, 0x03cfcf33u}, /* B */
  {0xc30c3fffu, 0x0fff0c30u}, /* C */
  {0xf3cf33cfu, 0x03cfcf3cu}, /* D */
  {0xcf0c3fffu, 0x0fff0c33u}, /* E */
  {0xcf0c3fffu, 0x00c30c33u}, /* F */
  {0xf30c3fffu, 0x0fffcf3cu}, /* G */
  {0xffcf3cf3u, 0x0cf3cf3fu}, /* H */
  {0x0c30cfffu, 0x0fff30c3u}, /* I */
  {0x30c30c30u, 0x0fffcf3cu}, /* J */
  {0xcfcf3cf3u, 0x0cf3cf33u}, /* K */
  {0xc30c30c3u, 0x0fff0c30u}, /* L */
  {0xfffffcf3u, 0x0cf3cf3fu}, /* M */
  {0xfffffcf3u, 0x0cf3ffffu}, /* N */
  {0xf3cf3fffu, 0x0fffcf3cu}, /* O */
  {0xffcf3fffu, 0x00c30c3fu}, /* P */
  {0xf3cf3fffu, 0x0c30fffcu}, /* Q */
  {0xcfcf33cfu, 0x0cf3cf33u}, /* R */
  {0xff0c3fffu, 0x0fffc30fu}, /* S */
  {0x0c30cfffu, 0x030c30c3u}, /* T */
  {0xf3cf3cf3u, 0x0fffcf3cu}, /* U */
  {0xf3cf3cf3u, 0x030ccf3cu}, /* V */
  {0xffcf3cf3u, 0x0cf3ffffu}, /* W */
  {0x0ccf3cf3u, 0x0cf3cf33u}, /* X */
  {0x0ccf3cf3u, 0x030c30c3u}, /* Y */
  {0x0cc30fffu, 0x0fff0c33u}, /* Z */
};
/* Single renderer owns this packet; callbacks do not nest. */
static aurora_graphics_bitmap glyph_packet;
static aurora_graphics_bitmap piece_packet;

static void fail(const char *message) {
  system.io.PutString(message);
  system.Exit(1);
}

static void wait_graphics(void) {
  aurora_word_t start = system.TimeMs();
  for (;;) {
    aurora_status_t busy = system.graphics.Busy();
    if (busy < 0) fail("Graphics status failed.\n");
    if (!busy) return;
    if (system.TimeMs() - start >= 10000u) fail("Graphics timeout (10 s).\n");
    system.Yield();
  }
}

static void fill(int x, int y, int w, int h, aurora_word_t color) {
  wait_graphics();
  if (system.graphics.FillRect(x, y, w, h, color) != AURORA_OK)
    fail("Graphics fill failed.\n");
}

__attribute__((noinline, optnone)) static void glyph(int x, int y, int c,
                                                       aurora_word_t color) {
  int index = c >= 'A' ? c - 'A' + 10 : c - '0';
  wait_graphics();
  glyph_packet.x = x; glyph_packet.y = y;
  glyph_packet.width = 6; glyph_packet.height = 10;
  glyph_packet.color = color; glyph_packet.background = BACKGROUND;
  glyph_packet.data_low = font_bitmap[index][0];
  glyph_packet.data_high = font_bitmap[index][1];
  if (system.graphics.Bitmap(&glyph_packet) != AURORA_OK)
    fail("Graphics bitmap failed.\n");
}

__attribute__((noinline, optnone)) static void text(int x, int y, const char *s,
                                                      aurora_word_t color) {
  while (*s) {
    if (*s != ' ') glyph(x, y, *s, color);
    x += 8;
    ++s;
  }
}

__attribute__((noinline)) static void draw_piece(int row, int col, int player) {
  int x = BX + col * TILE + 3, y = BY + row * TILE + 3;
  aurora_word_t color = player == 1 ? BLACK : WHITE;
  /* Same 80-pixel radius-five disk, split into two 10x5 masks (<64 bits).
   * Transparent zero bits preserve the corners. No runtime mask expansion. */
  wait_graphics();
  piece_packet.x = x; piece_packet.y = y;
  piece_packet.width = 10; piece_packet.height = 5;
  piece_packet.color = color; piece_packet.background = BACKGROUND;
  piece_packet.data_low = 0xdfe7f878u; piece_packet.data_high = 0x0003ffffu;
  if (system.graphics.BitmapTransparent(&piece_packet) != AURORA_OK)
    fail("Graphics piece failed.\n");
  wait_graphics();
  piece_packet.y = y + 5;
  piece_packet.data_low = 0x9fefffffu; piece_packet.data_high = 0x0000787fu;
  if (system.graphics.BitmapTransparent(&piece_packet) != AURORA_OK)
    fail("Graphics piece failed.\n");
}

/* Four one-pixel lines: highlight/restore only existing grid boundaries. */
__attribute__((noinline, optnone)) static void selection_border(aurora_word_t color) {
  int x = BX, y = BY, w = 81, h = 81;
  if (pending_row >= 0) { y += pending_row * TILE; h = TILE + 1; }
  else if (pending_column >= 0) { x += pending_column * TILE; w = TILE + 1; }
  else return;
  fill(x, y, w, 1, color);
  fill(x, y + h - 1, w, 1, color);
  fill(x, y, 1, h, color);
  fill(x + w - 1, y, 1, h, color);
}

static void clear_selection(void) {
  selection_border(INK);
  pending_row = pending_column = -1;
}

__attribute__((noinline, optnone)) static void draw_status(int message) {
  /* Normal move updates only the six-by-ten player digit, never the panel. */
  if (shown_player != current_player) {
    glyph(120, 8, '0' + current_player, current_player == 1 ? BLACK : WHITE);
    shown_player = current_player;
  }
  if (shown_message != message) {
    if (shown_message == BAD || shown_message == OCCUPIED)
      fill(112, 56, 40, 10, BACKGROUND);
    if (message == BAD) text(112, 56, "BAD", RED);
    else if (message == OCCUPIED) text(112, 56, "TAKEN", RED);
    else if (message >= WIN) {
      fill(112, 24, 40, 10, BACKGROUND);
      if (message == WIN) text(112, 24, "WIN", INK);
      else if (message == DRAW) text(112, 24, "DRAW", INK);
      else text(112, 24, "QUIT", INK);
    }
    shown_message = message;
  }
  wait_graphics();
}

/* Same five-in-row algorithm as the former terminal version. */
__attribute__((noinline, optnone)) static int line_is_winner(int start, int step, int player) {
  for (int i = 0; i < SIZE; ++i) {
    if (board[start] != player) return 0;
    start += step;
  }
  /* Preserve the first winning line in the existing row/column/diagonal scan. */
  winning_start = start - SIZE * step;
  winning_step = step;
  return 1;
}

__attribute__((noinline, optnone)) static int has_winning_line(int player) {
  for (int row = 0; row < SIZE; ++row)
    if (line_is_winner(row * SIZE, 1, player)) return 1;
  for (int col = 0; col < SIZE; ++col)
    if (line_is_winner(col, SIZE, player)) return 1;
  return line_is_winner(0, SIZE + 1, player) ||
         line_is_winner(SIZE - 1, SIZE - 1, player);
}

__attribute__((noinline, optnone)) static void mark_winning_line(void) {
  int index = winning_start;
  for (int i = 0; i < SIZE; ++i) {
    /* Four-by-four center mark leaves a black/white rim, never touches grid. */
    fill(BX + (index % SIZE) * TILE + 6,
         BY + (index / SIZE) * TILE + 6, 4, 4, RED);
    index += winning_step;
  }
}

static void finish(int message) {
  if (message == WIN) mark_winning_line();
  draw_status(message);
  if (message == WIN) {
    system.io.PutString("Player ");
    system.io.PutInt(current_player);
    system.io.PutString(" wins!\n");
  } else if (message == DRAW) system.io.PutString("Draw!\n");
  else system.io.PutString("Quit.\n");
  system.Exit(0);
}

__attribute__((noinline, optnone)) static void submit_move(int row, int col) {
  int index = row * SIZE + col;
  if (board[index]) { draw_status(OCCUPIED); return; }
  board[index] = current_player;
  ++move_count;
  draw_piece(row, col, current_player);
  if (has_winning_line(current_player)) { finish(WIN); return; }
  if (move_count == CELLS) { finish(DRAW); return; }
  current_player = 3 - current_player;
  draw_status(NORMAL);
}

__attribute__((noinline, optnone)) static void process_key(int c) {
  if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
  if (c == 'Q') { clear_selection(); finish(QUIT); return; }
  if (c == '\r' || c == '\n') return;
  if (c == 8 || c == 127 || c == 27) {
    clear_selection();
    draw_status(NORMAL);
    return;
  }
  if (pending_row < 0 && pending_column < 0) {
    if (c >= 'A' && c <= 'E') {
      pending_row = c - 'A'; selection_border(HIGHLIGHT); wait_graphics(); return;
    }
    if (c >= '1' && c <= '5') {
      pending_column = c - '1'; selection_border(HIGHLIGHT); wait_graphics(); return;
    }
  } else if (pending_row >= 0 && c >= '1' && c <= '5') {
    int row = pending_row;
    clear_selection();
    submit_move(row, c - '1');
    return;
  } else if (pending_column >= 0 && c >= 'A' && c <= 'E') {
    int col = pending_column;
    clear_selection();
    submit_move(c - 'A', col);
    return;
  }
  clear_selection();
  draw_status(BAD);
}

static void keyboard_callback(aurora_word_t event) {
  if (system.EventDrops()) fail("Input event queue overflow.\n");
  process_key((int)aurora_event_character(event));
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  system.DeferEvents();
  current_player = 1;
  move_count = 0;
  shown_player = 0;
  shown_message = NORMAL;
  winning_start = -1;
  winning_step = 0;
  pending_row = pending_column = -1;
  for (int i = 0; i < CELLS; ++i) board[i] = 0;
  if (system.io.Subscribe(keyboard_callback) != AURORA_OK) fail("Keyboard subscription failed.\n");
  if (system.io.PutString("Screen Gomoku: 5x5, five in a row wins.\n"
                          "P1 black first; P2 white. Full board: draw.\n"
                          "Type A3 or 3A (A-E, 1-5); lowercase works.\n"
                          "Second character places a stone; no Enter needed.\n"
                          "First letter highlights a row; first digit a column.\n"
                          "Backspace / Delete / Esc clears the selection.\n"
                          "Five red centers mark the winning line.\n"
                          "Q or q quits.\n") != AURORA_OK)
    system.Exit(1);
  fill(0, 0, 160, 100, BACKGROUND);
  for (int i = 0; i <= SIZE; ++i) {
    fill(BX + i * TILE, BY, 1, 81, INK);
    fill(BX, BY + i * TILE, 81, 1, INK);
  }
  for (int i = 0; i < SIZE; ++i) {
    glyph(BX + i * TILE + 5, 3, '1' + i, INK);
    glyph(12, BY + i * TILE + 3, 'A' + i, INK);
  }
  glyph(112, 8, 'P', INK);
  text(112, 24, "TURN", INK);
  draw_status(NORMAL);
  system.ResumeEvents();
  for (;;) system.WaitEvent();
}
