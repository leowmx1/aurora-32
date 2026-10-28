# Graphics demo (polling)

From repository root:

```powershell
./build-tools/build.ps1 -Program graphics-demo
```

Load matching kernel/build/kernel.hex into ROM and build/graphics-demo.hex into UROM.
No graphics completion IRQ is needed. Status at 0x09000009 must automatically
clear to zero after hardware finishes the fill.

Draws blue (40,25,80,50), red (5,5,25,15), then green (125,80,30,15).
Then draws an opaque red F on yellow at (56,40), a transparent red F at (104,40)
preserving the blue background, and a green 8x2 bar at (56,52) to check switching
back to rectangle mode. Both F bitmaps are 8x8 with bit0 at the top left.
All three large rectangles should remain. No background clear; clear/reset graphics
memory first if stale drawings remain.

The foreground submits through system.graphics.FillRect(), then polls Busy()
and calls system.Yield() while busy. It does not subscribe or call WaitEvent.
Tick and input interrupts remain active between the short system calls.
A 10-second busy timeout prints an error and exits with code 1; it does not cancel
the hardware operation. After all six operations finish, the program exits with code 0.

Bitmap descriptors use static storage, eight 32-bit words: x/y/w/h/color/background/
data_low/data_high. Use system.graphics.Bitmap() or BitmapTransparent(); do not
pass a user-stack descriptor. The kernel copies the descriptor before returning.

FillRect returns 0 for accepted, -5 for busy, -1 for invalid geometry.
Full contract: [graphics](../../../docs/kernel/graphics.md).
