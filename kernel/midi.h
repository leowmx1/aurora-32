#ifndef AURORA_KERNEL_MIDI_H
#define AURORA_KERNEL_MIDI_H

#define AURORA_MIDI_DEVICE 0x0a000000u
/* One hardware-selected channel. Word-sized state; no user pointers. */
static volatile aurora_word_t midi_active[128];
/* Two bounded, kernel-owned tracks: background and one foreground effect. */
static volatile aurora_word_t midi_steps[2][256];
static volatile aurora_word_t midi_count[2], midi_total[2], midi_start[2];
static volatile aurora_word_t midi_loop, midi_background_elapsed;
static volatile aurora_word_t midi_voice = 128u, midi_voice_slot;
static volatile aurora_word_t midi_voice_index, midi_voice_start;
static volatile aurora_word_t midi_due, midi_due_valid;

__attribute__((noinline, optnone)) static void kernel_midi_initialize(void) {
  for (aurora_word_t note = 0u; note < 128u; ++note)
    midi_active[note] = 0u;
  midi_count[0] = midi_count[1] = midi_due_valid = 0u;
  midi_voice = 128u;
}

__attribute__((noinline, optnone)) static aurora_status_t
kernel_midi_note_off(aurora_word_t note) {
  if (note > 127u) return AURORA_EINVAL;
  aurora.Write(AURORA_MIDI_DEVICE + note, 0u);
  midi_active[note] = 0u;
  return AURORA_OK;
}

__attribute__((noinline, optnone)) static aurora_status_t
kernel_midi_note_on(aurora_word_t note, aurora_word_t velocity) {
  if (note > 127u || velocity > 127u) return AURORA_EINVAL;
  /* MIDI velocity zero means release. Retrigger explicitly releases the
   * previous voice to avoid stacking repeated NoteOn messages on one key. */
  if (velocity == 0u) return kernel_midi_note_off(note);
  if (midi_active[note]) aurora.Write(AURORA_MIDI_DEVICE + note, 0u);
  aurora.Write(AURORA_MIDI_DEVICE + note, 128u + velocity);
  midi_active[note] = 1u;
  return AURORA_OK;
}

__attribute__((noinline, optnone)) static aurora_status_t
kernel_midi_program(aurora_word_t program) {
  if (program > 127u) return AURORA_EINVAL;
  aurora.Write(AURORA_MIDI_DEVICE + program, 256u);
  return AURORA_OK;
}

/* Bounded cleanup, only writes notes started by this kernel. No MMIO reads,
 * busy-wait, IRQ or delay; INT already cleared IE for all these operations. */
__attribute__((noinline, optnone)) static aurora_status_t
kernel_midi_all_notes_off(void) {
  midi_count[0] = midi_count[1] = midi_due_valid = 0u;
  midi_voice = 128u;
  for (aurora_word_t note = 0u; note < 128u; ++note) {
    if (midi_active[note]) {
      aurora.Write(AURORA_MIDI_DEVICE + note, 0u);
      midi_active[note] = 0u;
    }
  }
  return AURORA_OK;
}

/* Called with the complete IRQ register frame already saved. Timer is read
 * only when tracks exist. Deadlines advance from the original timeline, not
 * from delayed service time. Missed notes are skipped, never burst-replayed. */
__attribute__((noinline, optnone)) static void kernel_midi_service(void) {
  if (midi_count[0] == 0u && midi_count[1] == 0u) return;
  aurora_word_t now = aurora.Read(AURORA_TIMER_DEVICE);
  if (midi_due_valid && (int)(now - midi_due) < 0) return;
  aurora_word_t slot = 0u;
  if (midi_count[1]) {
    if ((aurora_word_t)(now - midi_start[1]) < midi_total[1]) slot = 1u;
    else {
      midi_count[1] = 0u;
      /* Resume the frozen background position at the effect's scheduled end. */
      midi_start[0] = midi_start[1] + midi_total[1] - midi_background_elapsed;
    }
  }
  aurora_word_t note = 128u, velocity = 0u, index = 0u, event_start = 0u;
  midi_due_valid = 0u;
  if (midi_count[slot]) {
    aurora_word_t elapsed = now - midi_start[slot];
    if (slot == 0u && midi_loop) {
      aurora_word_t cycles = elapsed / midi_total[0];
      midi_start[0] += cycles * midi_total[0];
      elapsed %= midi_total[0];
    } else if (elapsed >= midi_total[slot]) midi_count[slot] = 0u;
    if (midi_count[slot]) {
      event_start = midi_start[slot];
      for (index = 0u; index < midi_count[slot]; ++index) {
        aurora_word_t base = index * 4u;
        aurora_word_t hold = midi_steps[slot][base + 2u];
        aurora_word_t length = hold + midi_steps[slot][base + 3u];
        if (elapsed < length) {
          if (elapsed < hold) {
            note = midi_steps[slot][base];
            velocity = midi_steps[slot][base + 1u];
            midi_due = event_start + hold;
          } else midi_due = event_start + length;
          midi_due_valid = 1u;
          break;
        }
        elapsed -= length;
        event_start += length;
      }
    }
  }
  if (velocity == 0u) note = 128u;
  if (midi_voice != 128u &&
      (note != midi_voice || slot != midi_voice_slot ||
       index != midi_voice_index || event_start != midi_voice_start)) {
    kernel_midi_note_off(midi_voice);
    midi_voice = 128u;
  }
  if (note != 128u && midi_voice == 128u) {
    kernel_midi_note_on(note, velocity);
    midi_voice = note;
    midi_voice_slot = slot;
    midi_voice_index = index;
    midi_voice_start = event_start;
  }
}

/* All fields validated before changing either track. Copy from UROM/URAM,
 * never retain user pointers or read the inaccessible banked user stack. */
__attribute__((noinline, optnone)) static aurora_status_t
kernel_midi_sequence(aurora_word_t address, aurora_word_t count, aurora_word_t flags) {
  if (count == 0u || count > 64u || flags > 2u) return AURORA_EINVAL;
  aurora_word_t words = count * 4u;
  if (address > 0xffffffffu - words || !is_user_read_address(address) ||
      !is_user_read_address(address + words - 1u)) return AURORA_EFAULT;
  aurora_word_t total = 0u;
  for (aurora_word_t i = 0u; i < words; i += 4u) {
    aurora_word_t note = aurora.Read(address + i);
    aurora_word_t velocity = aurora.Read(address + i + 1u);
    aurora_word_t hold = aurora.Read(address + i + 2u);
    aurora_word_t gap = aurora.Read(address + i + 3u);
    if (note > 128u || velocity > 127u || hold > 60000u || gap > 60000u ||
        hold + gap == 0u) return AURORA_EINVAL;
    total += hold + gap;
  }
  aurora_word_t slot = flags == 2u ? 1u : 0u;
  kernel_midi_service();
  if (midi_count[0] == 0u && midi_count[1] == 0u) kernel_midi_all_notes_off();
  for (aurora_word_t i = 0u; i < words; ++i)
    midi_steps[slot][i] = aurora.Read(address + i);
  /* No service after overwriting the old data until its metadata is replaced. */
  aurora_word_t now = aurora.Read(AURORA_TIMER_DEVICE);
  if (slot == 1u && midi_count[1] == 0u)
    midi_background_elapsed = now - midi_start[0];
  if (slot == 0u) {
    midi_loop = flags;
    midi_background_elapsed = 0u;
  }
  midi_count[slot] = count;
  midi_total[slot] = total;
  midi_start[slot] = now;
  /* Force retrigger even when replacing a sequence with the same first key. */
  if (slot == 1u || midi_count[1] == 0u) {
    if (midi_voice != 128u) kernel_midi_note_off(midi_voice);
    midi_voice = 128u;
  }
  midi_due_valid = 0u;
  kernel_midi_service();
  return AURORA_OK;
}

#endif
