#include <aurora.h>
#include <aurora_system.h>

AURORA_USER_PROGRAM;
#define MAZE_W 15
#define MAZE_H 11
#define MAZE_SIZE (MAZE_W * MAZE_H)
static int passages[MAZE_SIZE];
static int path_stack[35], choices[4];
static int player_x, player_y;
static unsigned int random_state, moves, start_ms;
static aurora_io_buffer screen;
/* Keep the redraw flag word-sized; the backend cannot store shrunken i8 globals. */
static volatile int full_redraw;
static int painted_x, painted_y, previous_moves, previous_seconds;

static unsigned int maze_random(void) {
  random_state ^= random_state << 13;
  random_state ^= random_state >> 17;
  random_state ^= random_state << 5;
  return random_state;
}

static void reset_player(void) {
  player_x = player_y = 1;
  moves = 0u;
  start_ms = system.TimeMs();
}

/* Iterative randomized DFS: 7x5 rooms, no recursion or dynamic allocation. */
static void new_maze(unsigned int seed) {
  random_state = seed != 0u ? seed : 0x6d2b79f5u;
  for (int i = 0; i < MAZE_SIZE; ++i) passages[i] = 0;
  int top = 0;
  path_stack[0] = MAZE_W + 1;
  passages[MAZE_W + 1] = 1;
  while (top >= 0) {
    int cell = path_stack[top];
    int x = cell % MAZE_W, y = cell / MAZE_W;
    int count = 0;
    if (x > 1 && !passages[cell - 2]) choices[count++] = cell - 2;
    if (x < MAZE_W - 2 && !passages[cell + 2]) choices[count++] = cell + 2;
    if (y > 1 && !passages[cell - 2 * MAZE_W]) choices[count++] = cell - 2 * MAZE_W;
    if (y < MAZE_H - 2 && !passages[cell + 2 * MAZE_W]) choices[count++] = cell + 2 * MAZE_W;
    if (count == 0) { --top; continue; }
    int next = choices[maze_random() % (unsigned int)count];
    passages[(cell + next) / 2] = 1;
    passages[next] = 1;
    path_stack[++top] = next;
  }
  reset_player();
  full_redraw = 1;
}

#define TILE 8
#define MAP_X 20
#define MAP_Y 12
/* Digital 0x8000 + RGB555 palette: cream, muted teal, coral. */
#define BACKGROUND 0xf798u
#define WALL 0xa1eeu
#define MARKER 0xed27u

/* Original 3x5 font, expanded 2x offline to 6x10 (60 bits).
 * Bit0 is top-left; each glyph is one opaque bitmap, including its background. */
static const aurora_word_t font_bitmap[12][2] = {
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
  {0xff0c3fffu, 0x0fffc30fu}, /* S */
  {0x0c30cfffu, 0x030c30c3u}, /* T */
};
/* Single renderer owns this packet; callbacks do not nest. */
static aurora_graphics_bitmap glyph_packet;

static void graphics_fail(int error) {
  system.io.PutString("Graphics error: ");
  system.io.PutInt(error);
  system.io.PutChar('\n');
  system.Exit(1);
}

static void wait_graphics(void) {
  aurora_word_t start = system.TimeMs();
  for (;;) {
    aurora_status_t busy = system.graphics.Busy();
    if (busy < 0) graphics_fail(busy);
    if (!busy) return;
    if (system.TimeMs() - start >= 10000u) {
      system.io.PutString("Graphics timeout (10 s).\n");
      system.Exit(1);
    }
    system.Yield();
  }
}

static void fill(int x, int y, int w, int h, aurora_word_t color) {
  wait_graphics();
  aurora_status_t result = system.graphics.FillRect(x, y, w, h, color);
  if (result != AURORA_OK) graphics_fail(result);
}

__attribute__((noinline, optnone)) static void draw_glyph(int x, int glyph) {
  wait_graphics();
  glyph_packet.x = x; glyph_packet.y = 0;
  glyph_packet.width = 6; glyph_packet.height = 10;
  glyph_packet.color = WALL;
  glyph_packet.background = BACKGROUND;
  glyph_packet.data_low = font_bitmap[glyph][0];
  glyph_packet.data_high = font_bitmap[glyph][1];
  aurora_status_t result = system.graphics.Bitmap(&glyph_packet);
  if (result != AURORA_OK) graphics_fail(result);
}

/* Keep this small loop out of constant-division/unrolling transforms. The
 * bundled backend can omit the scratch-offset reload across draw_glyph calls
 * in that expanded form (e.g. the 009 -> 010 transition). */
__attribute__((noinline, optnone)) static void draw_number(int x, int value, int previous) {
  int divisor = 100;
  for (int digit = 0; digit < 3; ++digit) {
    int number = value / divisor % 10;
    if (previous < 0 || number != previous / divisor % 10)
      draw_glyph(x + digit * 8, number);
    divisor /= 10;
  }
}

/* Restore the cell the player left, including its start/exit marker. */
static void restore_cell(int x, int y) {
  int px = MAP_X + x * TILE, py = MAP_Y + y * TILE;
  fill(px, py, TILE, TILE, BACKGROUND);
  if (x == 1 && y == 1) fill(px + 3, py + 3, 2, 2, WALL);
  if (x == MAZE_W - 2 && y == MAZE_H - 2) {
    fill(px + 1, py + 1, 6, 6, MARKER);
    fill(px + 2, py + 2, 4, 4, BACKGROUND);
  }
}

static void finish_game(int escaped) {
  unsigned int seconds = (system.TimeMs() - start_ms) / 1000u;
  system.io.buffer.Begin(&screen);
  if (escaped) system.io.buffer.String(&screen, "Escaped! Well done.\n");
  else system.io.buffer.String(&screen, "Quit.\n");
  system.io.buffer.String(&screen, "Steps: ");
  system.io.buffer.Unsigned(&screen, moves);
  system.io.buffer.String(&screen, "  Time: ");
  system.io.buffer.Unsigned(&screen, seconds);
  system.io.buffer.String(&screen, "s\n");
  system.io.buffer.Flush(&screen);
  if (screen.error) {
    system.io.buffer.ReportError(&screen);
    system.Exit(1);
  }
  system.Exit(0);
}

static void initialize_screen(void) {
  fill(0, 0, 160, 100, BACKGROUND);
  painted_x = painted_y = -1;
  previous_moves = previous_seconds = -1;
  draw_glyph(20, 10);
  draw_glyph(100, 11);
  if (system.io.PutString("Screen Maze: S=steps, T=seconds (last 3 digits)\n"
                      "Teal=wall, cream=path, coral square=you\n"
                      "Small teal=start, coral outline=exit\n"
                      "WASD move  R restart  N new maze  Q quit\n") != AURORA_OK)
    system.Exit(1);
}

/* Keyboard callbacks never nest. Busy/Yield keeps tick running while drawing;
 * queued keyboard events are handled after the current callback returns. */
__attribute__((noinline)) static void draw_maze(void) {
  if (full_redraw) {
    fill(MAP_X, MAP_Y, MAZE_W * TILE, MAZE_H * TILE, BACKGROUND);
    for (int y = 0; y < MAZE_H; ++y) {
      int x = 0;
      while (x < MAZE_W) {
        if (passages[y * MAZE_W + x]) { ++x; continue; }
        int begin = x;
        while (x < MAZE_W && !passages[y * MAZE_W + x]) ++x;
        fill(MAP_X + begin * TILE, MAP_Y + y * TILE, (x - begin) * TILE, TILE, WALL);
      }
    }
    restore_cell(1, 1);
    restore_cell(MAZE_W - 2, MAZE_H - 2);
    painted_x = painted_y = -1;
    full_redraw = 0;
  }
  if (painted_x != player_x || painted_y != player_y) {
    if (painted_x >= 0) restore_cell(painted_x, painted_y);
    fill(MAP_X + player_x * TILE + 2, MAP_Y + player_y * TILE + 2, 4, 4, MARKER);
    painted_x = player_x;
    painted_y = player_y;
  }
  int displayed_moves = (int)(moves % 1000u);
  int displayed_seconds = (int)(((system.TimeMs() - start_ms) / 1000u) % 1000u);
  draw_number(28, displayed_moves, previous_moves);
  draw_number(108, displayed_seconds, previous_seconds);
  previous_moves = displayed_moves;
  previous_seconds = displayed_seconds;
  wait_graphics();
}

static void maze_key(int c) {
  if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
  if (c == 'Q') { finish_game(0); return; }
  if (c == 'R') { reset_player(); draw_maze(); return; }
  if (c == 'N') { new_maze(maze_random() ^ system.TimeMs()); draw_maze(); return; }
  int x = player_x, y = player_y;
  if (c == 'W') --y;
  else if (c == 'S') ++y;
  else if (c == 'A') --x;
  else if (c == 'D') ++x;
  else return;
  if (x < 0 || x >= MAZE_W || y < 0 || y >= MAZE_H || !passages[y * MAZE_W + x]) return;
  player_x = x; player_y = y; ++moves;
  draw_maze();
  if (x == MAZE_W - 2 && y == MAZE_H - 2) {
    finish_game(1);
  }
}

static void keyboard_callback(aurora_word_t event) {
  if (system.EventDrops() != 0u) {
    system.io.PutString("\nInput event queue overflow. Exiting safely.\n");
    system.Exit(1);
  }
  maze_key((int)(aurora_event_character(event)));
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  system.DeferEvents();
  if (system.io.Subscribe(keyboard_callback) != AURORA_OK) system.Exit(1);
  new_maze(system.TimeMs());
  initialize_screen();
  draw_maze();
  system.ResumeEvents();
  for (;;) system.WaitEvent();
}
