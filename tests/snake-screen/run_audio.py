"""Sequencer BIN test; ideal time/MMIO, no actual synthesizer or Digital."""
from run_snake_screen import ROOT, MidiDevice, CheckedCPU


class AudioCPU(MidiDevice, CheckedCPU):
    def __init__(self, offset=0):
        self.offset = offset
        super().__init__(ROOT / 'tests/snake-screen/build/audio_contract.bin')

    def read(self, address):
        if address == 0x08000000:
            return (self.offset + self.steps // 100) & 0xffffffff
        return super().read(address)


for offset in (0, 0xfffffe00):
    cpu = AudioCPU(offset)
    cpu.until(lambda: cpu.halted, limit=4000000)
    assert 'SNAKE AUDIO PASS' in cpu.output and 'exited with code 0.' in cpu.output
    notes = [(n,v,t) for n,v,t in cpu.midi_writes if v != 256]
    assert len(notes) % 2 == 0 and not cpu.midi_active
    # The single-channel arrangement never leaves an old note under a new one.
    for on, off in zip(notes[::2], notes[1::2]):
        assert on[1] >= 128 and off[1] == 0 and on[0] == off[0], (on,off)
    presses = [(n,v) for n,v,t in notes if v >= 128]
    loud = [n for n,v in presses if v == 218]
    assert loud == [72,76,79, 84,88,91, 67,63,60,48, 72,76,79,84], loud
    soft = [n for n,v in presses if v == 170]
    motif = [72,71,72,71,72,67,69,72,75,74,72,74,
             72,71,72,71,72,68,67,79,78,79]
    assert any(soft[i:i+44] == motif * 2 for i in range(4)), soft
    assert len(soft) > 44 and cpu.ticks > 0
    assert all(n != 0 for n,v in presses), 'rest must not send MIDI note 0'
    print(f'PASS audio BIN: start, looping melody, food/resume, loss, win, releases; timer offset={offset:#x}')
