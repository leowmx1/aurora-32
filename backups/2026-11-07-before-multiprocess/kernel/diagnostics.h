#ifndef AURORA_KERNEL_DIAGNOSTICS_H
#define AURORA_KERNEL_DIAGNOSTICS_H
static volatile aurora_word_t diagnostic_scratch, diagnostic_timer_ok;
static aurora_word_t diagnostic_report(const char *name, int ok) {
  kernel_status_prefix(); kernel_boot_put_string(ok ? "[PASS] " : "[WARN] ");
  kernel_boot_put_string(name); console_putchar('\n'); return ok ? 0u : 1u;
}
static void kernel_self_test(void) {
  aurora_word_t warnings = 0u, old = diagnostic_scratch;
  diagnostic_scratch = 0x55aa55aau;
  int ok = diagnostic_scratch == 0x55aa55aau;
  diagnostic_scratch = 0xaa55aa55u;
  ok = ok && diagnostic_scratch == 0xaa55aa55u;
  diagnostic_scratch = old;
  warnings += diagnostic_report("Kernel RAM scratch cell", ok);
  warnings += diagnostic_report("System-call gateway", aurora.Read(2u) == 0x30u &&
      aurora.Read(3u) == (aurora_word_t)(unsigned long)aurora_syscall_entry);
  ok = aurora.Read(AURORA_IVT_KEYBOARD) == (aurora_word_t)(unsigned long)aurora_keyboard_irq &&
      aurora.Read(AURORA_IVT_TELNET) == (aurora_word_t)(unsigned long)aurora_telnet_irq &&
      aurora.Read(AURORA_IVT_TICK) == (aurora_word_t)(unsigned long)aurora_tick_irq &&
      aurora.Read(AURORA_IVT_USER_MEMORY_FAULT) == (aurora_word_t)(unsigned long)aurora_user_memory_fault &&
      aurora.Read(AURORA_IVT_USER_ILLEGAL_INSTRUCTION) == (aurora_word_t)(unsigned long)aurora_user_illegal_instruction;
  warnings += diagnostic_report("Interrupt vector registrations", ok);
  aurora_word_t start = aurora.Read(AURORA_TIMER_DEVICE);
  diagnostic_timer_ok = 0u;
  for (aurora_word_t i = 0; i < 65536u; ++i) {
    if (aurora.Read(AURORA_TIMER_DEVICE) != start) { diagnostic_timer_ok = 1u; break; }
    aurora.Nop();
  }
  warnings += diagnostic_report("Millisecond timer progression", diagnostic_timer_ok != 0u);
  warnings += diagnostic_report("EEPROM directory validation", fs_mount());
  kernel_status_prefix(); kernel_boot_put_string("Self-test complete: ");
  kernel_console_put_uint(warnings); kernel_boot_put_string(" warning(s).\n");
  kernel_boot_put_string("Basic checks only; use verify all for files.\n");
}
#endif
