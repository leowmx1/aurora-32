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
/* Regular thin font rasterized at 12x16; no runtime scaling; bit0 is top-left. */
static const aurora_word_t font_bitmap[36][6] = {
  {0xf0000000u,0xc30c1980u,0x3643c430u,0x0c30c33cu,0x00f01983u,0x00000000u}, /* 0 */
  {0x60000000u,0x006c0780u,0x06006006u,0x60060060u,0x03fc0600u,0x00000000u}, /* 1 */
  {0xf8000000u,0x01801880u,0x18018018u,0x300600c0u,0x03fc0180u,0x00000000u}, /* 2 */
  {0xf8000000u,0x01801800u,0x0f008018u,0x00300180u,0x00fc1803u,0x00000000u}, /* 3 */
  {0xc0000000u,0x01a01e01u,0x1981981bu,0xfe18418cu,0x01801807u,0x00000000u}, /* 4 */
  {0xf8000000u,0x80080081u,0x1800f800u,0x00300300u,0x007c1801u,0x00000000u}, /* 5 */
  {0xe0000000u,0xc0180301u,0x31c1fc00u,0x0c30c30cu,0x00f01983u,0x00000000u}, /* 6 */
  {0xfc000000u,0x01803003u,0x0c00c018u,0x30060060u,0x00180300u,0x00000000u}, /* 7 */
  {0xf0000000u,0xc30c1980u,0x0f019810u,0x0c30c198u,0x00f838c3u,0x00000000u}, /* 8 */
  {0xf0000000u,0xc10c1880u,0x38c30c30u,0x80100378u,0x00780c01u,0x00000000u}, /* 9 */
  {0x60000000u,0x00d00f00u,0x18819809u,0x043fc108u,0x06062043u,0x00000000u}, /* A */
  {0xfc000000u,0xc30c18c0u,0x0fc18c10u,0x0c30c38cu,0x00fc18c3u,0x00000000u}, /* B */
  {0xe0000000u,0xc00c2181u,0x00c00c00u,0x0c00c00cu,0x01f02180u,0x00000000u}, /* C */
  {0xfc000000u,0xc30c18c0u,0x20c30c30u,0x0c30c30cu,0x007c18c3u,0x00000000u}, /* D */
  {0xf8000000u,0x80080081u,0x1f800800u,0x08008008u,0x01f80080u,0x00000000u}, /* E */
  {0xf8000000u,0x80080081u,0x1f800800u,0x08008008u,0x00080080u,0x00000000u}, /* F */
  {0xf0000000u,0xc00c2181u,0x3c400400u,0x0c30c304u,0x03f03183u,0x00000000u}, /* G */
  {0x0c000000u,0xc30c30c3u,0x3fc30c30u,0x0c30c30cu,0x030c30c3u,0x00000000u}, /* H */
  {0xfc000000u,0x00600601u,0x06006006u,0x60060060u,0x01fc0600u,0x00000000u}, /* I */
  {0xf8000000u,0x01801801u,0x18018018u,0x80180180u,0x00780c81u,0x00000000u}, /* J */
  {0x0c000000u,0xc0cc18c3u,0x03c02c06u,0xcc06c02cu,0x030c18c0u,0x00000000u}, /* K */
  {0x18000000u,0x80180180u,0x01801801u,0x18018018u,0x03f80180u,0x00000000u}, /* L */
  {0x0c000000u,0x429c38c3u,0x26425429u,0x04204264u,0x06062062u,0x00000000u}, /* M */
  {0x0c000000u,0xc31c31c3u,0x36c32c33u,0x8c34c34cu,0x030c38c3u,0x00000000u}, /* N */
  {0xf0000000u,0x430c1980u,0x20620430u,0x0c304206u,0x00f01983u,0x00000000u}, /* O */
  {0xfc000000u,0xc30c18c0u,0x18c30c30u,0x0c00c0fcu,0x000c00c0u,0x00000000u}, /* P */
  {0xf0000000u,0x430c1980u,0x20620430u,0x0c304204u,0x00f019c3u,0x3c044006u}, /* Q */
  {0xfc000000u,0xc10c18c0u,0x0fc18c10u,0x8c18c0ccu,0x030c30c1u,0x00000000u}, /* R */
  {0xf0000000u,0xc00c1180u,0x0f003800u,0x00300180u,0x00f81843u,0x00000000u}, /* S */
  {0xfc000000u,0x00600603u,0x06006006u,0x60060060u,0x00600600u,0x00000000u}, /* T */
  {0x0c000000u,0xc30c30c3u,0x30c30c30u,0x0c30c30cu,0x00f01883u,0x00000000u}, /* U */
  {0x06000000u,0xc30c2066u,0x19810830u,0xf0090198u,0x00600f00u,0x00000000u}, /* V */
  {0x06000000u,0x42042066u,0x26426420u,0x942d4264u,0x038c39c2u,0x00000000u}, /* W */
  {0x0c000000u,0x019810c3u,0x0600700fu,0x981d80f0u,0x030e30c1u,0x00000000u}, /* X */
  {0x06000000u,0x810c30c6u,0x0f009019u,0x60060060u,0x00600600u,0x00000000u}, /* Y */
  {0xfc000000u,0x01801003u,0x0600400cu,0x18030030u,0x03fc0080u,0x00000000u}, /* Z */
};
/* Single renderer owns this packet; callbacks do not nest. */
static aurora_graphics_image glyph_packet;
static aurora_graphics_image piece_packet;
static const aurora_word_t disk[18] = {
  0xc0007e00u,0xffe003ffu,0x0ffff007u,0xfc1ffff8u,0xfffe3fffu,0x7ffffe7fu,0xff7ffffeu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0x7ffffeffu,0xfe7ffffeu,0xfffc7fffu,0x1ffff83fu,0xe00ffff0u,0xffc007ffu,0x007e0003u
};

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
  if(x==0 && y==0 && w==160 && h==100) { w=480; h=240; }
  else { w=(x+w)*12/5-x*12/5; h=(y+h)*12/5-y*12/5; x=48+x*12/5; y=y*12/5; }
  if (system.graphics.FillRect(x, y, w, h, color) != AURORA_OK)
    fail("Graphics fill failed.\n");
}

__attribute__((noinline, optnone)) static void glyph(int x, int y, int c,
                                                       aurora_word_t color) {
  int index = c >= 'A' ? c - 'A' + 10 : c - '0';
  wait_graphics();
  glyph_packet.x = 49+x*12/5; glyph_packet.y = 2+y*12/5;
  glyph_packet.width=12; glyph_packet.height=16;
  glyph_packet.color = color; glyph_packet.background = BACKGROUND;
  glyph_packet.data=font_bitmap[index]; glyph_packet.words=6u;
  if (system.graphics.Image(&glyph_packet) != AURORA_OK)
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
  wait_graphics();
  piece_packet.x = 48+(BX+col*TILE+3)*12/5;
  piece_packet.y = (BY+row*TILE+3)*12/5;
  piece_packet.width = piece_packet.height = 24;
  piece_packet.color = player==1?BLACK:WHITE;
  piece_packet.background = BACKGROUND;
  piece_packet.data = disk; piece_packet.words = 18u;
  if (system.graphics.ImageTransparent(&piece_packet)!=AURORA_OK)
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
  /* Normal move updates only the 12x16 player digit, never the panel. */
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
    /* Logical 4x4 center mark leaves a black/white rim, never touches grid. */
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
