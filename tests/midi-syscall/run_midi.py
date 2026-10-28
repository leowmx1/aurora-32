"""Execute emitted ROM/UROM, including actual syscall and exception code."""
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tests/kernel-events'))
from run_events import CheckedCPU
from device import MidiDevice


class MidiCPU(MidiDevice, CheckedCPU):
    pass


def run(fault=None):
    cpu = MidiCPU(ROOT / 'tests/midi-syscall/build/contract.bin')
    cpu.until(lambda: 'MIDI CALLBACK READY\n' in cpu.output)
    expected = [(0,256), (0,255), (0,0), (127,129), (127,0), (127,256),
                (60,228), (60,0), (60,229), (64,208), (60,0), (64,0)]
    assert [(n,v) for n,v,t in cpu.midi_writes] == expected, cpu.midi_writes
    # Allow WaitEvent to sleep, then exercise syscall use inside a callback.
    deadline = cpu.steps + 30000
    cpu.until(lambda: cpu.steps >= deadline)
    cpu.inject.append((1, ord('k')))
    cpu.until(lambda: 'MIDI EXIT READY\n' in cpu.output)
    if fault:
        cpu.until(lambda: not cpu.kernel_mode)
        cpu.interrupt(cpu.read(fault))
    cpu.until(lambda: cpu.halted)
    expected += [(67,218), (67,0), (60,228), (64,208), (60,0), (64,0)]
    assert [(n,v) for n,v,t in cpu.midi_writes] == expected, cpu.midi_writes
    assert not cpu.midi_active and cpu.restores == 1 and not cpu.snapshots
    assert cpu.ticks > 0
    message = {None: 'exited with code 0.',
               0x03000004: 'Memory access violation in USER_PROCESS.',
               0x03000005: 'Illegal instruction in USER_PROCESS.'}[fault]
    assert message in cpu.output, cpu.output
    print(f'PASS MIDI BIN: bounds, MMIO bits, retrigger, zero velocity, chord, callback and cleanup ({message})')


if __name__ == '__main__':
    for fault in (None, 0x03000004, 0x03000005):
        run(fault)
