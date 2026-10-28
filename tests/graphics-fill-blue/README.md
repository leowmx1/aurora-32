# Sequential non-overlapping rectangles smoke test

Standalone **kernel-mode ROM** program for the 160 x 100 rectangle-fill interface.
This is not a user program and requires neither kernel.hex nor a UROM image.
It does not change the regular kernel or the circuit.

The test retains the centered blue rectangle and adds two other rectangles:

| Order | Color/index | x, y | w, h | Inclusive pixel range |
|---|---|---|---|---|
| 1 | Blue / 4 | 40, 25 | 80, 50 | x=40..119, y=25..74 |
| 2 | Red / 2 | 5, 5 | 25, 15 | x=5..29, y=5..19 |
| 3 | Green / 3 | 125, 80 | 30, 15 | x=125..154, y=80..94 |

All three are within the 160 x 100 screen and do not overlap. At the end they must
all remain visible: red at top left, blue in the center, green at bottom right.
The earlier rectangles are not redrawn to hide corruption. The controller must
restart its scan from the new rectangle on every start command.
Pixels outside these rectangles must remain unchanged. Clear/reset graphics memory
before testing if the previous full-screen blue image is still present; this program
deliberately does not issue a background fill that could hide an incorrect write range.

From the repository root:

```powershell
.\build-tools\compile.ps1 -SourceFile .\tests\graphics-fill-blue\fill_blue.c -HexFile .\tests\graphics-fill-blue\build\fill_blue.hex -Mode kernel
```

Load `build/fill_blue.hex` into **ROM**, starting at address 0, then reset.
The program initializes KSP/KFP, disables interrupts,
and performs the following hardware word writes for each rectangle, in this order:

| Register | CPU address | Value |
|---|---|---|
| status/control | 0x09000009 | 0 (stop before changing parameters) |
| x | 0x09000000 | x from the table above |
| y | 0x09000001 | y from the table above |
| w | 0x09000002 | width from the table above |
| h | 0x09000003 | height from the table above |
| color | 0x09000004 | palette index from the table above |
| mode | 0x09000008 | 0 (rectangle, before start) |
| status/control | 0x09000009 | 1 (start after all parameters are set) |

All graphics registers now share segment 0x09; address bits [2:0] select a register.
These are word addresses: offsets are not multiplied by four. Offsets 5/6 are
reserved for future bitmap data/background and are never accessed by this test.
The old A..E graphics segments are not used; segment A is now MIDI.
Color 4 uses Digital's fixed palette, not a packed 0xRRGGBB value:
[upstream palette documentation](https://github.com/hneemann/Digital/blob/master/src/main/resources/lang/lang_de.xml).

Status/control at 0x09000009 is readable: writing 1 starts, writing 0 stops.
Hardware automatically clears status to 0 after the rectangle is complete.
After starting once, the program polls until status is zero. It does not require
observing 1 first: a small/fast operation may already have finished at the first read.
Only after completion does it stop/configure/start the next rectangle. There are
exactly three starts, with no clearing, fixed delays or terminal output between them.

The timer at 0x08000000 is read immediately before starting and after observing
completion for each rectangle. After all three are complete, the program prints
their elapsed times once, for example:

```text
Fill complete: x=40 y=25 w=80 h=50 blue, 40 ms.
Fill complete: x=5 y=5 w=25 h=15 red, 4 ms.
Fill complete: x=125 y=80 w=30 h=15 green, 5 ms.
```

The numbers above are illustrative, not measurements. Unsigned elapsed-time subtraction
handles timer rollover for durations under 2^32 ms. Timing includes the start write
and polling/detection overhead, excludes parameter setup and terminal printing, and
does not establish when Digital's GUI has finished repainting. A result of 0 ms is
possible at millisecond resolution. If status never clears, it keeps waiting and
does not falsely announce completion; no timeout or automatic retry is implemented.

After printing, it idles with NOP/JMP to keep the common clock running.
Width/height are treated as pixel counts. The program deliberately does not HALT.
Actual rectangle scan, completion and display depend on the hardware fill circuit.
The generated code is checked separately; real fill timing must be measured in Digital.

To return to normal applications, reload the matching kernel ROM and user UROM images.
