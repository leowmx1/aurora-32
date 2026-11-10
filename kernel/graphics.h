#ifndef AURORA_KERNEL_GRAPHICS_H
#define AURORA_KERNEL_GRAPHICS_H

enum {
  AURORA_GRAPHICS_X_DEVICE = 0x09000000u,
  AURORA_GRAPHICS_Y_DEVICE = 0x09100000u,
  AURORA_GRAPHICS_W_DEVICE = 0x09200000u,
  AURORA_GRAPHICS_H_DEVICE = 0x09300000u,
  AURORA_GRAPHICS_COLOR_DEVICE = 0x09400000u,
  AURORA_GRAPHICS_BG_DEVICE = 0x09500000u,
  AURORA_GRAPHICS_DATA_DEVICE = 0x09600000u,
  AURORA_GRAPHICS_MODE_DEVICE = 0x09800000u,
  AURORA_GRAPHICS_STATUS_DEVICE = 0x09900000u,
};

/* INT has disabled IRQs, so idle-check/configure/start is atomic with respect
 * to other syscalls and callbacks. Never poll for completion here: the engine
 * runs independently after IRET, with tick and input IRQs enabled again.
 * Packed scalar arguments avoid dereferencing a user stack descriptor. */
__attribute__((noinline, optnone)) static aurora_status_t
kernel_graphics_fill_rect(aurora_word_t position, aurora_word_t size,
                          aurora_word_t color) {
  if(position>0x00efffffu || size>0x00f0ffffu) return AURORA_EINVAL;
  aurora_word_t x = position, y = position;
  aurora_word_t width = size, height = size;
  /* Avoid backend halfword-load legalization for constant shifts by 16:
   * all values surviving the bounds above are positive 32-bit integers. */
  aurora_word_t divisor=65536u;
  __asm__ volatile("div %0, %1, %0" : "+r"(y) : "r"(divisor));
  __asm__ volatile("div %0, %1, %0" : "+r"(height) : "r"(divisor));
  /* Like aurora_event_character: the bundled backend can drop a constant
   * low-byte mask during narrow-type legalization. Keep both ANDs explicit. */
  aurora_word_t mask = 65535u;
  __asm__ volatile("and %0, %1, %0" : "+r"(x) : "r"(mask));
  __asm__ volatile("and %0, %1, %0" : "+r"(width) : "r"(mask));
  if (x >= AURORA_GRAPHICS_WIDTH || y >= AURORA_GRAPHICS_HEIGHT)
    return AURORA_EINVAL;
  if (width == 0u || height == 0u ||
      width > AURORA_GRAPHICS_WIDTH - x ||
      height > AURORA_GRAPHICS_HEIGHT - y) return AURORA_EINVAL;
  if (aurora.Read(AURORA_GRAPHICS_STATUS_DEVICE) != 0u)
    return AURORA_EBUSY;
  /* In particular, do NOT stop a busy operation to replace its parameters. */
  aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE, 0u);
  aurora.Write(AURORA_GRAPHICS_X_DEVICE, x);
  aurora.Write(AURORA_GRAPHICS_Y_DEVICE, y);
  aurora.Write(AURORA_GRAPHICS_W_DEVICE, width);
  aurora.Write(AURORA_GRAPHICS_H_DEVICE, height);
  aurora.Write(AURORA_GRAPHICS_COLOR_DEVICE, color);
  /* A preceding bitmap may have left mode=1/2. */
  aurora.Write(AURORA_GRAPHICS_MODE_DEVICE, 0u);
  aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE, 1u);
  return AURORA_OK;
}

/* Retain the old 8-bit packed syscall for existing stored ABI-3 executables. */
static aurora_status_t kernel_graphics_legacy_rect(aurora_word_t position,
    aurora_word_t size, aurora_word_t color) {
  if(position>65535u || size>65535u) return AURORA_EINVAL;
  aurora_word_t x=position,w=size,mask=255u;
  __asm__ volatile("and %0, %1, %0" : "+r"(x) : "r"(mask));
  __asm__ volatile("and %0, %1, %0" : "+r"(w) : "r"(mask));
  return kernel_graphics_fill_rect(x+(position/256u)*65536u,
                                   w+(size/256u)*65536u,color);
}

/* Fixed eight-word UROM/URAM packet. No USP access or retained user pointer.
 * INT keeps interrupts disabled throughout validation, snapshot and submit. */
__attribute__((noinline, optnone)) static aurora_status_t
kernel_graphics_bitmap(aurora_word_t address, aurora_word_t mode,
                       aurora_word_t reserved, aurora_word_t memory) {
  if ((mode != 1u && mode != 2u) || reserved != 0u) return AURORA_EINVAL;
  if (!((address >= AURORA_UROM_BEGIN && address <= AURORA_UROM_END - 8u) ||
        (address >= AURORA_URAM_BEGIN && address <= AURORA_URAM_END - 8u)))
    return AURORA_EFAULT;
  aurora_word_t packet[8];
  address=user_physical(address);
  for (aurora_word_t i = 0u; i < 8u; ++i)
    packet[i] = aurora.Read(address + i);
  aurora_word_t x = packet[0], y = packet[1];
  aurora_word_t w = packet[2], h = packet[3];
  if (x >= AURORA_GRAPHICS_WIDTH || y >= AURORA_GRAPHICS_HEIGHT)
    return AURORA_EINVAL;
  if (w == 0u || h == 0u || w > AURORA_GRAPHICS_WIDTH - x ||
      h > AURORA_GRAPHICS_HEIGHT - y) return AURORA_EINVAL;
  /* Dimensions are now bounded, so the product cannot overflow. */
  aurora_word_t words=(w*h+31u)/32u;
  aurora_word_t data=packet[6];
  if (memory) {
    if (packet[7]!=words) return AURORA_EINVAL;
    if (!((data>=AURORA_UROM_BEGIN && data<=AURORA_UROM_END-words) ||
          (data>=AURORA_URAM_BEGIN && data<=AURORA_URAM_END-words))) return AURORA_EFAULT;
    data=user_physical(data);
  } else {
    if (w*h>64u) return AURORA_EINVAL;
    words=2u; /* Legacy descriptor always supplies two inline words. */
  }
  if (aurora.Read(AURORA_GRAPHICS_STATUS_DEVICE) != 0u) return AURORA_EBUSY;
  aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE, 0u);
  for (aurora_word_t i = 0u; i < 6u; ++i)
    aurora.Write(AURORA_GRAPHICS_X_DEVICE + (i<<20u), packet[i]);
  /* IE remains clear: user pointers cannot change bank or contents mid-upload.
   * No data RAM write until the engine is idle and all bounds are validated. */
  for (aurora_word_t i=0u;i<words;++i)
    aurora.Write(AURORA_GRAPHICS_DATA_DEVICE+i,
                 memory?aurora.Read(data+i):packet[6u+i]);
  aurora.Write(AURORA_GRAPHICS_MODE_DEVICE, mode);
  aurora.Write(AURORA_GRAPHICS_STATUS_DEVICE, 1u);
  return AURORA_OK;
}

#endif
