#ifndef AURORA_KERNEL_LDMA_H
#define AURORA_KERNEL_LDMA_H
/* Kernel-only, word-addressed Storage -> UROM engine. There is no abort,
 * completion IRQ or error register. SUM accumulates actual UROM readback
 * modulo 2^32, not Storage/write-input data. Busy must cover the final readback
 * and accumulation. A CPU write asserts CS_DMAsum+WE and clears SUM. */
enum { LDMA_ORIGIN=0x0c000000u, LDMA_TARGET=0x0c000001u,
       LDMA_LENGTH=0x0c000002u, LDMA_STATUS=0x0c000003u,
       LDMA_SUM=0x0c000004u };
__attribute__((noinline)) static aurora_word_t
ldma_copy(aurora_word_t origin, aurora_word_t target, aurora_word_t words) {
  if (aurora.Read(LDMA_STATUS)) {
    kernel_boot_put_string("\nLDMA unexpectedly busy. Reset required.\n");
    kernel_shutdown();
  }
  aurora.Write(LDMA_ORIGIN,origin);
  aurora.Write(LDMA_TARGET,target);
  aurora.Write(LDMA_LENGTH,words);
  aurora.Write(LDMA_SUM,0u);
  aurora_word_t start=aurora.Read(AURORA_TIMER_DEVICE), polls=0u;
  aurora.Write(LDMA_STATUS,1u);
  /* IRQs stay enabled: tick/keyboard/Telnet can run, but no USER exists yet.
   * Do not read Storage or UROM until status becomes idle. */
  while(aurora.Read(LDMA_STATUS)) {
    if(aurora.Read(AURORA_TIMER_DEVICE)-start>=30000u || ++polls>=1000000u) {
      kernel_boot_put_string("\nLDMA timeout. Reset required.\n");
      /* No abort protocol: never return to a shell that could read Storage,
       * start another copy or execute partially written user code. */
      kernel_shutdown();
    }
  }
  return aurora.Read(LDMA_SUM);
}
#endif
