#include <aurora.h>
AURORA_KERNEL_PROGRAM;

/* Standalone ROM; never execute the UROM contents under test. */
__asm__(".globl __aurora_rom_entry\n"
        "__aurora_rom_entry:\n"
        "  jmp _start\n");

#define STORAGE 0x0b000000u
#define UROM 0x80000000u
#define DMA 0x0c000000u
#define STAT (DMA+3u)
#define SUM (DMA+4u)
#define TIMER 0x08000000u
#define GUARD 0x39c5a76eu
static volatile aurora_u32 expected[4096] __attribute__((section(".noinit")));
static const aurora_u32 cases[5][3] = {
  {0u,0u,1u}, {1u,63u,2u}, {31u,255u,17u},
  {251u,4093u,257u}, {4096u,8191u,4096u}
};

__attribute__((noreturn)) static void idle(void) {
  /* Keep clocks running even after a timeout. Reset manually before retrying. */
  for (;;) aurora.Nop();
}
static void hex(aurora_u32 value) {
  aurora.io.PutString("0x");
  for (aurora_u32 divisor=0x10000000u; divisor; divisor/=16u) {
    aurora_u32 digit=value/divisor%16u;
    aurora.io.PutChar(digit<10u?'0'+digit:'A'+digit-10u);
  }
}
__attribute__((noreturn)) static void mismatch(aurora_u32 address,
                                             aurora_u32 want, aurora_u32 got) {
  aurora.io.PutString("FAIL address="); hex(address);
  aurora.io.PutString(" expected="); hex(want);
  aurora.io.PutString(" actual="); hex(got);
  aurora.io.PutChar('\n'); idle();
}
__attribute__((noinline,optnone)) static void run_case(aurora_u32 index) {
  aurora_u32 origin=cases[index][0], target=cases[index][1], len=cases[index][2];
  aurora_u32 expected_sum=0u;
  aurora.io.PutString("Test "); aurora.io.PutInt((int)(index+1u));
  aurora.io.PutString(": origin="); aurora.io.PutInt((int)origin);
  aurora.io.PutString(" target="); aurora.io.PutInt((int)target);
  aurora.io.PutString(" len="); aurora.io.PutInt((int)len);
  aurora.io.PutChar('\n');
  if (aurora.Read(STAT)!=0u) {
    aurora.io.PutString("FAIL DMA already busy. Reset before retry.\n"); idle();
  }
  /* Storage is read-only in this test. Poison every destination word, so an
   * idle/no-op DMA cannot pass even when Storage contains all zeroes. */
  for (aurora_u32 i=0u;i<len;++i) {
    expected[i]=aurora.Read(STORAGE+origin+i);
    expected_sum+=expected[i];
    aurora.Write(UROM+target+i,~expected[i]);
  }
  if (target) aurora.Write(UROM+target-1u,GUARD);
  aurora.Write(UROM+target+len,GUARD);
  /* Verify CPU UROM writes before attributing any later error to DMA. */
  for (aurora_u32 i=0u;i<len;++i) {
    aurora_u32 got=aurora.Read(UROM+target+i);
    if (got!=~expected[i]) mismatch(UROM+target+i,~expected[i],got);
  }
  aurora.Write(DMA,origin);       /* Internal WORD offsets, no segment bits. */
  aurora.Write(DMA+1u,target);
  aurora.Write(DMA+2u,len);       /* Word count, NOT last index or byte count. */
  aurora.Write(SUM,0u);          /* CS_DMAsum + WE explicitly clears the sum. */
  if(aurora.Read(SUM)!=0u) mismatch(SUM,0u,aurora.Read(SUM));
  aurora_u32 start=aurora.Read(TIMER), polls=0u;
  aurora.Write(STAT,1u);
  while (aurora.Read(STAT)!=0u) {
    ++polls;
    if (aurora.Read(TIMER)-start>=30000u || polls>=1000000u) {
      aurora.io.PutString("FAIL DMA TIMEOUT. Reset before retry.\n"); idle();
    }
  }
  aurora_u32 elapsed=aurora.Read(TIMER)-start;
  aurora_u32 copied_sum=aurora.Read(SUM);
  if(copied_sum!=expected_sum) mismatch(SUM,expected_sum,copied_sum);
  for (aurora_u32 i=0u;i<len;++i) {
    aurora_u32 got=aurora.Read(UROM+target+i);
    if (got!=expected[i]) mismatch(UROM+target+i,expected[i],got);
    got=aurora.Read(STORAGE+origin+i);
    if (got!=expected[i]) mismatch(STORAGE+origin+i,expected[i],got);
  }
  if (target && aurora.Read(UROM+target-1u)!=GUARD)
    mismatch(UROM+target-1u,GUARD,aurora.Read(UROM+target-1u));
  if (aurora.Read(UROM+target+len)!=GUARD)
    mismatch(UROM+target+len,GUARD,aurora.Read(UROM+target+len));
  aurora.io.PutString("PASS copy+guards+sum, DMA elapsed ");
  aurora.io.PutInt((int)elapsed); aurora.io.PutString(" ms, busy polls ");
  aurora.io.PutInt((int)polls); aurora.io.PutChar('\n');
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  aurora.DisableInterrupt();
  aurora.io.PutString("LDMA smoke test: Storage -> UROM, kernel only.\n"
                      "Storage is not modified; UROM test area is overwritten.\n");
  for (aurora_u32 i=0u;i<5u;++i) run_case(i);
  aurora.io.PutString("LDMA ALL PASS (5 tests).\n");
  idle();
}
