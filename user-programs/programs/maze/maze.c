#include <aurora.h>
#include <aurora_system.h>
#include <aurora_kbqueue.h>

AURORA_USER_PROGRAM;
#define MAZE_W 15
#define MAZE_H 11
#define MAZE_SIZE (MAZE_W * MAZE_H)
static int passages[MAZE_SIZE];
static int path_stack[35], choices[4];
static int player_x, player_y;
static unsigned int random_state, moves, start_ms;
static aurora_io_buffer screen;

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
}

static void draw_maze(void) {
  system.io.buffer.Begin(&screen);
  system.io.buffer.String(&screen, "\fMAZE  Steps:");
  system.io.buffer.Unsigned(&screen, moves);
  system.io.buffer.String(&screen, "  Time:");
  system.io.buffer.Unsigned(&screen, (system.TimeMs() - start_ms) / 1000u);
  system.io.buffer.String(&screen, "s\n");
  for (int y = 0; y < MAZE_H; ++y) {
    for (int x = 0; x < MAZE_W; ++x) {
      if (x != 0) system.io.buffer.Char(&screen, ' ');
      int c = passages[y * MAZE_W + x] ? ' ' : '#';
      if (x == 1 && y == 1) c = 'S';
      if (x == MAZE_W - 2 && y == MAZE_H - 2) c = 'E';
      if (x == player_x && y == player_y) c = '@';
      system.io.buffer.Char(&screen, (aurora_word_t)c);
    }
    system.io.buffer.Char(&screen, '\n');
  }
  system.io.buffer.String(&screen, "@ you  S start  E exit\nWASD move  R restart\nN new maze  Q quit\n");
  system.io.buffer.Flush(&screen);
  if (screen.error) { system.io.buffer.ReportError(&screen); system.Exit(1); }
}

static void maze_key(int c) {
  if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
  if (c == 'Q') { system.io.PutString("Quit.\n"); system.Exit(0); return; }
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
    system.io.PutString("Escaped! Well done.\n");
    system.Exit(0);
  }
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  if (kbqueue.Init() != AURORA_OK) {
    system.io.PutString("Keyboard subscription failed.\n");
    system.Exit(1);
  }
  new_maze(system.TimeMs());
  draw_maze();
  for (;;) {
    int c = kbqueue.Next();
    if (c == AURORA_KBQUEUE_OVERFLOW) system.io.PutString("Input queue full; keys discarded.\n");
    else if (c >= 0) maze_key(c);
    else aurora.Nop();
  }
}
