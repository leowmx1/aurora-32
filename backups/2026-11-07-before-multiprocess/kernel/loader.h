#ifndef AURORA_KERNEL_LOADER_H
#define AURORA_KERNEL_LOADER_H
#include "filesystem.h"
#include "ldma.h"
/* AEXE v1. Segment: destination, file offset, file words, memory words,
 * flags (1=code, 2=data), reserved. Validate completely before writing. */
static void loader_progress(const char *label, aurora_word_t part) {
  /* 24 columns, no ANSI or carriage-return dependency. Digital's existing
   * backspace handling supports overwriting this fixed-width line. */
  if (part) for (aurora_word_t i = 0; i < 24u; ++i) console_putchar('\b');
  kernel_boot_put_string(label); /* exactly seven columns */
  console_putchar('[');
  for (aurora_word_t i = 0; i < 10u; ++i) console_putchar(i < part ? '-' : ' ');
  kernel_boot_put_string("] ");
  if (part < 10u) console_putchar(' ');
  if (!part) console_putchar(' ');
  kernel_console_put_uint(part * 10u); console_putchar('%');
  if (part == 10u) console_putchar('\n');
}
/* Complete the whole-file checksum using non-DMA words (headers/data/gaps).
 * Source ranges may be unordered or shared. For shared source words subtract
 * the extra copies counted by DMA, without falling back to a full CPU scan.
 * Every destination copy still contributes its actual UROM readback SUM. */
__attribute__((noinline,optnone)) static aurora_word_t
loader_unmapped_sum(const kernel_file *file) {
  aurora_word_t pos=0u, sum=0u;
  aurora_word_t count=fs_read(file,7u), table=fs_read(file,8u);
  while(pos<file->words) {
    aurora_word_t end=file->words, copies=0u;
    for(aurora_word_t i=0u;i<count;++i) {
      aurora_word_t at=table+i*6u;
      if(fs_read(file,at+4u)!=1u) continue;
      aurora_word_t off=fs_read(file,at+1u), n=fs_read(file,at+2u);
      if(pos>=off && pos-off<n) {
        ++copies;
        if(off+n<end) end=off+n;
      }
      if(off>pos && off<end) end=off;
    }
    if(copies==1u) pos=end;
    else while(pos<end) {
      aurora_word_t value=fs_read(file,pos++);
      if(!copies) sum+=value;
      else sum-=value*(copies-1u);
    }
  }
  return sum;
}
__attribute__((noinline)) static aurora_word_t
loader_load(const kernel_file *file, aurora_word_t install) {
  /* install=0: non-destructive Storage verification; 2: quickrun skips comparison.
   * Hardware still reads back UROM for both run and quickrun. */
  if(file->type!=1u || file->words<16u ||
     !storage_range(file->offset,file->words,STORAGE_WORDS)) return 0u;
  if (fs_read(file, 0) != 0x41455845u || fs_read(file, 1) != 1u ||
      fs_read(file, 2) != 16u || fs_read(file, 3) != file->words ||
      fs_read(file, 5) != 3u || fs_read(file, 6) != 2u ||
      fs_read(file, 10) != 1u) return 0u;
  /* Flag 1: compiler startup initializes globals + banked user SP/FP.
   * .noinit/unused RAM and stack are intentionally not an isolation boundary. */
  aurora_word_t entry = fs_read(file, 4), count = fs_read(file, 7);
  aurora_word_t table = fs_read(file, 8), stride = fs_read(file, 9);
  if (!count || count > 8u || stride != 6u || table < 16u ||
      !storage_range(table, count * stride, file->words)) return 0u;
  for (aurora_word_t i = 11; i < 16; ++i) if (fs_read(file, i)) return 0u;
  aurora_word_t sum = 0u, entry_found = 0u;
  aurora_word_t dma_check = install == 1u;
  aurora_word_t pos = 0u;
  for (aurora_word_t i = 0; i < count; ++i) {
    aurora_word_t at = table + i * stride;
    aurora_word_t dest = fs_read(file, at), off = fs_read(file, at + 1u);
    aurora_word_t n = fs_read(file, at + 2u), mem = fs_read(file, at + 3u);
    aurora_word_t flags = fs_read(file, at + 4u);
    if (!mem || n > mem || fs_read(file, at + 5u) ||
        !storage_range(off, n, file->words) ||
        (n && off < table + count * stride)) return 0u;
    if (flags == 1u) {
      if (dest < AURORA_UROM_BEGIN || dest >= AURORA_UROM_END ||
          mem > AURORA_UROM_END - dest || n != mem) return 0u;
      if (entry >= dest && entry - dest < n) entry_found = 1u;
    } else if (flags == 2u) {
      if (dest < AURORA_URAM_BEGIN || dest >= AURORA_URAM_END ||
          mem > AURORA_URAM_END - dest) return 0u;
    } else return 0u;
    for (aurora_word_t j = 0; j < i; ++j) {
      aurora_word_t prev = table + j * stride;
      aurora_word_t begin = fs_read(file, prev), size = fs_read(file, prev + 3u);
      if (dest < begin + size && begin < dest + mem) return 0u;
    }
  }
  if (!entry_found) return 0u;
  if(!install) {
    /* Explicit verify NAME checks Storage only, without overwriting UROM. */
    loader_progress("Verify ",0u);
    for(aurora_word_t part=1u;part<=10u;++part) {
      aurora_word_t end=file->words*part/10u;
      while(pos<end) sum+=fs_read(file,pos++);
      loader_progress("Verify ",part);
    }
    if(sum!=file->checksum) return 0u;
  } else if(dma_check) sum=loader_unmapped_sum(file);
  if (!install) return entry;
  for (aurora_word_t i = 0; i < count; ++i) {
    aurora_word_t at = table + i * stride;
    aurora_word_t dest = fs_read(file, at), off = fs_read(file, at + 1u);
    aurora_word_t n = fs_read(file, at + 2u), mem = fs_read(file, at + 3u);
    loader_progress("Copy   ", 0u);
    pos = 0u;
    if (dest >= AURORA_UROM_BEGIN && dest < AURORA_UROM_END) {
      /* All segments were checked before the first DMA. Code has n == mem.
       * Submit internal 24-bit physical offsets, not CPU addresses.
       * This single-user stage loads bank zero (at most 2^22 words).
       * LDMA itself is not translated by the bank selector. */
      while (pos < n) {
        aurora_word_t chunk=n-pos;
        if(chunk>0x00ffffffu) chunk=0x00ffffffu;
        aurora_word_t copied_sum=ldma_copy(file->offset+off+pos,dest-AURORA_UROM_BEGIN+pos,chunk);
        if(dma_check) sum+=copied_sum;
        pos+=chunk;
      }
      /* No hardware progress counter: report completion, not invented %. */
      loader_progress("Copy   ",10u);
    } else {
    /* LDMA only targets UROM. Data segments and BSS remain CPU writes. */
    for (aurora_word_t part = 1; part <= 10u; ++part) {
      aurora_word_t end = mem * part / 10u;
      while (pos < end) {
        aurora.Write(dest + pos, pos < n ? fs_read(file, off + pos) : 0u);
        ++pos;
      }
      loader_progress("Copy   ", part);
    }
    }
  }
  if(dma_check) {
    loader_progress("Verify ",0u);
    if(sum!=file->checksum) {
      kernel_boot_put_string(" FAILED (LDMA UROM checksum)\nExpected: ");
      kernel_console_put_hex(file->checksum);
      kernel_boot_put_string(" Actual: "); kernel_console_put_hex(sum);
      console_putchar('\n'); return 0u;
    }
    loader_progress("Verify ",10u);
  }
  /* UROM is verified by hardware readback SUM. Only URAM data/BSS, which
   * LDMA cannot access, still needs CPU readback. */
  if(install==2u) return entry;
  for(aurora_word_t i=0u;i<count;++i) {
    aurora_word_t at=table+i*stride;
    if(fs_read(file,at+4u)==1u) continue;
    aurora_word_t dest=fs_read(file,at), off=fs_read(file,at+1u);
    aurora_word_t n=fs_read(file,at+2u), mem=fs_read(file,at+3u);
    loader_progress("Verify ", 0u);
    pos = 0u;
    for (aurora_word_t part = 1; part <= 10u; ++part) {
      aurora_word_t end = mem * part / 10u;
      while (pos < end) {
        if (aurora.Read(dest + pos) != (pos < n ? fs_read(file, off + pos) : 0u)) {
          kernel_boot_put_string(" FAILED\n"); return 0u;
        }
        ++pos;
      }
      loader_progress("Verify ", part);
    }
  }
  return entry;
}
#endif
