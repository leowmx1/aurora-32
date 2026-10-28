# MIDI syscall regression

From the repository root:

```powershell
.\build-tools\build.ps1
.\build-tools\build-user.ps1 .\tests\midi-syscall\contract.c -KernelObject .\kernel\build\kernel.o
python .\tests\midi-syscall\run_midi.py
.\build-tools\build-user.ps1 .\tests\midi-syscall\sequence_contract.c -KernelObject .\kernel\build\kernel.o
python .\tests\midi-syscall\run_sequence.py
python .\tests\snake-screen\run_snake_screen.py
```

Runs actual generated kernel/user binaries, including INT/IRET, tick scheduling,
keyboard callbacks and full context restoration. The ideal MIDI sink records MMIO
writes; it neither synthesizes sound nor simulates Digital microcode/electrical timing.
The current MIDI base is 0x0A000000, not 0x0F000000. The device model rejects
retired device-segment accesses and checks canonical note/program offsets 0..127.

Checks note/velocity/program boundaries, invalid input with no MMIO, reserved syscall
21, runtime arguments, exact address/data bits, note retrigger, zero-velocity release,
two simultaneous notes, idempotent AllNotesOff, callback API use, normal exit cleanup,
memory-fault cleanup and illegal-instruction cleanup. Snake tests navigate to food
and check food/ending effects and pause cancellation, alongside graphics controls.
The separate snake audio contract covers full short tunes and background looping.

The sequence contract checks bounded validation, copying user data, raw-note exclusion,
fixed deadlines and background position preservation across an effect. Its foreground
spins without querying audio or yielding: real tick interrupts must advance playback.
It repeats with a long graphics fill and across timer wraparound. Playback precision
is still limited by tick spacing and interrupt-disabled sections; this is not hard realtime.

Hardware listening test: load matching kernel.hex into ROM and snake-screen.hex into
UROM. Connect MIDI PC to data bit 8, select a melodic channel in the component, and
ensure host audio is enabled. Eat food to hear the short ascending effect and resumed background. No MIDI interrupt
or audio-completion polling is required. Check that Q/game-over does not leave a held note.
