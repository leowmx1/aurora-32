# Verified bank-zero fallback (2026-11-07)

Kernel sources/build, storage build and user headers before multi-process changes.
Includes the new 22-bit user address layout and bank-zero initialization at
0x0D000000. The user reported successful hardware loading at this stage.

Software validation: full OS regression and Tiny C self-host fixed point passed
on the actual BIN / ideal ISA device model, not Digital timing.

ROM: `kernel/build/kernel.hex`
SHA256: `13715BE868438C8B20799A56EB0334C30AC7D341A01E34080C9E31354EBAF530`

Storage EEPROM: `storage-build/storage.hex`
SHA256: `0A9EA304677CED8A66A592795790C4C94D0290D3884B51CD528719E193E47163`

Re-loading Storage replaces runtime file edits; export the live EEPROM first.
This backup is still SYSTEM + one user slot, not the multi-process kernel.
