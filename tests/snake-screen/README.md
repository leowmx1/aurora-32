# Screen Snake regression

```powershell
.\build-tools\build.ps1 -Program snake-screen
python .\tests\snake-screen\run_snake_screen.py
.\build-tools\build-user.ps1 .\tests\snake-screen\audio_contract.c -KernelObject .\kernel\build\kernel.o
python .\tests\snake-screen\run_audio.py
```

Runs actual kernel/user BINs using the existing ideal ISA and rectangle-fill model,
with no graphics IRQ. Not a Digital or microcode timing test.

Checks framebuffer tile colors,
numeric HUD pixels, three-rectangle normal moves, start/turn/pause/speed/quit, food
growth/score and wall exit, one full-screen clear, and complete callback restoration.
The terminal Snake has been removed; tests no longer depend on its source or BIN.
Generated program artifacts stay with its source.

MIDI checks cover food effects, immediate pause cancellation, silence while paused,
the four-note loss jingle before exit, and no held notes at WaitEvent or HALT.
audio_contract.c includes the game's actual sequence tables/submission code with a separate test entry;
it exercises complete start/food/loss/win tunes, two full quiet background loops,
pause and timer wraparound. The victory tune fixture is not a full-board gameplay test.
MIDI writes are made by the kernel, from syscalls or the tick-driven sequencer. No physical audio is synthesized.
Gameplay models a roughly 10 ms tick and checks one startup help message followed
by no terminal output during normal play.
The independent MIDI sequence regression checks frozen background resume and playback during slow drawing.

Default speed is checked as 20 ms/step (range still 10..150). M/m toggles are tested
before start, while running/paused, on resume and during the loss jingle. Muted
gameplay still eats food/scores/exits with no note messages. The background fixture
checks two cycles of the short Snake Battle motif, including silent rests, rather
than the old equal-duration eight-note arpeggio.
