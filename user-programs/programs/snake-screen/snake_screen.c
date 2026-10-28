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

#define STEP_MIN_MS 10u
#define STEP_MAX_MS 150u
#define STEP_DEFAULT_MS 20u
#define STEP_CHANGE_MS 10u
static snake_game game;
static aurora_io_buffer screen;
static aurora_word_t last_move_ms, step_ms;
/* Keep flags word-sized: the current backend cannot store optimizer-created
 * i8 globals (global boolean shrinking). */
static volatile int started, paused, quit_requested, dirty;

/* Kernel-owned sequences advance on tick independently of drawing. Background
 * playback is frozen during an effect, then resumes at the interrupted phase. */
enum { SOUND_NONE, SOUND_MUSIC, SOUND_START, SOUND_FOOD, SOUND_LOSE, SOUND_WIN };
/* Short monophonic opening motif, WePie Snake Battle BGM (bars 1..4):
 * https://www.tan8.com/yuepu-57642.html
 * Quarter=128; each row is note, velocity, hold_ms, gap_ms; 128 means rest. */
#define MUSIC_COUNT 25
static const aurora_word_t music_steps[MUSIC_COUNT * 4] = {
  72u, 42u, 264u, 87u,
  71u, 42u, 88u, 29u,
  72u, 42u, 264u, 87u,
  71u, 42u, 88u, 29u,
  72u, 42u, 351u, 117u,
  67u, 42u, 351u, 117u,
  69u, 42u, 351u, 117u,
  72u, 42u, 264u, 87u,
  75u, 42u, 88u, 29u,
  74u, 42u, 264u, 87u,
  72u, 42u, 88u, 29u,
  74u, 42u, 351u, 117u,
  72u, 42u, 264u, 87u,
  71u, 42u, 88u, 29u,
  72u, 42u, 264u, 87u,
  71u, 42u, 88u, 29u,
  72u, 42u, 351u, 117u,
  68u, 42u, 264u, 87u,
  67u, 42u, 88u, 29u,
  128u, 42u, 0u, 234u,
  79u, 42u, 351u, 117u,
  78u, 42u, 176u, 58u,
  79u, 42u, 176u, 58u,
  128u, 42u, 0u, 234u,
  128u, 42u, 0u, 468u,
};
static const aurora_word_t start_steps[12] = {
  72,90,90,30, 76,90,90,30, 79,90,90,30
};
static const aurora_word_t food_steps[12] = {
  84,90,70,30, 88,90,70,30, 91,90,70,30
};
static const aurora_word_t lose_steps[16] = {
  67,90,130,30, 63,90,130,30, 60,90,130,30, 48,90,220,30
};
static const aurora_word_t win_steps[16] = {
  72,90,110,30, 76,90,110,30, 79,90,110,30, 84,90,260,30
};
static volatile int sound_mode, music_started, sound_enabled;

static void midi_check(aurora_status_t result) {
  if (result != AURORA_OK) {
    system.io.PutString("MIDI syscall failed. Load matching kernel.\n");
    system.Exit(1);
  }
}

static void stop_sound(void) {
  if (sound_mode != SOUND_NONE) midi_check(system.midi.AllNotesOff());
  sound_mode = SOUND_NONE;
}

__attribute__((noinline, optnone)) static void play_sound(int mode) {
  if (!sound_enabled) { stop_sound(); return; }
  if (mode == SOUND_MUSIC || mode == SOUND_START) {
    stop_sound();
    midi_check(system.midi.Sequence(music_steps, MUSIC_COUNT, 1u));
    if (mode == SOUND_START) midi_check(system.midi.Sequence(start_steps, 3u, 2u));
  } else if (mode == SOUND_FOOD) {
    /* Do not reload or stop the background: the kernel retains its position. */
    midi_check(system.midi.Sequence(food_steps, 3u, 2u));
  } else {
    stop_sound();
    if (mode == SOUND_LOSE) midi_check(system.midi.Sequence(lose_steps, 4u, 2u));
    else midi_check(system.midi.Sequence(win_steps, 4u, 2u));
  }
  sound_mode = mode;
}

/* Only completion bookkeeping; no user-side note deadlines or note writes. */
static void update_sound(void) {
  if (!sound_enabled) { stop_sound(); return; }
  if (sound_mode == SOUND_NONE || sound_mode == SOUND_MUSIC) return;
  aurora_status_t busy = system.midi.SequenceBusy();
  if (busy < 0) midi_check(busy);
  if (busy == 0) sound_mode = SOUND_NONE;
  else if (busy == 1) sound_mode = SOUND_MUSIC;
}

static void sync_sound(void) {
  if (quit_requested || !sound_enabled) { stop_sound(); return; }
  if (game.state != SNAKE_RUNNING) return;
  if (!started || paused) { stop_sound(); return; }
  if (sound_mode == SOUND_NONE) {
    play_sound(music_started ? SOUND_MUSIC : SOUND_START);
    music_started = 1;
  }
}

/* 160x100 screen: 12x8 square cells, with a 2-pixel border and numeric HUD.
 * Digital RGB555 palette: muted slate, sage, gold and coral. */
#define CELL_SIZE 10
#define BOARD_X 20
#define BOARD_Y 16
#define COLOR_BACKGROUND 0x8ca7u
#define COLOR_BODY 0xb2f0u
#define COLOR_HEAD 0xff2cu
#define COLOR_FOOD 0xfdabu
static int painted[SNAKE_CELLS];
static int previous_score, previous_step, previous_border;
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

static void graphics_fail(int code) {
  system.io.PutString("Graphics error: ");
  system.io.PutInt(code);
  system.io.PutChar('\n');
  system.Exit(1);
}

/* No completion IRQ. Keep tick enabled while waiting in user mode.
 * Only this foreground renderer submits fills, never keyboard callbacks. */
static void wait_graphics(void) {
  aurora_word_t start = system.TimeMs();
  for (;;) {
    aurora_status_t busy = system.graphics.Busy();
    if (busy < 0) graphics_fail(busy);
    if (busy == 0) return;
    if (system.TimeMs() - start >= 10000u) {
      system.io.PutString("Graphics timeout (10 s).\n");
      system.Exit(1);
    }
    system.Yield();
  }
}

static void fill(int x, int y, int width, int height, aurora_word_t color) {
  wait_graphics();
  aurora_status_t result = system.graphics.FillRect(x, y, width, height, color);
  if (result != AURORA_OK) graphics_fail(result);
}

__attribute__((noinline, optnone)) static void draw_glyph(int x, int glyph) {
  wait_graphics();
  glyph_packet.x = x; glyph_packet.y = 2;
  glyph_packet.width = 6; glyph_packet.height = 10;
  glyph_packet.color = COLOR_BODY;
  glyph_packet.background = COLOR_BACKGROUND;
  glyph_packet.data_low = font_bitmap[glyph][0];
  glyph_packet.data_high = font_bitmap[glyph][1];
  aurora_status_t result = system.graphics.Bitmap(&glyph_packet);
  if (result != AURORA_OK) graphics_fail(result);
}

static void draw_number(int x, int value, int previous) {
  int divisor = 100;
  for (int digit = 0; digit < 3; ++digit) {
    int number = value / divisor % 10;
    if (previous < 0 || number != previous / divisor % 10)
      draw_glyph(x + digit * 8, number);
    divisor /= 10;
  }
}

static void print_exit_status(void) {
  /* Normal gameplay is terminal-silent, including food/pause/speed/M changes. */
  system.io.buffer.Begin(&screen);
  system.io.buffer.String(&screen, "SCREEN SNAKE  Score:");
  system.io.buffer.Unsigned(&screen, (aurora_word_t)game.score);
  system.io.buffer.String(&screen, "  ");
  system.io.buffer.Unsigned(&screen, step_ms);
  system.io.buffer.String(&screen, " ms/step\n");
  if (quit_requested) system.io.buffer.String(&screen, "Quit.\n");
  else if (game.state == SNAKE_WALL) system.io.buffer.String(&screen, "Hit wall. Game over.\n");
  else if (game.state == SNAKE_SELF) system.io.buffer.String(&screen, "Hit snake. Game over.\n");
  else if (game.state == SNAKE_WIN) system.io.buffer.String(&screen, "Board filled. You win!\n");
  system.io.buffer.Flush(&screen);
  if (screen.error) {
    system.io.buffer.ReportError(&screen);
    system.Exit(1);
  }
}

static void initialize_screen(void) {
  fill(0, 0, 160, 100, COLOR_BACKGROUND);
  for (int i = 0; i < SNAKE_CELLS; ++i) painted[i] = 0;
  previous_score = previous_step = previous_border = -1;
  draw_glyph(20, 10);  /* S: score */
  draw_glyph(100, 11); /* T: milliseconds per step */
}

__attribute__((noinline)) static void draw_screen(void) {
  int border = (!started || paused || quit_requested ||
                game.state != SNAKE_RUNNING) ? COLOR_HEAD : COLOR_BODY;
  if (border != previous_border) {
    fill(18, 14, 124, 2, border);
    fill(18, 96, 124, 2, border);
    fill(18, 16, 2, 80, border);
    fill(140, 16, 2, 80, border);
    previous_border = border;
  }
  int head_cell = game.y[game.head] * SNAKE_WIDTH + game.x[game.head];
  for (int y = 0; y < SNAKE_HEIGHT; ++y) {
    for (int x = 0; x < SNAKE_WIDTH; ++x) {
      int cell = y * SNAKE_WIDTH + x;
      int kind = 0;
      if (cell == game.food) kind = 3;
      if (game.occupied[cell]) kind = 1;
      if (cell == head_cell) kind = 2;
      if (painted[cell] == kind) continue;
      int px = BOARD_X + x * CELL_SIZE + 1;
      int py = BOARD_Y + y * CELL_SIZE + 1;
      aurora_word_t color = COLOR_BACKGROUND;
      if (kind == 1) color = COLOR_BODY;
      if (kind == 2) color = COLOR_HEAD;
      fill(px, py, 8, 8, color);
      if (kind == 3) fill(px + 2, py + 2, 4, 4, COLOR_FOOD);
      painted[cell] = kind;
    }
  }
  draw_number(28, game.score, previous_score);
  draw_number(108, (int)step_ms, previous_step);
  previous_score = game.score;
  previous_step = (int)step_ms;
  /* Finish before releasing deferred callbacks or exiting the process. */
  wait_graphics();
  dirty = 0;
}

static void process_key(aurora_word_t character) {
  int c = (int)character;
  if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
  if (c == 'Q') {
    quit_requested = 1;
    dirty = 1;
  } else if (c == 'M') {
    sound_enabled = !sound_enabled;
    if (started) music_started = 1;
    /* Audio changes need no redraw and must not reset the movement deadline. */
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

static void keyboard_callback(aurora_word_t event) {
  if (system.EventDrops() != 0u) {
    system.io.PutString("\nInput event queue overflow. Exiting safely.\n");
    system.Exit(1);
  }
  process_key(aurora_event_character(event));
}

static void update_game(aurora_word_t now) {
  if (game.state == SNAKE_RUNNING && started && !paused && !quit_requested && !dirty &&
      (aurora_word_t)(now - last_move_ms) >= step_ms) {
    int old_score = game.score;
    snake_step(&game);
    if (game.state == SNAKE_WIN) play_sound(SOUND_WIN);
    else if (game.state != SNAKE_RUNNING) play_sound(SOUND_LOSE);
    else if (game.score != old_score) play_sound(SOUND_FOOD);
    dirty = 1;
  }
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  started = paused = quit_requested = 0;
  music_started = 0;
  sound_enabled = 1;
  sound_mode = SOUND_NONE;
  midi_check(system.midi.Program(0u));
  step_ms = STEP_DEFAULT_MS;
  snake_init(&game, system.TimeMs());
  dirty = 1;
  system.DeferEvents();
  if (system.io.Subscribe(keyboard_callback) != AURORA_OK) {
    system.io.PutString("Keyboard subscription failed.\n");
    system.Exit(1);
  }
  /* Print once before the first frame; food/pause/speed changes stay silent. */
  if (system.io.PutString("Screen Snake\n"
                          "SPACE start / pause   WASD move\n"
                          "P pause / resume     M music + sound on/off\n"
                          "+/- speed (10..150 ms, default 20)\n"
                          "Q quit   S=score   T=ms/step\n") != AURORA_OK)
    system.Exit(1);
  initialize_screen();
  draw_screen();
  last_move_ms = system.TimeMs();
  system.ResumeEvents();
  for (;;) {
    system.DeferEvents();
    sync_sound();
    update_sound();
    update_game(system.TimeMs());
    if (dirty) {
      draw_screen();
      /* Do not catch up with a burst of invisible moves after slow output.
       * Unsigned differences handle timer wraparound. */
      last_move_ms = system.TimeMs();
    }
    if (quit_requested || (game.state != SNAKE_RUNNING && sound_mode == SOUND_NONE)) {
      stop_sound();
      print_exit_status();
      system.Exit(0);
    }
    /* Calculate while events are deferred. A callback delivered by ResumeEvents
     * or immediately before Sleep leaves a kernel wake credit, so stale timing
     * cannot put us back to sleep after a pause/speed/mute/quit request. */
    int wait_input = (!started || paused) && sound_mode == SOUND_NONE;
    aurora_word_t delay = 20u; /* Ending sound completion bookkeeping only. */
    if (game.state == SNAKE_RUNNING && started && !paused) {
      aurora_word_t elapsed = system.TimeMs() - last_move_ms;
      delay = elapsed < step_ms ? step_ms - elapsed : 0u;
    }
    system.ResumeEvents();
    if (wait_input) system.WaitEvent();
    else if (system.Sleep(delay) != AURORA_OK) {
      system.io.PutString("Sleep failed. Load matching kernel.\n");
      system.Exit(1);
    }
  }
}
