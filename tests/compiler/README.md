# Tiny C tests

Use the bundled build tools to build kernel/compiler/editor and repack storage
first. The tests execute Aurora machine instructions; they do not compile input
Tiny C on the host or substitute a host-language expression evaluator.

- `run_compiler.py`: actual kernel, compile/save/remount/load/execute; arithmetic,
  recursive and forward functions, arrays/pointers, lifetime/initialization,
  short circuit, malformed source, collisions, divide by zero, Sleep and input.
- `run_install.py`: actual kernel + compiled `install_contract.c`, valid AEXE,
  bounds, bad pointers, existing path protection and read-only storage failures.
- `run_tinyc_examples.py`: verify packed text equals source, compile/run all the
  small examples, check removed legacy examples are absent.
- `run_selfhost.py`: actual kernel + source from ASFS, compiler -> compiler2 ->
  compiler3, byte-for-byte fixed point, then compile/run an array program with
  compiler3. Includes normal tick scheduling and callback context checks.
- `run_native.py`: faster isolated user-ISA/syscall-contract bootstrap check for
  debugging. Does **not** replace the real-kernel test or model tick scheduling.
- `run_limits.py`: isolated 8-argument/return ABI and source/code/frame capacity
  rejection checks; never installs an output after a rejected compilation.

Run with `pypy3` when available; CPython also works but takes much longer.
Self-hosting can execute tens of millions of instructions. Progress is printed
every ten million instructions. `build/selfhost.json` records the current source
and result hashes. Generated test files are ignored by Git.

Full suite: `tests/run-regression.ps1 -Python pypy3 -SelfHost`.
All devices and CPU instructions are idealized; no Digital propagation timing,
real elapsed time or electrical stability is validated by these scripts.
