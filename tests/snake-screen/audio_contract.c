/* Exercise the actual game's static sequencer without requiring a full-board win. */
#define _start snake_game_start
#include "../../user-programs/programs/snake-screen/snake_screen.c"
#undef _start

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  started = 1;
  sound_enabled = 1;
  paused = quit_requested = music_started = 0;
  game.state = SNAKE_RUNNING;
  sound_mode = SOUND_NONE;
  midi_check(system.midi.Program(0u));
  play_sound(SOUND_START);
  while (sound_mode != SOUND_MUSIC) { update_sound(); system.Yield(); }
  /* Let two complete background cycles play before interrupting it. */
  aurora_word_t begin = system.TimeMs();
  while ((aurora_word_t)(system.TimeMs() - begin) < 20000u) {
    update_sound(); system.Yield();
  }
  play_sound(SOUND_FOOD);
  while (sound_mode != SOUND_MUSIC) { update_sound(); system.Yield(); }
  paused = 1;
  sync_sound();
  begin = system.TimeMs();
  while ((aurora_word_t)(system.TimeMs() - begin) < 400u) {
    sync_sound(); update_sound(); system.Yield();
  }
  paused = 0;
  game.state = SNAKE_WALL;
  play_sound(SOUND_LOSE);
  while (sound_mode != SOUND_NONE) { update_sound(); system.Yield(); }
  game.state = SNAKE_WIN;
  play_sound(SOUND_WIN);
  while (sound_mode != SOUND_NONE) { update_sound(); system.Yield(); }
  system.io.PutString("SNAKE AUDIO PASS\n");
  system.Exit(0);
  for (;;) system.Yield();
}
