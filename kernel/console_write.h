#ifndef AURORA_KERNEL_CONSOLE_WRITE_H
#define AURORA_KERNEL_CONSOLE_WRITE_H

/* Internal kernel implementation. The including translation unit provides
 * aurora, the user-address validator and the shared syscall constants. */
static aurora_status_t kernel_console_write_words(aurora_word_t address,
                                                  aurora_word_t count) {
  for (aurora_word_t i = 0u; i < count; ++i)
    aurora.io.PutChar(aurora.Read(address + i) & 0xffu);
  return (aurora_status_t)count;
}

/* Validate the complete bounded span before producing any output. All
 * addresses here are CPU word addresses, not host byte addresses. */
__attribute__((noinline, optnone)) static aurora_status_t
kernel_console_write(aurora_word_t address, aurora_word_t count) {
  if (count > AURORA_CONSOLE_WRITE_MAX)
    return AURORA_EINVAL;
  if (count == 0u)
    return 0;
  if (!is_user_read_address(address))
    return AURORA_EFAULT;
  aurora_word_t end = address + count - 1u;
  if (end < address || !is_user_read_address(end))
    return AURORA_EFAULT;
  /* Do not cross from one user memory device to another. */
  if (address < AURORA_URAM_BEGIN && end >= AURORA_URAM_BEGIN)
    return AURORA_EFAULT;
  return kernel_console_write_words(address, count);
}

/* Telnet has the same bounded user-buffer contract as console Write. */
__attribute__((noinline, optnone)) static aurora_status_t
kernel_telnet_write(aurora_word_t address, aurora_word_t count) {
  if (count > AURORA_CONSOLE_WRITE_MAX) return AURORA_EINVAL;
  if (count == 0u) return 0;
  if (!is_user_read_address(address)) return AURORA_EFAULT;
  aurora_word_t end = address + count - 1u;
  if (end < address || !is_user_read_address(end)) return AURORA_EFAULT;
  if (address < AURORA_URAM_BEGIN && end >= AURORA_URAM_BEGIN) return AURORA_EFAULT;
  for (aurora_word_t i = 0u; i < count; ++i)
    aurora.Write(AURORA_TELNET_DEVICE, aurora.Read(address + i) & 0xffu);
  return (aurora_status_t)count;
}

#endif
