#ifndef __AURORA_H
#define __AURORA_H

#ifndef __aurora__
#error "aurora.h requires an Aurora target"
#endif

/*
 * Public Aurora C API.
 *
 * Aurora addresses are hardware word addresses.  Read and Write pass them to
 * the target unchanged; no byte/word address conversion is performed.
 */
typedef unsigned int aurora_u32;
typedef unsigned int aurora_word __attribute__((aligned(1)));
typedef void (*aurora_interrupt_handler)(void);

/*
 * Put exactly one of these declarations at file scope, before _start.
 * Aurora's backend reads this marker once for the whole translation unit and
 * consistently selects its code, global-memory and stack address regions.
 * A module without either marker remains kernel for compatibility.
 */
#define AURORA_KERNEL_PROGRAM                                             \
  __attribute__((used)) static const unsigned int __aurora_program_mode = 0u
#define AURORA_USER_PROGRAM                                               \
  __attribute__((used)) static const unsigned int __aurora_program_mode = 1u

#define aurora_interrupt(device)                                         \
  __attribute__((aurora_interrupt(device)))

typedef struct aurora_address_api {
  aurora_u32 rom;
  aurora_u32 ram;
  aurora_u32 stack;
  aurora_u32 ivt;
  aurora_u32 keyboard;
  aurora_u32 terminal;
  aurora_u32 telnet;
} aurora_address_api;

typedef struct aurora_interrupt_api {
  void (*Enable)(void);
  void (*Disable)(void);
  void (*Register)(aurora_u32, aurora_interrupt_handler);
} aurora_interrupt_api;

typedef struct aurora_stack_api {
  void (*SetPointer)(aurora_u32);
  aurora_u32 (*GetPointer)(void);
  void (*Push)(aurora_u32);
  aurora_u32 (*Pop)(void);
} aurora_stack_api;

typedef struct aurora_io_api {
  aurora_u32 (*GetChar)(void);
  void (*PutChar)(aurora_u32);
  void (*PutInt)(int);
  void (*PutString)(const char *);
} aurora_io_api;

typedef struct aurora_api {
#ifndef AURORA_USER_API_ONLY
  void (*EnableInterrupt)(void);
  void (*DisableInterrupt)(void);
#endif
  void (*SetUpStackPointer)(void);
  void (*SetStackPointer)(aurora_u32);
  aurora_u32 (*GetStackPointer)(void);
#ifndef AURORA_USER_API_ONLY
  void (*Halt)(void) __attribute__((noreturn));
#endif
  void (*Nop)(void);
  aurora_u32 (*Read)(aurora_u32);
  void (*Write)(aurora_u32, aurora_u32);

#ifndef AURORA_USER_API_ONLY
  aurora_interrupt_api interrupt;
#endif
  aurora_stack_api stack;
  aurora_io_api io;
  aurora_address_api address;

  volatile const aurora_word *rom;
  volatile aurora_word *ram;
  volatile aurora_word *ivt;
  volatile const aurora_word *keyboard;
  volatile aurora_word *terminal;
  volatile aurora_word *telnet;
} aurora_api;

#ifndef AURORA_USER_API_ONLY
static __inline__ __attribute__((always_inline)) void
__aurora_enable_interrupt(void) {
  __builtin_aurora_enable_interrupt();
}

static __inline__ __attribute__((always_inline)) void
__aurora_disable_interrupt(void) {
  __builtin_aurora_disable_interrupt();
}
#endif

static __inline__ __attribute__((always_inline)) void
__aurora_set_stack_pointer(aurora_u32 address) {
  __builtin_aurora_set_stack_pointer(address);
}

static __inline__ __attribute__((always_inline)) void
__aurora_set_up_stack_pointer(void) {
  // _start's compiler-generated prologue initializes SP before allocating
  // local objects. Resetting it here would overwrite that active frame.
}

static __inline__ __attribute__((always_inline)) aurora_u32
__aurora_get_stack_pointer(void) {
  return __builtin_aurora_get_stack_pointer();
}

static __inline__ __attribute__((always_inline)) void
__aurora_push(aurora_u32 value) {
  __builtin_aurora_push(value);
}

static __inline__ __attribute__((always_inline)) aurora_u32
__aurora_pop(void) {
  return __builtin_aurora_pop();
}

static __inline__ __attribute__((always_inline)) aurora_u32
__aurora_get_char(void) {
  return *(volatile const aurora_word *)(unsigned long)0x04000000u;
}

static __inline__ __attribute__((always_inline)) void
__aurora_put_char(aurora_u32 value) {
  *(volatile aurora_word *)(unsigned long)0x05000000u = value;
}

#ifndef AURORA_USER_API_ONLY
static __inline__ __attribute__((always_inline, noreturn)) void
__aurora_halt(void) {
  __builtin_aurora_halt();
}
#endif

static __inline__ __attribute__((always_inline)) void __aurora_nop(void) {
  __builtin_aurora_nop();
}

static __inline__ __attribute__((always_inline)) aurora_u32
__aurora_read(aurora_u32 address) {
  return *(volatile aurora_word *)(unsigned long)address;
}

static __inline__ __attribute__((always_inline)) void
__aurora_write(aurora_u32 address, aurora_u32 value) {
  *(volatile aurora_word *)(unsigned long)address = value;
}

static __inline__ __attribute__((always_inline)) void
__aurora_register_interrupt(aurora_u32 vector_address,
                            aurora_interrupt_handler handler) {
  __aurora_write(vector_address, (aurora_u32)(unsigned long)handler);
}

/* Print one signed decimal integer without a trailing newline. */
static __attribute__((noinline)) void __aurora_put_int(int value) {
  aurora_u32 magnitude;

  if (value < 0) {
    __aurora_put_char((aurora_u32)'-');
    /* Unsigned subtraction also handles the INT_MIN magnitude. */
    magnitude = 0u - (aurora_u32)value;
  } else {
    magnitude = (aurora_u32)value;
  }

  if (magnitude == 0u) {
    __aurora_put_char((aurora_u32)'0');
    return;
  }

  aurora_u32 old_sp = __aurora_get_stack_pointer();
  while (magnitude != 0u) {
    __aurora_push((aurora_u32)'0' + magnitude % 10u);
    magnitude /= 10u;
  }
  while (__aurora_get_stack_pointer() > old_sp)
    __aurora_put_char(__aurora_pop());
}

/*
 * Print a zero-ended string. Aurora stores each char in one addressed word,
 * so strings are read as words even though the public C type is char *.
 */
static __attribute__((noinline)) void
__aurora_put_string(const char *text) {
  aurora_u32 address = (aurora_u32)(unsigned long)text;
  for (;;) {
    aurora_u32 c =
        *(volatile const aurora_word *)(unsigned long)address & 0xffu;
    if (c == 0u)
      return;
    __aurora_put_char(c);
    address++;
  }
}

static const aurora_api aurora = {
#ifndef AURORA_USER_API_ONLY
    __aurora_enable_interrupt,
    __aurora_disable_interrupt,
#endif
    __aurora_set_up_stack_pointer,
    __aurora_set_stack_pointer,
    __aurora_get_stack_pointer,
#ifndef AURORA_USER_API_ONLY
    __aurora_halt,
#endif
    __aurora_nop,
    __aurora_read,
    __aurora_write,
#ifndef AURORA_USER_API_ONLY
    {__aurora_enable_interrupt, __aurora_disable_interrupt,
     __aurora_register_interrupt},
#endif
    {__aurora_set_stack_pointer, __aurora_get_stack_pointer, __aurora_push,
     __aurora_pop},
    {__aurora_get_char, __aurora_put_char, __aurora_put_int,
     __aurora_put_string},
    {0x00000000u, 0x01000000u, 0x02000000u, 0x03000000u,
     0x04000000u, 0x05000000u, 0x06000000u},
    (volatile const aurora_word *)(unsigned long)0x00000000u,
    (volatile aurora_word *)(unsigned long)0x01000000u,
    (volatile aurora_word *)(unsigned long)0x03000000u,
    (volatile const aurora_word *)(unsigned long)0x04000000u,
    (volatile aurora_word *)(unsigned long)0x05000000u,
    (volatile aurora_word *)(unsigned long)0x06000000u};

#endif
