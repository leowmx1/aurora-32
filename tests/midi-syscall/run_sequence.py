"""Actual BIN: tick-only audio under user compute/slow drawing, no user pump."""
import pathlib
import sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tests/graphics-syscall'))
from run_graphics import GraphicsCPU
from run_events import CheckedCPU
from device import MidiDevice


class SequenceCPU(MidiDevice, GraphicsCPU):
    def __init__(self, duration, offset):
        self.offset = offset
        super().__init__(ROOT / 'tests/midi-syscall/build/sequence_contract.bin', duration=duration)

    def read(self, address):
        if address == 0x08000000:
            return (self.offset + self.steps // 1000) & 0xffffffff
        return super().read(address)

    def syscall(self):
        CheckedCPU.syscall(self)


def run(duration, offset):
    cpu = SequenceCPU(duration, offset)
    cpu.until(lambda: 'SEQUENCE READY\n' in cpu.output)
    first = next(t for n,v,t in cpu.midi_writes if (n,v) == (60,168))
    cpu.until(lambda: any((n,v) == (62,168) for n,v,t in cpu.midi_writes))
    note62 = next(t for n,v,t in cpu.midi_writes if (n,v) == (62,168))
    target = note62 + 120000
    cpu.until(lambda: cpu.steps >= target)
    cpu.inject.append((1, ord('e')))
    cpu.until(lambda: sum((n,v) == (65,168) for n,v,t in cpu.midi_writes) >= 2,
              limit=4500000)
    presses = [(n,v,t) for n,v,t in cpu.midi_writes if v >= 128 and v != 256]
    assert [(n,v) for n,v,t in presses[:7]] == [
        (60,168),(62,168),(84,218),(88,218),(62,168),(64,168),(65,168)], presses
    # Resume the interrupted second note, not note60; fixed beat grid resumes
    # after 240 ms effect. Next loop/notes do not accumulate callback/draw delay.
    expected_ms = [0,400,None,None,None,1040,1440,1840,2240,2640,3040]
    for (_,_,time), expected in zip(presses, expected_ms):
        if expected is not None:
            assert abs((time-first)/1000 - expected) < 30, (presses, expected)
    assert not any(n in (10,33) for n,v,t in presses), 'user buffer reread'
    if duration:
        assert any(first < t < first+duration for n,v,t in presses[1:]), 'no audio while drawing'
    cpu.inject.append((1, ord('q')))
    cpu.until(lambda: cpu.halted)
    assert 'SEQUENCE PASS' in cpu.output and not cpu.midi_active, cpu.output
    assert cpu.restores == 2 and not cpu.snapshots
    print(f'PASS sequence: tick-only playback, frozen-position resume, fixed deadlines, copied data, validation; draw={duration}, offset={offset:#x}')


if __name__ == '__main__':
    run(0, 0)
    run(2000000, 0)
    run(2000000, 0xfffffe00)
