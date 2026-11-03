# Storage and loader regression

Build production ROM/user programs/storage with `../../build-tools/build.ps1`, then run
`python tests/storage/run_storage.py` from the project root.

The model starts with empty UROM and executes the actual production kernel. EEPROM bus reads and
kernel UROM writes are modeled; no loader/syscall is substituted by host code. Tests cover terminal
editing/listing, repeated launches/exits, graphics and all four screen games, user protection faults
halting, bad directory/format/checksum/ranges and unwritable UROM. UROM is now
volatile RAM; B remains EEPROM. The default image includes seven user programs.

Existing single-program regressions intentionally expect exit to halt. Build a separate test kernel:

```powershell
.\kernel\build.ps1 -DirectBoot -OutputDirectory .\tests\storage\build\direct-kernel
$env:AURORA_TEST_KERNEL_DIR=(Resolve-Path .\tests\storage\build\direct-kernel).Path
python tests/kernel-events/run_events.py
python tests/kernel-events/run_sleep.py
Remove-Item Env:AURORA_TEST_KERNEL_DIR
```

Rebuild contract C sources against the current gateway before running old tests. Do not replace
production kernel/build with this direct-boot variant. These are ideal ISA/device tests, not Digital
microcode/electrical timing tests. Real EEPROM write timing and privilege gating require circuit tests.

Run `python tests/storage/run_listing.py` after building the production kernel to check
`ls` and `programs` formatting. It executes the generated kernel against the normal
directory, a 31-character filename, and an empty directory. Names are left-aligned
to the longest name (at least 4 columns), types occupy a separate column, sizes are right-aligned in 12 columns,
and the unit appears only in the heading. Listing must not write UROM.

ASFS2 tests (actual kernel and user BINs, not mocked file operations):

```powershell
.\build-tools\build-user.ps1 tests/storage/fs_contract.c
python tests/storage/run_file_contract.py
python tests/storage/run_files.py
python tests/storage/run_capacity.py
python tests/storage/run_shutdown.py
```

The contract covers pointer/device bounds, enumeration, EOF, forbidden executable saves,
text save/checksum, and metadata-only moves/deletes (same-name move and delete: 1 write each), stable payload addresses,
cycle rejection, freed extent reuse and remounting. The interactive test covers line editing,
save/quit/reopen, management, empty files, quickrun vs normal verification, and readonly hardware.

`run_shutdown.py` injects the real memory/illegal-instruction vectors while a user runs,
including an in-flight GPU draw, and compares all 16,000 framebuffer pixels with an
independent font/rectangle reference. It checks no further user execution/ticks, no shell
return, completed GPU work before HALT, ordinary black-screen shutdown and stuck-GPU fallback.

Full build + current regression set: `./tests/run-regression.ps1` (Python required).
`run_ldma.py` checks actual loader code with an ideal asynchronous LDMA: register offsets,
multiple segments, no CPU UROM copying, CPU data/BSS fallback, verification failure and
busy/timeout shutdown. No CPU Storage/UROM access is allowed while the modeled engine is busy.
SUM models actual UROM readback, not the source/write-input bus. Tests require per-job reset,
reject destination-write and SUM faults, assert no CPU source-code/UROM reads during loading,
and cover unordered sources, gaps/padding, modulo-32 arithmetic and shared-source correction
without a full CPU checksum fallback. URAM data/BSS still receives CPU readback verification.
`run_remote.py` covers independent local/remote shells while a user is active, terminal ownership,
CRLF launch, busy-slot rejection, kill and live file-ID protection. `run_performance.py` compares
instruction counts with the recorded pre-change baseline and poisons uninitialized URAM to check
noinit safety. It also runs the bounded PutString/Write ABI contract.

`run_history.py` checks both terminals: newest-first numbering, the ten-entry limit,
multiple ring wraparounds, !1/!!/!9/!10 replay, invalid references and isolated histories.
