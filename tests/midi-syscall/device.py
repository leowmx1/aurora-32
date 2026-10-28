"""Ideal MIDI register sink; no sound synthesis or Digital timing."""

MIDI_BASE = 0x0a000000


class MidiDevice:
    def __init__(self, *args, **kwargs):
        self.midi_writes = []
        self.midi_active = set()
        self.midi_program = None
        super().__init__(*args, **kwargs)

    def read(self, address):
        assert not 0x0b000000 <= address < 0x10000000, 'retired device segment read'
        assert not MIDI_BASE <= address < MIDI_BASE + 0x01000000, 'MIDI is write-only'
        return super().read(address)

    def write(self, address, value):
        assert not 0x0b000000 <= address < 0x10000000, 'retired device segment write'
        if MIDI_BASE <= address < MIDI_BASE + 0x01000000:
            assert self.kernel_mode and not self.ie, 'MIDI must use kernel INT path'
            assert MIDI_BASE <= address < MIDI_BASE + 128, hex(address)
            assert 0 <= value <= 256, value
            note = address - MIDI_BASE
            self.midi_writes.append((note, value, self.steps))
            if value == 256:
                self.midi_program = note
            elif value >= 128:
                assert value > 128, 'velocity zero should be normalized to NoteOff'
                assert note not in self.midi_active, 'retrigger without release'
                self.midi_active.add(note)
            else:
                assert value == 0
                self.midi_active.discard(note)
        else:
            super().write(address, value)
