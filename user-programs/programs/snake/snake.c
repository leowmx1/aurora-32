#include <aurora.h>
#include <aurora_system.h>
#define SNAKE_WIDTH 12
#define SNAKE_HEIGHT 8
#define SNAKE_CELLS (SNAKE_WIDTH * SNAKE_HEIGHT)

enum { SNAKE_RUNNING, SNAKE_WALL, SNAKE_SELF, SNAKE_WIN };

typedef struct snake_game {
  int occupied[SNAKE_CELLS];
  int x[SNAKE_CELLS];
  int y[SNAKE_CELLS];
  int head, tail, length;
  int dx, dy, next_dx, next_dy, turn_pending;
  int food, score, state;
  unsigned int random_state;
} snake_game;

static int snake_next_index(int index) {
  ++index;
  if (index == SNAKE_CELLS)
    index = 0;
  return index;
}

/* Select among free cells; bounded work, even with only one free cell. */
static void snake_place_food(snake_game *g) {
  if (g->length == SNAKE_CELLS) {
    g->food = -1;
    g->state = SNAKE_WIN;
    return;
  }
  g->random_state = g->random_state * 1664525u + 1013904223u;
  unsigned int free_index = g->random_state % (unsigned int)(SNAKE_CELLS - g->length);
  for (int i = 0; i < SNAKE_CELLS; ++i) {
    if (!g->occupied[i]) {
      if (free_index == 0u) {
        g->food = i;
        return;
      }
      --free_index;
    }
  }
}

static void snake_init(snake_game *g, unsigned int seed) {
  for (int i = 0; i < SNAKE_CELLS; ++i)
    g->occupied[i] = 0;
  g->tail = 0;
  g->head = 2;
  g->length = 3;
  for (int i = 0; i < 3; ++i) {
    g->x[i] = 4 + i;
    g->y[i] = 4;
    g->occupied[4 * SNAKE_WIDTH + 4 + i] = 1;
  }
  g->dx = 1;
  g->dy = 0;
  g->next_dx = 1;
  g->next_dy = 0;
  g->turn_pending = 0;
  g->score = 0;
  g->state = SNAKE_RUNNING;
  g->random_state = seed;
  snake_place_food(g);
}

/* At most one turn between moves: a rapid W,A cannot reverse a rightward
 * snake before it actually moves up. Key repeats cannot fill a turn queue. */
static void snake_turn(snake_game *g, int dx, int dy) {
  if (g->state != SNAKE_RUNNING || g->turn_pending)
    return;
  if (dx == -g->dx && dy == -g->dy)
    return;
  if (dx == g->dx && dy == g->dy)
    return;
  g->next_dx = dx;
  g->next_dy = dy;
  g->turn_pending = 1;
}

static void snake_step(snake_game *g) {
  if (g->state != SNAKE_RUNNING)
    return;
  g->dx = g->next_dx;
  g->dy = g->next_dy;
  g->turn_pending = 0;
  int x = g->x[g->head] + g->dx;
  int y = g->y[g->head] + g->dy;
  if (x < 0 || x >= SNAKE_WIDTH || y < 0 || y >= SNAKE_HEIGHT) {
    g->state = SNAKE_WALL;
    return;
  }
  int cell = y * SNAKE_WIDTH + x;
  int grow = cell == g->food;
  int tail_cell = g->y[g->tail] * SNAKE_WIDTH + g->x[g->tail];
  /* Moving into the old tail is legal only when that tail moves away. */
  if (g->occupied[cell] && (grow || cell != tail_cell)) {
    g->state = SNAKE_SELF;
    return;
  }
  if (!grow) {
    g->occupied[tail_cell] = 0;
    g->tail = snake_next_index(g->tail);
  } else {
    ++g->length;
    ++g->score;
  }
  g->head = snake_next_index(g->head);
  g->x[g->head] = x;
  g->y[g->head] = y;
  g->occupied[cell] = 1;
  if (grow)
    snake_place_food(g);
}

AURORA_USER_PROGRAM;

#define INPUT_CAPACITY 32u
#define STEP_MIN_MS 10u
#define STEP_MAX_MS 150u
#define STEP_DEFAULT_MS 100u
#define STEP_CHANGE_MS 10u
static snake_game game;
static aurora_io_buffer screen;
static volatile aurora_word_t input_queue[INPUT_CAPACITY]
    __attribute__((section(".noinit")));
static volatile aurora_word_t input_read, input_write, input_overflow;
static volatile aurora_word_t event_radix = 256u;
static aurora_word_t last_sequence, last_move_ms, step_ms;
/* Keep flags word-sized: the current backend cannot store optimizer-created
 * i8 globals (global boolean shrinking). */
static volatile int started, paused, quit_requested, dirty;

static void keyboard_callback(aurora_word_t event) {
  aurora_word_t next = (input_write + 1u) & (INPUT_CAPACITY - 1u);
  if (next == input_read) {
    input_overflow = 1u;
    return;
  }
  input_queue[input_write] = event;
  input_write = next;
}

__attribute__((noinline)) static void draw_screen(void) {
  system.io.buffer.Begin(&screen);
  system.io.buffer.String(&screen, "\fSNAKE  Score:");
  system.io.buffer.Unsigned(&screen, (aurora_word_t)game.score);
  system.io.buffer.Char(&screen, '\n');
  system.io.buffer.String(&screen, "+-----------------------+\n");
  int head_cell = game.y[game.head] * SNAKE_WIDTH + game.x[game.head];
  for (int y = 0; y < SNAKE_HEIGHT; ++y) {
    system.io.buffer.Char(&screen, '|');
    for (int x = 0; x < SNAKE_WIDTH; ++x) {
      if (x != 0) system.io.buffer.Char(&screen, ' ');
      int cell = y * SNAKE_WIDTH + x;
      int c = ' ';
      if (cell == game.food) c = '*';
      if (game.occupied[cell]) c = 'o';
      if (cell == head_cell) c = '@';
      system.io.buffer.Char(&screen, (aurora_word_t)c);
    }
    system.io.buffer.String(&screen, "|\n");
  }
  system.io.buffer.String(&screen, "+-----------------------+\n");
  if (quit_requested) system.io.buffer.String(&screen, "Quit.\n");
  else if (game.state == SNAKE_WALL) system.io.buffer.String(&screen, "Hit wall. Game over.\n");
  else if (game.state == SNAKE_SELF) system.io.buffer.String(&screen, "Hit snake. Game over.\n");
  else if (game.state == SNAKE_WIN) system.io.buffer.String(&screen, "Board filled. You win!\n");
  else if (!started) system.io.buffer.String(&screen, "SPACE to start\n");
  else if (paused) system.io.buffer.String(&screen, "Paused: P to resume\n");
  else {
    system.io.buffer.Unsigned(&screen, step_ms);
    system.io.buffer.String(&screen, " ms/step\n");
  }
  system.io.buffer.String(&screen, "WASD move  P pause\n+/- speed  Q quit\n");
  system.io.buffer.Flush(&screen);
  if (screen.error != 0) {
    system.io.buffer.ReportError(&screen);
    system.Exit(1);
  }
  dirty = 0;
}

static void process_key(aurora_word_t character) {
  int c = (int)character;
  if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
  if (c == 'Q') {
    quit_requested = 1;
    dirty = 1;
  } else if (!started) {
    if (c == ' ') {
      started = 1;
      dirty = 1;
    }
  } else if (c == 'P' || c == ' ') {
    paused = !paused;
    dirty = 1;
  } else if (c == '+' || c == '=') {
    if (step_ms > STEP_MIN_MS) {
      step_ms = step_ms > STEP_MIN_MS + STEP_CHANGE_MS
                    ? step_ms - STEP_CHANGE_MS : STEP_MIN_MS;
      dirty = 1;
    }
  } else if (c == '-') {
    if (step_ms < STEP_MAX_MS) {
      step_ms = step_ms < STEP_MAX_MS - STEP_CHANGE_MS
                    ? step_ms + STEP_CHANGE_MS : STEP_MAX_MS;
      dirty = 1;
    }
  } else if (!paused) {
    if (c == 'W') snake_turn(&game, 0, -1);
    if (c == 'S') snake_turn(&game, 0, 1);
    if (c == 'A') snake_turn(&game, -1, 0);
    if (c == 'D') snake_turn(&game, 1, 0);
  }
}

static void consume_input(void) {
  /* Bound a drain pass so a stream of callbacks cannot starve movement. */
  for (unsigned int n = 0; n < INPUT_CAPACITY; ++n) {
    aurora_word_t read = input_read;
    if (read == input_write) break;
    aurora_word_t event = input_queue[read];
    input_read = (read + 1u) & (INPUT_CAPACITY - 1u);
    if ((event & 0xff000000u) != 0xa5000000u) continue;
    aurora_word_t seq = (event & 0x00ffff00u) / event_radix;
    if (seq == 0u || seq == last_sequence) continue;
    last_sequence = seq;
    process_key(event & 0xffu);
    if (quit_requested) break;
  }
  /* If input was lost, stop safely rather than keep moving blindly. */
  if (input_overflow) {
    input_overflow = 0u;
    paused = 1;
    dirty = 1;
  }
}

static void update_game(aurora_word_t now) {
  if (started && !paused && !quit_requested && !dirty &&
      (aurora_word_t)(now - last_move_ms) >= step_ms) {
    snake_step(&game);
    dirty = 1;
  }
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  input_read = input_write = input_overflow = 0u;
  last_sequence = 0u;
  started = paused = quit_requested = 0;
  step_ms = STEP_DEFAULT_MS;
  snake_init(&game, system.TimeMs());
  dirty = 1;
  if (aurora_keyboard_subscribe(keyboard_callback) != AURORA_OK) {
    system.io.PutString("Keyboard subscription failed.\n");
    system.Exit(1);
  }
  for (;;) {
    consume_input();
    update_game(system.TimeMs());
    if (dirty) {
      draw_screen();
      /* Do not catch up with a burst of invisible moves after slow output.
       * Unsigned differences handle timer wraparound. */
      last_move_ms = system.TimeMs();
    }
    if (quit_requested || game.state != SNAKE_RUNNING) system.Exit(0);
    /* Keyboard input is delivered by IRQ callbacks, not GetChar polling. */
    for (int i = 0; i < 128; ++i) aurora.Nop();
  }
}
