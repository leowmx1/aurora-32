# Kernel event regression

These tests execute the generated kernel and user BINs in an ideal instruction-level model.
The model does not implement syscalls or scheduling on the kernel's behalf: the emitted kernel code does that.
It is not a Digital circuit, microcode, propagation-delay or socket test.

Run from the repository root (Python 3 required only for testing):

```powershell
.\build-tools\build.ps1
.\build-tools\build-user.ps1 .\tests\kernel-events\event_contract.c -KernelObject .\kernel\build\kernel.o
python .\tests\kernel-events\run_events.py
python .\tests\kernel-events\run_core_examples.py
python .\tests\kernel-events\run_examples.py
.\build-tools\build-user.ps1 .\tests\kernel-events\sleep_contract.c -KernelObject .\kernel\build\kernel.o
python .\tests\kernel-events\run_sleep.py
```

`run_events.py` tests two tick periods (10007 and 3001 model instructions): mixed-source ordering,
63 admitted/7 dropped from a 70-event burst, nested deferral, blocked wait, unsubscribe compaction,
callbacks yielding twice, and wakeup/idle handoff while no tick is due.
Each run checks all 16 registers, FLAGS, PC, USP and UFP against the suspended context on 69 returns.
The idle check also asserts that a blocked user executes no instructions and the system does not spin in Yield syscalls.
Screen Snake is covered separately by `tests/snake-screen/run_snake_screen.py`.
Sleep covers real timed blocking with no subscription/user work/idle Yield spam,
zero/invalid/reserved arguments, deferred/callback rejection, key and Telnet early
wake, wake credit before Sleep, cancellation of stale deadlines, subsequent
indefinite WaitEvent, timer wrap and MIDI note changes while the user is blocked.
Snake additionally verifies early turn/Q wake without a due tick at a 150ms step.
The former terminal Snake regression has been removed with that program.

The other scripts cover calculator/trace, signed-range guessing,
and terminal commands/Telnet bursts/split CRLF/cross-device non-nesting.
The former terminal maze and board programs are removed. Their screen replacements
are tested by `tests/maze-screen/run_maze_screen.py` and `tests/gomoku-screen/run_gomoku_screen.py`.
Tests run outside `user-programs/programs`: example folders remain C source, README and build output only.
Generated test images live in this test's own build/; they are not an additional shipped user example.

`run_target.py` and `run_system.py` provide the underlying ISA and banked-stack models restored from
the archived earlier harness; use the commands above for the current ABI, not their legacy standalone runners.
