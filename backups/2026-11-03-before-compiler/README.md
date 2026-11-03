# Before editor rename / native compiler (2026-11-03)

Recovery pair:

- Load `kernel/kernel.hex` into kernel ROM at 0.
- Load `storage/storage.hex` into Storage (B) at device offset 0.
- Reset the CPU. The old `txt-viewer` program and all other packaged programs remain available.
- UROM is RAM and will be populated by the loader. No separate user HEX is needed.

The storage image was copied BEFORE repacking for the compiler. The kernel is a fresh
build of the pre-compiler source (61,393 words): the new executable-install syscall
and editor rename were removed in this independent source snapshot. This includes
the existing LDMA UROM-readback-SUM loader; matching LDMA hardware is still required.
The snapshot does not change or replace the working tree's new implementation.

SHA-256:

```
kernel/kernel.hex  5DCC6659640B6DB39B40098E563D7EDAEE2A5B6AD2D4DC5DD8D46417D13CA618
storage/storage.hex AFBF78B30135503576120E879432433B218E959EB49B2E1E965846AF074DBA0E
```

This preserves disk images, NOT unsaved EEPROM edits inside a running Digital
simulation. Export that EEPROM separately before loading a new storage image.
Circuit/microcode files are not changed by this software backup.

Rebuild the recovery kernel from the project root, if needed:

```powershell
./build-tools/compile.ps1 backups/2026-11-03-before-compiler/source/kernel/kernel.c backups/2026-11-03-before-compiler/kernel/kernel.hex -Mode kernel
```
