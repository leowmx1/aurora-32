# Program storage

Edit `programs.json` to choose executable names and HEX files. Paths are relative to this directory.
Build everything with `../build-tools/build.ps1`, or repack with `../build-tools/pack-storage.ps1`.
Each HEX needs its generated `.program.json` build metadata; rebuild old programs using the stable INT gateway.

Load `build/storage.hex` at local EEPROM address 0, mapped to CPU word address `0x0B000000`.
The installed device holds 2^24 32-bit words (64 MiB), through `0x0BFFFFFF`.
The packer defaults to this capacity; the HEX contains only the image, not full-capacity padding.
Load the normal kernel into ROM. UROM must allow kernel-only writes; no user image needs to be preloaded.
The terminal supports `ls`, `run calculator`, `snake`, `maze`, `gomoku`, `help`, `mount`, `halt`.
ASFS2 also supports `quickrun`, `ls PATH`, `tree`, `cat`, `mkdir`, `touch`, `rename`, `mv`, and `rm`.
Run `editor` to read/edit text through system calls. Storage WE must permit kernel writes.
Runtime edits affect the simulated EEPROM only: export it in Digital to preserve changes.
Reloading a freshly packed image overwrites runtime edits; there is no journal or automatic merge.

Manifest version 2 accepts `Name` paths and executable `Hex`, or `Type: Text`, `Source`, and
optional `ReserveWords` (default 2048, maximum 65536). Parent directories are generated automatically.
Text is ASCII, one character per 32-bit word. See [ASFS2](../docs/kernel/filesystem-v2.md).

`build/storage.json` describes image size, SHA-256 and file locations. All offsets and sizes are 32-bit words.
See [the format and hardware contract](../docs/kernel/storage.md).

## Uncompiled Tiny C examples

The image contains these editable source files as **Text**, not executables:

- `tinyc/hello.c`: greetings and the squares of 1 through 5.
- `tinyc/keys.c`: keyboard echo using `getchar()`; press `q` to finish.
- `tinyc/recursion.c`: recursive factorial and Fibonacci (720 and 55).
- `tinyc/arrays.c`: array sorting through a pointer argument.
- `tinyc/pointers.c`: address-of, dereference, word strings and 32-bit char.
- `tinyc/compiler.c`: the exact compiler source, reserved capacity 65536 words.
- `tinyc/readme.txt`: instructions and self-hosting steps.

The old loop-only Fibonacci and GCD examples have been replaced by the new
function/array/pointer examples. These changes do not delete runtime EEPROM files.

Use `ls tinyc` or `cat tinyc/hello.c` in the system console, or open the
files in `editor`. To compile one on Aurora-32, run `compiler`, enter
`tinyc/hello.c` at `Source >`, and a new executable name such as `hello`
at `Output >`. Then use `run hello`. Repacking the storage image only
copies these sources as text; it does not compile them.
