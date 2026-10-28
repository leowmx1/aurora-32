"""Actual kernel/user BIN sleep contract; no scheduler emulation, not Digital."""
from run_events import CheckedCPU, ROOT
import sys
sys.path.insert(0, str(ROOT/'tests/midi-syscall'))
from device import MidiDevice


class SleepCPU(MidiDevice, CheckedCPU):
    def __init__(self, wrap=False):
        super().__init__(ROOT/'tests/kernel-events/build/sleep_contract.bin')
        self.tick_period = self.next_tick = 1009
        self.offset = 0
        self.wrap = wrap
        self.sleep_requests = 0
        self.note64_user_steps = None

    def write(self, address, value):
        if address == 0x0a000040 and value == 208:
            self.note64_user_steps = self.user_steps
        super().write(address, value)

    def read(self, address):
        if address == 0x08000000: return (self.offset + self.steps//100) & 0xffffffff
        return super().read(address)

    def emit(self, text):
        super().emit(text)
        if self.wrap and self.output.endswith('TIMER\n'):
            self.offset = (0xfffffff0 - self.steps//100) & 0xffffffff

    def syscall(self):
        if not self.kernel_mode and self.r[0] == 29 and self.r[1] in (100,1000,1200,0x7fffffff):
            self.sleep_requests += 1
        super().syscall()

    def blocked(self, request):
        self.until(lambda: self.sleep_requests >= request)
        # After syscall: return to kernel SYSTEM with interrupts enabled.
        self.until(lambda: self.kernel_mode and self.ie)
        end = self.steps + 3000
        before = self.user_steps, self.system_yields
        self.until(lambda: self.steps >= end)
        assert (self.user_steps, self.system_yields) == before, 'blocked user ran or SYSTEM spun Yield'


def run(wrap):
    c = SleepCPU(wrap)
    c.until(lambda: c.output.endswith('TIMER\n'))
    c.blocked(1)
    c.until(lambda: c.output.endswith('KEY\n'))
    c.blocked(2)
    ticks = c.ticks
    c.next_tick = c.steps + 1000000
    c.inject.append((1, ord('k')))
    c.until(lambda: c.output.endswith('CANCEL\n'), limit=30000)
    assert c.ticks == ticks, 'key wake must not require a tick'
    c.next_tick = c.steps + c.tick_period
    c.blocked(3)
    sleeping_user_steps = c.user_steps
    c.until(lambda: c.output.endswith('NET\n'), limit=300000)
    assert c.note64_user_steps == sleeping_user_steps, 'MIDI did not advance while user slept'
    assert not c.midi_active
    c.blocked(4)
    c.inject.append((2, ord('n')))
    c.until(lambda: c.output.endswith('RACE\n'))
    c.inject.append((1, ord('r')))
    c.until(lambda: c.output.endswith('WAIT\n'))
    # Let the prior long sleep deadline pass: WaitEvent must NOT time out.
    end = c.steps + 140000
    c.until(lambda: c.steps >= end)
    before = c.user_steps, c.system_yields
    end = c.steps + 20000
    c.until(lambda: c.steps >= end)
    assert (c.user_steps, c.system_yields) == before
    c.inject.append((1, ord('w')))
    c.until(lambda: c.halted)
    assert 'SLEEP CONTRACT PASS' in c.output and 'exited with code 0.' in c.output, c.output
    assert c.restores == 4 and not c.snapshots
    print(f'PASS Sleep: timed block, zero/invalid/deferred/callback, key/net early wake, credit race, cancel, WaitEvent, wrap={wrap}')


if __name__ == '__main__':
    run(False)
    run(True)
