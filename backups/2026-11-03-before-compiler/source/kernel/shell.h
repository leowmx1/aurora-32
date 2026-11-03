#ifndef AURORA_KERNEL_SHELL_H
#define AURORA_KERNEL_SHELL_H
#include "loader.h"
#include "diagnostics.h"
#include "console_screen.h"
/* Buffers belong to SYSTEM foreground; IRQs only enqueue device words. */
#define SHELL_HISTORY_LIMIT 10u
typedef struct shell_session {
  aurora_word_t line[64], history[SHELL_HISTORY_LIMIT * 64u];
  volatile aurora_word_t length, overflow, skip_lf, count, next;
} shell_session;
/* Unused history slots and text tails are never consumed. */
static shell_session consoles[2] __attribute__((section(".noinit")));
#define shell_line (consoles[shell_console].line)
#define shell_history (consoles[shell_console].history)
#define shell_length (consoles[shell_console].length)
#define shell_overflow (consoles[shell_console].overflow)
#define shell_skip_lf (consoles[shell_console].skip_lf)
#define history_count (consoles[shell_console].count)
#define history_next (consoles[shell_console].next)
#define shell_needs_prompt (console_prompt[shell_console])
#define shell_needs_banner (console_banner[shell_console])
static volatile aurora_word_t boot_report_pending;
static void shell_put_words(const aurora_word_t *p) {
  while (*p) console_putchar(*p++);
}
static int shell_equal(const aurora_word_t *a, const char *b) {
  while (*a && *a == *b) { ++a; ++b; }
  return *a == *b;
}
static void shell_prompt(void) { kernel_boot_put_string(shell_console?"aurora:remote:/ > ":"aurora:/ > "); }
static void shell_banner(void) {
  if(!process_table[PROCESS_USER].valid && (!shell_console || user_console)) console_screen_logo();
  kernel_boot_put_string(shell_console?"Aurora-32 Remote System Console\n":"Aurora-32 System Console\n");
  kernel_boot_put_string("----------------------------------------\n");
  if (boot_report_pending && !shell_console) {
    kernel_boot_put_string("BIOS startup: ");
    kernel_console_put_uint(aurora.Read(AURORA_TIMER_DEVICE));
    kernel_boot_put_string(" ms since reset.\n");
    boot_report_pending = 0u;
  }
  if (fs_words) {
    kernel_console_put_uint(fs_program_count());
    kernel_boot_put_string(" programs available. Type ls to browse.\n");
  } else kernel_boot_put_string("Storage unavailable. Load image; mount.\n");
  kernel_boot_put_string("help: commands | history: recent commands\n");
}
static void shell_initialize(void) {
  for(aurora_word_t i=0u;i<2u;++i) {
    consoles[i].length=consoles[i].overflow=consoles[i].skip_lf=0u;
    consoles[i].count=consoles[i].next=0u; consoles[i].line[0]=0u;
  }
  kernel_boot_status("Performing basic system self-test.");
  kernel_self_test();
  aurora.DisableInterrupt();
  keyboard_read = keyboard_write; telnet_read = telnet_write;
  console_putchar('\f');
  boot_report_pending = 1u;
  console_prompt[0]=console_prompt[1]=console_banner[0]=console_banner[1]=1u;
}
static void shell_help(const aurora_word_t *topic) {
  if (!*topic) {
    kernel_boot_put_string("PROGRAMS AND STORAGE\n"
      "  ls [PATH]        List directory\n"
      "  tree / cat PATH  Directory tree / text\n"
      "  mkdir / touch    Create directory / text\n"
      "  rm PATH          Remove file / empty dir\n"
      "  rename PATH NAME Rename in same directory\n"
      "  mv PATH DEST     Move without copying data\n"
      "  run NAME         Load and execute\n"
      "  quickrun NAME    Skip checksum comparison\n"
      "  stat NAME        File and image details\n"
      "  verify NAME|all  Check without loading\n"
      "  df               Storage image usage\n"
      "  mount            Re-read the directory\n"
      "SYSTEM\n"
      "  about / version  Build and ABI info\n"
      "  uptime           Milliseconds since reset\n"
      "  ps               Process and exit status\n"
      "  mem              Memory address layout\n"
      "  devices          Device address map\n"
      "  selftest         Non-destructive checks\n"
      "CONSOLE AND NETWORK\n"
      "  echo TEXT        Print text\n"
      "  netstat          Remote console traffic\n"
      "  kill [1|user]    Terminate user program\n"
      "  history          Last 10; newest is 1\n"
      "  !! / !N          Repeat last/history item\n"
      "  clear            Clear and show banner\n"
      "  halt             Clear screen and stop CPU\n"
      "help run / help edit / help storage\n");
  } else if (shell_equal(topic, "run")) {
    kernel_boot_put_string("run NAME, or NAME alone (case-sensitive).\n"
      "LDMA copies and verifies UROM before execution.\n"
      "quickrun NAME skips checksum comparison.\n"
      "Only one user program runs at a time.\n"
      "Exit returns here; protection faults halt.\n");
  } else if (shell_equal(topic, "edit")) {
    kernel_boot_put_string("Enter: submit (CR/LF/CRLF).\n"
      "Backspace/Delete: erase last character.\n"
      "Esc/Ctrl-U: discard current command.\n"
      "63 characters maximum; overlong lines fail.\n"
      "history, !!, !1..!10: recall commands.\n"
      "1 is newest; !! is the same as !1.\n"
      "No ANSI arrow-key sequences required.\n");
  } else if (shell_equal(topic, "storage")) {
    kernel_boot_put_string("ASFS2: directories, executables and text.\n"
      "Storage and UROM need kernel-only writes.\n"
      "df: device capacity and image size in words.\n"
      "verify all checks files without loading.\n"
      "mkdir/touch/rm/rename/mv manage entries.\n"
      "mv/rm do not copy file data.\n"
      "txt-viewer edits text; no journal or cd.\n"
      "Export EEPROM in Digital to keep edits.\n");
  } else kernel_boot_put_string("Unknown help topic. Try help.\n");
}
static aurora_word_t shell_history_slot(aurora_word_t n) {
  /* n is zero-based, newest first; next points at the next write slot. */
  return ((history_next + SHELL_HISTORY_LIMIT - 1u - n) % SHELL_HISTORY_LIMIT) * 64u;
}
static int shell_expand_history(void) {
  if (shell_line[0] != '!') return 1;
  aurora_word_t n = 0u;
  if (shell_line[1] == '!' && !shell_line[2]) n = 1u;
  else if (shell_line[1] >= '1' && shell_line[1] <= '9' && !shell_line[2]) n = shell_line[1] - '0';
  else if (shell_line[1] == '1' && shell_line[2] == '0' && !shell_line[3]) n = 10u;
  if (!n || n > history_count) { kernel_boot_put_string("No such history item.\n"); return 0; }
  aurora_word_t offset = shell_history_slot(n - 1u);
  for (aurora_word_t i = 0; i < 64u; ++i) shell_line[i] = shell_history[offset + i];
  shell_put_words(shell_line); console_putchar('\n'); return 1;
}
static void shell_record_history(void) {
  for (aurora_word_t i = 0; i < 64u; ++i) shell_history[history_next * 64u + i] = shell_line[i];
  history_next = (history_next + 1u) % SHELL_HISTORY_LIMIT;
  if (history_count < SHELL_HISTORY_LIMIT) ++history_count;
}
static void shell_file_info(const kernel_file *file) {
  kernel_boot_put_string("Type:          ");
  kernel_boot_put_string(file->type==1u?"EXE\n":file->type==2u?"TXT\n":"DIR\n");
  kernel_boot_put_string("EEPROM offset: "); kernel_console_put_hex(file->offset);
  kernel_boot_put_string("\nFile words:    "); kernel_console_put_uint(file->words);
  kernel_boot_put_string("\nChecksum:      "); kernel_console_put_hex(file->checksum);
  kernel_boot_put_string("\nCapacity:      "); kernel_console_put_uint(file->capacity);
  if(file->type==1u) {
  kernel_boot_put_string("\nEntry:         "); kernel_console_put_hex(fs_read(file, 4u));
  kernel_boot_put_string("\nABI:           "); kernel_console_put_uint(fs_read(file, 5u));
  kernel_boot_put_string("\nSegments:      "); kernel_console_put_uint(fs_read(file, 7u));
  }
  kernel_boot_put_string("\nUse verify to validate these file fields.\n");
}
static void shell_verify_all(void) {
  aurora_word_t passed = 0u, checked=0u;
  for (aurora_word_t i = 0; i < fs_count; ++i) {
    aurora_word_t at = fs_directory + i * fs_entry_words;
    kernel_file file;
    if(!fs_get(i+1u,&file) || file.type==3u) continue;
    ++checked;
    for (aurora_word_t j = 0; j < 32u; ++j) {
      aurora_word_t c = storage_read(at + 4u + j); if (!c) break; console_putchar(c);
    }
    console_putchar('\n');
    if (file.type==1u?loader_load(&file,0u)!=0u:fs_checksum(&file)) { ++passed; kernel_boot_put_string("OK\n"); }
    else kernel_boot_put_string("INVALID\n");
  }
  kernel_console_put_uint(passed); console_putchar('/'); kernel_console_put_uint(checked);
  kernel_boot_put_string(" files verified.\n");
}
static void shell_run_file(const aurora_word_t *name, const kernel_file *file,aurora_word_t quick) {
  if(process_table[PROCESS_USER].valid) {
    kernel_boot_put_string("User slot busy. Use ps or kill; cannot run a second program.\n"); return;
  }
  if(file->type!=1u) { kernel_boot_put_string("Not an executable. Use cat or txt-viewer.\n"); return; }
  if(quick) kernel_boot_put_string("Quickrun: checksum comparison skipped.\n");
  kernel_boot_put_string("Loading "); shell_put_words(name); console_putchar('\n');
  aurora_word_t start = aurora.Read(AURORA_TIMER_DEVICE);
  aurora_word_t entry = loader_load(file, quick?2u:1u);
  if (!entry) {
    kernel_boot_put_string("Load failed: format/ABI/checksum/range or UROM write verification.\n"); return;
  }
  kernel_boot_put_string("Ready in "); kernel_console_put_uint(aurora.Read(AURORA_TIMER_DEVICE) - start);
  kernel_boot_put_string(" ms. Starting program.\n");
  /* Publish only with IE clear, return all shell frames before scheduling. */
  aurora.DisableInterrupt();
  user_console=shell_console;
  input_swallow_lf[user_console]=shell_skip_lf;
  initialize_user_process(entry); shell_needs_prompt = 0u;
}
/* Keep the command dispatcher out of the boot/idle function. The bundled
 * backend's high-pressure optimized dispatcher can corrupt live path pointers
 * across fs_open calls; this cold path favors explicit, predictable frames. */
__attribute__((noinline,optnone)) static void shell_execute(void) {
  if (!shell_expand_history()) return;
  aurora_word_t *name = shell_line;
  while (*name == ' ') ++name;
  aurora_word_t length = 0u;
  while (name[length]) ++length;
  while (length && name[length - 1u] == ' ') name[--length] = 0;
  if (!length) return;
  if (!shell_equal(name, "history")) shell_record_history();
  aurora_word_t *arg = name;
  while (*arg && *arg != ' ') ++arg;
  if (*arg) *arg++ = 0;
  while (*arg == ' ') ++arg;
  if (shell_equal(name, "help")) { shell_help(arg); return; }
  if (shell_equal(name, "echo")) { shell_put_words(arg); console_putchar('\n'); return; }
  if(shell_equal(name,"kill")) {
    if(*arg && !shell_equal(arg,"1") && !shell_equal(arg,"user")) {
      kernel_boot_put_string("Usage: kill [1|user]\n"); return;
    }
    aurora.DisableInterrupt();
    if(!process_table[PROCESS_USER].valid) {
      aurora.EnableInterrupt(); kernel_boot_put_string("No user program running.\n"); return;
    }
    aurora_word_t caller=shell_console;
    shell_console=user_console;
    finish_user_exit(137u);
    kernel_boot_put_string("Terminated by system console (kill).\n");
    shell_console=caller;
    aurora.EnableInterrupt();
    kernel_boot_put_string("User process terminated.\n"); return;
  }
  if(shell_equal(name,"ls") || shell_equal(name,"programs")) {
    kernel_file dir;
    if(!fs_open(arg,&dir) || dir.type!=3u) kernel_boot_put_string("Directory not found.\n");
    else fs_list(dir.id);
    return;
  }
  if(shell_equal(name,"mkdir") || shell_equal(name,"touch") || shell_equal(name,"rm") || shell_equal(name,"rename") || shell_equal(name,"mv")) {
    if(!*arg) { kernel_boot_put_string("Missing path.\n"); return; }
    aurora_status_t result; kernel_file file;
    if(shell_equal(name,"mkdir") || shell_equal(name,"touch")) result=fs_create(arg,shell_equal(name,"mkdir")?3u:2u);
    else {
      aurora_word_t *newname=arg; while(*newname && *newname!=' ') ++newname;
      if(*newname) *newname++=0u;
      while(*newname==' ') ++newname;
      if(!fs_open(arg,&file)) result=AURORA_ENOENT;
      else if(shell_equal(name,"rm")) result=*newname?AURORA_EINVAL:fs_remove(&file);
      else if(shell_equal(name,"mv")) result=*newname?fs_move(&file,newname):AURORA_EINVAL;
      else result=fs_rename(&file,newname);
    }
    if(result<0) { kernel_boot_put_string("File operation failed: "); kernel_console_put_int(result); console_putchar('\n'); }
    else kernel_boot_put_string("Done.\n");
    return;
  }
  if (shell_equal(name, "run") || shell_equal(name,"quickrun") || shell_equal(name,"cat") || shell_equal(name, "stat") || shell_equal(name, "verify")) {
    if (!*arg) { kernel_boot_put_string("Missing file name. Type ls.\n"); return; }
    if (shell_equal(name, "verify") && shell_equal(arg, "all")) { shell_verify_all(); return; }
    kernel_file file;
    if (!fs_open(arg, &file)) { kernel_boot_put_string("Program not found. Type ls.\n"); return; }
    if (shell_equal(name, "stat")) { shell_file_info(&file); return; }
    if (shell_equal(name, "run") || shell_equal(name,"quickrun")) { shell_run_file(arg,&file,shell_equal(name,"quickrun")); return; }
    if(shell_equal(name,"cat")) {
      if(file.type!=2u) { kernel_boot_put_string("Not a text file.\n"); return; }
      if(!fs_checksum(&file)) { kernel_boot_put_string("Text checksum mismatch.\n"); return; }
      for(aurora_word_t i=0;i<file.words;++i) { aurora_word_t c=fs_read(&file,i);
        console_putchar((c==9u || c==10u || c==13u || (c>=32u && c<=126u))?c:'?'); }
      console_putchar('\n'); return;
    }
    kernel_boot_put_string((file.type==1u?loader_load(&file,0u)!=0u:fs_checksum(&file)) ?
        "File verified. UROM unchanged.\n" : "Invalid file.\n"); return;
  }
  if (*arg) { kernel_boot_put_string("Unexpected arguments. Type help.\n"); return; }
  if (shell_equal(name,"tree")) { fs_tree();
  } else if (shell_equal(name, "history")) {
    for (aurora_word_t i = 0; i < history_count; ++i) {
      kernel_console_put_uint(i + 1u); kernel_boot_put_string("  ");
      shell_put_words(&shell_history[shell_history_slot(i)]); console_putchar('\n');
    }
  } else if (shell_equal(name, "clear")) {
    console_putchar('\f'); shell_needs_banner = 1u;
  } else if (shell_equal(name, "mount")) {
    kernel_boot_put_string(fs_mount() ? "Storage mounted.\n" : "Invalid storage image.\n");
  } else if (shell_equal(name, "selftest")) { kernel_self_test();
  } else if (shell_equal(name, "uptime")) {
    kernel_console_put_uint(aurora.Read(AURORA_TIMER_DEVICE));
    kernel_boot_put_string(" ms since reset (modulo 2^32).\n");
  } else if (shell_equal(name, "about") || shell_equal(name, "version")) {
    kernel_boot_put_string("Aurora-32 System Console 1.0\n32-bit word-addressed CPU | ABI 3\n"
      "ASFS 1/2 / AEXE 1 | INT gateway 0x00000002\n"
      "Preemptive tick; queued event callbacks.\n"
      "One resident kernel and one user slot.\n");
  } else if (shell_equal(name, "df")) {
    kernel_boot_put_string("Device words:  "); kernel_console_put_uint(STORAGE_WORDS);
    kernel_boot_put_string(" (64 MiB)\n");
    if (!fs_words) kernel_boot_put_string("No valid image mounted; usage unknown.\n");
    else {
      kernel_boot_put_string("Directory slots: "); kernel_console_put_uint(fs_count);
      kernel_boot_put_string("\nImage words:   "); kernel_console_put_uint(fs_words);
      kernel_boot_put_string("\nOutside image: "); kernel_console_put_uint(STORAGE_WORDS - fs_words);
      kernel_boot_put_string("\nData starts:   "); kernel_console_put_uint(fs_data);
      kernel_boot_put_string("\nAllocated:     "); kernel_console_put_uint(fs_allocated());
      kernel_boot_put_string("\nFree words:    "); kernel_console_put_uint(STORAGE_WORDS-fs_allocated());
      console_putchar('\n');
    }
    if(fs_words) kernel_boot_put_string(fs_version==2u?"Text writable; freed extents reused.\n":"Legacy ASFS1: read-only.\n");
  } else if (shell_equal(name, "ps")) {
    kernel_boot_put_string("SYSTEM  RUNNING  local + remote consoles\nUSER 1  ");
    if(process_table[PROCESS_USER].valid) kernel_boot_put_string(user_console?"RUNNING  remote terminal\n":"RUNNING  local terminal\n");
    else kernel_boot_put_string("EMPTY    ready for run\n");
    kernel_boot_put_string("Last exit: ");
    kernel_console_put_int(user_exit_code); kernel_boot_put_string("\nLast fault: ");
    kernel_console_put_hex(user_fault_vector); console_putchar('\n');
  } else if (shell_equal(name, "mem")) {
    kernel_boot_put_string("WORD ADDRESS RANGES (not installed sizes)\n"
      "ROM    00000000-00FFFFFF  kernel code\nRAM    01000000-01FFFFFF  kernel data\n"
      "KSTACK 02000000-02FFFFFF  banked stacks\nUROM   80000000-80FFFFFF  loaded code\n"
      "URAM   81000000-81FFFFFF  user data\nUSTACK 82000000-82FFFFFF  user stack\n"
      "No MMU or dynamic memory allocator.\n");
  } else if (shell_equal(name, "devices")) {
    kernel_boot_put_string("Keyboard  04000000  IRQ 03000001\nTerminal  05000000  write\n"
      "Telnet    06000000  IRQ 03000002\nTick      07000000  IRQ 03000007\n"
      "Timer     08000000  read milliseconds\nGraphics  09000000-09000009\n"
      "MIDI      0A000000-0A00007F\nStorage   0B000000-0BFFFFFF  ASFS\n"
      "LDMA      0C000000-0C000004 (SUM)\n");
  } else if (shell_equal(name, "netstat")) {
    /* Snapshot only; leave interrupts enabled during the potentially long print. */
    aurora.DisableInterrupt();
    aurora_word_t rx=telnet_rx_bytes, tx=telnet_tx_bytes;
    aurora_word_t rxms=telnet_rx_ms, txms=telnet_tx_ms;
    aurora_word_t rseen=telnet_rx_seen, tseen=telnet_tx_seen;
    aurora_word_t queued=(telnet_write+TELNET_QUEUE_SIZE-telnet_read)%TELNET_QUEUE_SIZE;
    aurora_word_t drops=telnet_drops, keys=keyboard_drops;
    aurora.EnableInterrupt();
    kernel_boot_put_string("Telnet: device 0x06000000\nTraffic since reset (bytes, includes CR/LF):\nRX: ");
    kernel_console_put_uint(rx); kernel_boot_put_string("  TX: "); kernel_console_put_uint(tx);
    kernel_boot_put_string("\nLast RX: ");
    if(rseen) { kernel_console_put_uint(rxms); kernel_boot_put_string(" ms since reset"); }
    else kernel_boot_put_string("never");
    kernel_boot_put_string("\nLast TX: ");
    if(tseen) { kernel_console_put_uint(txms); kernel_boot_put_string(" ms since reset"); }
    else kernel_boot_put_string("never");
    kernel_boot_put_string("\nRX queue pending: "); kernel_console_put_uint(queued);
    kernel_boot_put_string("\nRX queue drops: "); kernel_console_put_uint(drops);
    kernel_boot_put_string("\nKeyboard queue drops: "); kernel_console_put_uint(keys);
    kernel_boot_put_string("\nCounters wrap modulo 2^32; TX means device writes.");
    kernel_boot_put_string("\nNo connection/delivery status available.\n");
  } else if (shell_equal(name, "halt")) {
    kernel_boot_status("System halted by operator."); kernel_shutdown();
  } else {
    kernel_file file;
    if (!fs_open(name, &file)) { kernel_boot_put_string("Program not found. Type ls or help.\n"); return; }
    shell_run_file(name, &file,0u);
  }
}
static void shell_service_one(void) {
  if(process_table[PROCESS_USER].valid && user_console==shell_console) return;
  if (shell_needs_prompt) {
    shell_length = shell_overflow = 0u; shell_line[0] = 0u;
    shell_needs_prompt = 0u;
    if (shell_needs_banner) { shell_needs_banner = 0u; shell_banner(); }
    shell_prompt();
  }
  aurora_word_t c;
  aurora.DisableInterrupt();
  int got=0;
  if(shell_console) {
    if(telnet_read!=telnet_write) {
      c=telnet_queue[telnet_read]; telnet_read=(telnet_read+1u)%TELNET_QUEUE_SIZE; got=1;
    }
  } else got=dequeue_keyboard_word(&c);
  aurora.EnableInterrupt();
  if (!got) return;
  if (c == '\n' && shell_skip_lf) { shell_skip_lf = 0u; return; }
  shell_skip_lf = c == '\r';
  if (c == '\r' || c == '\n') {
    console_putchar('\n'); shell_line[shell_length] = 0;
    shell_needs_prompt = 1u;
    if (shell_overflow) kernel_boot_put_string("Command too long.\n");
    else {
      /* SYSTEM operations may be preempted by user FS syscalls. Reserve the
       * shared metadata workspace until the entire command has completed. */
      shell_fs_busy=1u; shell_execute(); shell_fs_busy=0u;
    }
  } else if (c == 27u || c == 21u) {
    shell_length = shell_overflow = 0u; shell_line[0] = 0u;
    kernel_boot_put_string("\nCommand cancelled.\n"); shell_prompt();
  } else if (c == 8u || c == 127u) {
    if (shell_length && !shell_overflow) { --shell_length; kernel_boot_put_string("\b \b"); }
  } else if (c >= 32u && c <= 126u) {
    if (shell_length < 63u && !shell_overflow) { shell_line[shell_length++] = c; console_putchar(c); }
    else shell_overflow = 1u;
  }
}
static void shell_service(void) {
  shell_console=0u; shell_service_one();
  shell_console=1u; shell_service_one();
}
#undef shell_line
#undef shell_history
#undef shell_length
#undef shell_overflow
#undef shell_skip_lf
#undef history_count
#undef history_next
#undef shell_needs_prompt
#undef shell_needs_banner
#endif
