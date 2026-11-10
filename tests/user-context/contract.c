#include <aurora.h>
AURORA_KERNEL_PROGRAM;

/* Standalone ROM, no production kernel or UROM program required. */
__asm__(".globl __aurora_rom_entry\n"
        "__aurora_rom_entry:\n"
        "  jmp context_reset\n"
        "context_reset:\n"
        "  di\n"
        "  jmp _start\n");

/* Caller-saved r0..r3. No compiler arithmetic can change FLAGS between
 * ADD and the conditional branches. Leave the chosen USP/UFP unchanged. */
extern aurora_u32 context_flags_test(void);
__asm__(".globl context_flags_test\n"
        "context_flags_test:\n"
        "  ousp r2\n"
        "  oufp r3\n"
        "  ldi r0, 0x7fffffff\n"
        "  ldi r1, 1\n"
        "  add r0, r1, r0\n"
        "  lusp r2\n"
        "  lufp r3\n"
        "  ousp r2\n"
        "  oufp r3\n"
        "  jcc v, .Lcontext_v\n"
        "  jmp .Lcontext_bad_flags\n"
        ".Lcontext_v:\n"
        "  jcc n, .Lcontext_n\n"
        "  jmp .Lcontext_bad_flags\n"
        ".Lcontext_n:\n"
        "  jcc z, .Lcontext_bad_flags\n"
        "  jcc c, .Lcontext_bad_flags\n"
        "  ldi r0, 1\n"
        "  ret\n"
        ".Lcontext_bad_flags:\n"
        "  ldi r0, 0\n"
        "  ret\n");

struct user_pointers { aurora_u32 sp, fp; };
static volatile struct user_pointers saved[2];

static __inline__ __attribute__((always_inline)) aurora_u32 kernel_fp(void) {
  aurora_u32 result;
  __asm__ volatile("ofp %0" : "=r"(result));
  return result;
}

__attribute__((noreturn)) static void fail(int stage) {
  aurora.io.PutString("FAIL stage=");
  aurora.io.PutInt(stage);
  aurora.io.PutChar('\n');
  aurora.Halt();
}

__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  aurora.DisableInterrupt();
  aurora.io.PutString("USP/UFP smoke test: kernel only, IRQs off.\n");
  aurora_u32 original_sp = aurora.GetUserStackPointer();
  aurora_u32 original_fp = aurora.GetUserFramePointer();
  aurora_u32 ksp = aurora.GetStackPointer(), kfp = kernel_fp();

  aurora.SetUserStackPointer(0x82000120u);
  aurora.SetUserFramePointer(0x82000240u);
  saved[0].sp = aurora.GetUserStackPointer();
  saved[0].fp = aurora.GetUserFramePointer();
  if (saved[0].sp != 0x82000120u || saved[0].fp != 0x82000240u) fail(1);
  aurora.SetUserStackPointer(0x82000360u);
  aurora.SetUserFramePointer(0x82000480u);
  saved[1].sp = aurora.GetUserStackPointer();
  saved[1].fp = aurora.GetUserFramePointer();
  if (saved[1].sp != 0x82000360u || saved[1].fp != 0x82000480u) fail(2);

  /* Alternate two saved pointer pairs, without enabling process scheduling. */
  for (aurora_u32 i = 0; i < 32; ++i) {
    aurora_u32 index = i & 1u;
    aurora.SetUserStackPointer(saved[index].sp);
    aurora.SetUserFramePointer(saved[index].fp);
    if (aurora.GetUserStackPointer() != saved[index].sp ||
        aurora.GetUserFramePointer() != saved[index].fp) fail(3);
    if (aurora.GetStackPointer() != ksp || kernel_fp() != kfp) fail(4);
  }
  aurora.io.PutString("PASS pointer save/restore, KSP/KFP unchanged.\n");

  aurora.SetUserStackPointer(0x82000120u);
  aurora_u32 old_word = aurora.Read(0x82000120u), value = 0x13579bdfu;
  __asm__ volatile("pushu %0" : : "r"(value) : "memory");
  if (aurora.GetUserStackPointer() != 0x82000121u ||
      aurora.Read(0x82000120u) != value) fail(5);
  aurora_u32 popped;
  __asm__ volatile("popu %0" : "=r"(popped) : : "memory");
  if (popped != value || aurora.GetUserStackPointer() != 0x82000120u) fail(6);
  aurora.Write(0x82000120u, old_word);
  if (aurora.GetStackPointer() != ksp || kernel_fp() != kfp) fail(7);
  aurora.io.PutString("PASS PUSHU/POPU use the selected USP.\n");
  if (!context_flags_test()) fail(8);
  aurora.io.PutString("PASS FLAGS preserved (ADD overflow).\n");
  aurora.SetUserStackPointer(original_sp);
  aurora.SetUserFramePointer(original_fp);
  if (aurora.GetUserStackPointer() != original_sp ||
      aurora.GetUserFramePointer() != original_fp) fail(9);
  aurora.io.PutString("USER CONTEXT ALL PASS.\n");
  aurora.Halt();
}
