#ifndef AURORA_KERNEL_STORAGE_H
#define AURORA_KERNEL_STORAGE_H
/* Installed device: 2^24 32-bit words (64 MiB), kernel read/write.
 * Filesystem offsets never contain CPU addresses. */
enum { STORAGE_BASE = 0x0b000000u, STORAGE_WORDS = 0x01000000u };
static aurora_word_t storage_read(aurora_word_t offset) {
  return aurora.Read(STORAGE_BASE + offset);
}
static int storage_range(aurora_word_t offset, aurora_word_t words,
                         aurora_word_t limit) {
  return offset <= limit && words <= limit - offset;
}
#endif
