# Self-hosting Tiny C

Native Aurora user-mode compiler. Build the LLVM seed with
`user-programs/build.ps1 -Program compiler`, then repack storage.

Run `compiler`, enter `tinyc/arrays.c`, then a new output name such as `sorted`.
After input, `Run in background? [Y/n] >` offers Y/Enter for background,
N for foreground, and q to cancel. Background (syscall 36) releases the terminal.
The early shell prompt does not mean compilation is finished: wait for
the named process exit notice with code 0 before running the result.
Background stdout is silently discarded by the kernel. Choose N to see progress
and detailed errors; background output is not saved for later viewing.
If that terminal's background slot is occupied, compilation stays foreground.
File Open/Info/Read/Install retry EBUSY with Yield, including foreground source
reading while the other terminal executes filesystem commands such as ls.
Use `ps` to inspect it; do not start it with `compiler &`, since input is required.
For self-hosting, use the exact
`tinyc/compiler.c` source and output `compiler2`, then repeat with compiler2.

During foreground compilation, `Function: NAME` reports each function definition before
its body is compiled. Prototypes are silent; long function bodies may still
take time between reports. The same progress output works in self-hosted builds.

No existing output is overwritten. Source examples stay uncompiled in storage.
The one bootstrap header is the LLVM seed's OS/runtime bridge, not compiler logic.

[Language, limits, ABI and self-host tests](../../../docs/user_program/compiler/README.md).
