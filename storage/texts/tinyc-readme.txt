Tiny C sources (not precompiled)

hello.c      Output, arithmetic and a loop.
keys.c       Keyboard echo; q finishes. Callback + WaitEvent input.
recursion.c  Recursive factorial and Fibonacci: 720 and 55.
arrays.c     Sort an array through a pointer argument.
pointers.c   Address-of, dereference, strings and 32-bit char storage.
compiler.c   The compiler itself: exact seed source, no rewriting.

Example:
  run compiler
  Source > tinyc/arrays.c
  Output > sorted
  Run in background? [Y/n] > Y
  (wait for the compiler exit notice with code 0)
  run sorted

Self-host:
  run compiler
  Source > tinyc/compiler.c
  Output > compiler2
  Run in background? [Y/n] > Y
  (wait for the compiler exit notice with code 0)
  run compiler2
  Source > tinyc/compiler.c
  Output > compiler3
  Run in background? [Y/n] > Y

Use new output names; existing files are never overwritten.
After both filenames, choose Y/Enter for background, N for foreground, q to cancel.
The shell prompt returns early. Use ps or another foreground program while waiting.
Background stdout is discarded. Choose N for progress and detailed errors.
If the background slot is occupied, compilation continues in foreground.
Filesystem busy (-5) is retried with Yield, including source reads during remote ls.
Start with run compiler, not compiler &: filenames still need foreground input.
Self-compilation takes much longer than the small examples.
Do not reset during Saving executable. Export your EEPROM to keep results.
All sources use int main(). This is a C subset, not full ISO C:
no for, ++, structs, floating point, function pointers or general #include.
The compiler's one bootstrap.h include is a recognized seed-runtime bridge.
Only global URAM buffers may be passed to filesystem syscalls.
