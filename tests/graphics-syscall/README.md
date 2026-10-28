# Graphics syscall regression (no graphics IRQ)

Run from repository root:

```powershell
.\build-tools\build.ps1 -Program graphics-demo
.\build-tools\build-user.ps1 .\tests\graphics-syscall\contract.c -KernelObject .\kernel\build\kernel.o
python .\tests\graphics-syscall\run_graphics.py
.\build-tools\build-user.ps1 .\tests\graphics-syscall\bitmap_contract.c -KernelObject .\kernel\build\kernel.o
python .\tests\graphics-syscall\run_bitmap_syscall.py
```

Executes actual kernel/user BINs in an ideal ISA/device model, not Digital.
The graphics engine only auto-clears busy; it NEVER raises a completion IRQ.
Registers are 0x09000000..08, with status/control at 0x09000009. The model rejects
noncanonical aliases and retired device segments. Rectangles require eight writes
(including mode=0), bitmaps eleven, ending with control=1.
Checks immediate/delayed completion, preserved rectangles, wrapper/raw bounds,
negative/overflowing inputs, zero dimensions, full-screen and bottom-right edge,
dynamic arguments, busy rejection without writes, normal tick and five keyboard
callback context restores. Both demo and contract check a stuck-busy timeout.

The model asserts no write to graphics vector 0x03000003 and no WaitEvent during
drawing. The contract checks that retired request 21 returns ENOSYS. Existing
keyboard/Telnet event handling is exercised by tests/kernel-events.

The bitmap contract covers pointer/mode/reserved/geometry validation, 64-bit masks,
bit31/32, variable width, transparency, snapshot semantics, mode reset, completion
timing, normal tick and 13 callbacks, and timeout. The URAM end-boundary probe
saves/restores the final eight words because the compiler puts globals there.
