#ifndef AURORA_BUFFER_H
#define AURORA_BUFFER_H

/* Internal implementation included by aurora_system.h after scalar I/O.
 * Use <aurora_system.h> in applications. */

/* Keep instances static/global: the kernel does not read UStack buffers.
 * A chunk is 64 hardware words, not 64 packed host bytes. */
typedef struct aurora_io_buffer {
  aurora_word_t words[AURORA_CONSOLE_WRITE_MAX];
  volatile aurora_word_t used;
  volatile aurora_status_t error;
  /* Preserve the first failure instead of collapsing every mismatch to -1.
   * Stages: 1=local bounds, 2=Write return, 3=count changed across Write,
   * 4=append count readback mismatch. */
  volatile aurora_word_t failed_stage;
  volatile aurora_word_t batch;
  volatile aurora_word_t requested;
  volatile aurora_status_t returned;
  volatile aurora_word_t after;
  volatile aurora_word_t last_before, last_expected, last_observed, last_character;
} aurora_io_buffer;

typedef struct aurora_system_buffer_api {
  void (*Begin)(aurora_io_buffer *out);
  void (*Char)(aurora_io_buffer *out, aurora_word_t c);
  void (*String)(aurora_io_buffer *out, const char *text);
  void (*Unsigned)(aurora_io_buffer *out, aurora_word_t value);
  void (*Flush)(aurora_io_buffer *out);
  void (*ReportError)(const aurora_io_buffer *out);
} aurora_system_buffer_api;

static void __aurora_buffer_begin(aurora_io_buffer *out) {
  out->used = 0u;
  out->error = 0;
  out->failed_stage = 0u;
  out->batch = 0u;
  out->requested = 0u;
  out->returned = 0;
  out->after = 0u;
  out->last_before = out->last_expected = out->last_observed = 0u;
  out->last_character = 0u;
}

/* Keep one small call site/check rather than cloning this register-sensitive
 * path into every drawing loop. This does not disable interrupts. */
__attribute__((noinline)) static void __aurora_buffer_flush(aurora_io_buffer *out) {
  if (out->error != 0)
    return;
  aurora_word_t count = out->used;
  if (count == 0u)
    return;
  out->requested = count;
  out->after = count;
  if (count > AURORA_CONSOLE_WRITE_MAX) {
    out->returned = 0;
    out->failed_stage = 1u;
    out->error = AURORA_EINVAL;
    return;
  }
  ++out->batch;
  aurora_status_t result = __aurora_system_write(out->words, count);
  out->returned = result;
  out->after = out->used;
  if (result != (aurora_status_t)out->requested) {
    out->failed_stage = 2u;
    out->error = result < 0 ? result : AURORA_EINVAL;
    return;
  }
  if (out->after != out->requested) {
    out->failed_stage = 3u;
    out->error = AURORA_EINVAL;
    return;
  }
  out->used = 0u;
}

static void __aurora_buffer_char(aurora_io_buffer *out, aurora_word_t c) {
  if (out->error != 0)
    return;
  aurora_word_t count = out->used;
  if (count >= AURORA_CONSOLE_WRITE_MAX) {
    __aurora_buffer_flush(out);
    if (out->error != 0)
      return;
    count = out->used;
    /* Do not use an unchecked fresh volatile value as an array index. */
    if (count >= AURORA_CONSOLE_WRITE_MAX) {
      out->requested = out->after = count;
      out->failed_stage = 1u;
      out->error = AURORA_EINVAL;
      return;
    }
  }
  out->last_before = count;
  out->last_character = c;
  out->last_expected = count + 1u;
  out->words[count] = c;
  out->used = count + 1u;
  out->last_observed = out->used;
  if (out->last_observed != out->last_expected) {
    out->requested = out->last_expected;
    out->after = out->last_observed;
    out->failed_stage = 4u;
    out->error = AURORA_EINVAL;
    return;
  }
  if (count + 1u == AURORA_CONSOLE_WRITE_MAX)
    __aurora_buffer_flush(out);
}

/* Use the old scalar console API here, never the failed buffered writer.
 * Address is printed in signed decimal; reinterpret its 32-bit bit pattern
 * to obtain the CPU word address. This is a failure-only diagnostic. */
__attribute__((noinline)) static void
__aurora_buffer_report_error(const aurora_io_buffer *out) {
  __aurora_system_put_string("\nConsole Write failed: stage=");
  __aurora_system_put_int((int)out->failed_stage);
  __aurora_system_put_string(" batch=");
  __aurora_system_put_int((int)out->batch);
  __aurora_system_put_string(" requested=");
  __aurora_system_put_int((int)out->requested);
  __aurora_system_put_string(" returned=");
  __aurora_system_put_int(out->returned);
  __aurora_system_put_string(" after=");
  __aurora_system_put_int((int)out->after);
  __aurora_system_put_string(" error=");
  __aurora_system_put_int(out->error);
  __aurora_system_put_string(" address=");
  __aurora_system_put_int((int)(__UINTPTR_TYPE__)out->words);
  __aurora_system_put_char('\n');
  __aurora_system_put_string("Append: before=");
  __aurora_system_put_int((int)out->last_before);
  __aurora_system_put_string(" expected=");
  __aurora_system_put_int((int)out->last_expected);
  __aurora_system_put_string(" observed=");
  __aurora_system_put_int((int)out->last_observed);
  __aurora_system_put_string(" char=");
  __aurora_system_put_int((int)out->last_character);
  __aurora_system_put_char('\n');
}

static void __aurora_buffer_string(aurora_io_buffer *out, const char *text) {
  while (*text)
    __aurora_buffer_char(out, (aurora_word_t)*text++);
}

static void __aurora_buffer_unsigned(aurora_io_buffer *out, aurora_word_t n) {
  aurora_word_t digits[10];
  unsigned int count = 0;
  do {
    digits[count++] = n % 10u;
    n /= 10u;
  } while (n != 0u);
  while (count != 0u)
    __aurora_buffer_char(out, '0' + digits[--count]);
}

#endif
