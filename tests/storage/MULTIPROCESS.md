# Multiprocess regression

Current PUSHU/POPU keep KernelMode: they select USP but use physical kernel
addressing. The model must not silently apply user bank translation to them.
The kernel injects callback returns with `Write(USP + bank_base, trampoline)`
and `LUSP(USP + 1)` instead. The Tetris test includes the reported single-key
failure scenario: process-demo in bank 0 plus foreground Tetris in bank 1.

`run_background.py` covers syscall 36 (system.Background), preserving bank and
stack, callback/defer rejection, occupied quota, idempotence, input handoff,
local/remote ownership and compiler's foreground fallback when BG is busy.
It also checks silent BG PutChar/PutInt/PutString/Write, two buffered batches,
success counts and retained argument errors. Between dispatch and return, the
test observes both terminal devices and asserts no writes (including controls).
Readiness is observed via Sleep syscalls, not stdout that must now be discarded.
`compiler/run_selfhost.py` also keeps a foreground calculator alive while the
compiler compiles itself in the background and checks byte-identical stages.

Build the production kernel and storage first, then:

```powershell
./build-tools/compile.ps1 tests/storage/multiprocess_contract.c tests/storage/build/multiprocess_contract.hex -Mode user -ExtraArguments '-DAURORA_SYSCALL_ENTRY_ADDRESS=2'
pypy3 tests/storage/run_multiprocess.py
pypy3 tests/storage/run_remote.py
```

The full `tests/run-regression.ps1` includes both and `run_banked_io.py`.
The latter runs the full existing filesystem save/read/list, executable-install,
and console-pointer contracts in bank 3 while other users remain alive.
These execute emitted BINs;
they do not replace kernel syscalls, callbacks, or scheduling with host stubs.
Hardware bank mapping, IRQ entry/IRET, and devices are ideal contract models,
not Digital propagation timing or a real Telnet socket.

`run_multiprocess.py` uses an in-memory copy of the storage image with two test
executables; it does not replace the production storage file. It loads the same
C fixture into all four banks, keeps recursive stack frames alive over Sleep,
checks per-bank globals, routes simultaneous local/remote callback bursts, and
checks full register/FLAGS/PC/USP/UFP callback restoration. All four users must
advance under a 3001-instruction tick period. A pending tick at the first IRET
must not indefinitely starve one fixed slot.

It also covers nonzero-bank FS request/output buffers, bitmap and MIDI packets,
DMA bus exclusion while other users exist, terminal foreground/background
quotas, immediate background exit notifications while BOTH foregrounds remain
occupied, device ownership, live file pins, slot-4 kill, and slot reuse. A final
case uses the actual `process-demo &` and calculator from production storage.

`run_remote.py` additionally runs two independent calculator instances, validates
CRLF handoff and shell history, and tests editor file-ID pinning. Before hardware
testing, back up any edited EEPROM image. Load the matching kernel and storage;
all three user memories need four physical 2^22-word banks, kernel bypass, the
D-segment bank register, and the four USP/UFP instructions.
