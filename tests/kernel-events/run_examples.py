"""Run real kernel/user BINs in the existing Aurora ISA contract model.
Not a Digital circuit, real socket, or microcode timing test.
"""
import pathlib
import sys
from collections import deque

ROOT = pathlib.Path(__file__).resolve().parents[2]
from run_system import SystemCPU
from run_target import signed, MASK


class DemoCPU(SystemCPU):
    def __init__(self, name, binary):
        super().__init__(ROOT / 'kernel/build/kernel.bin', binary, 0,
                         tick_period=10007, key_period=40000)
        self.name = name
        self.output = ''
        self.network_out = []
        self.network_in = deque()
        self.network_word = 0
        self.network_reads = self.network_irqs = 0
        self.telnet_entry = 0
        self.keyboard_entry = 0
        self.callback_inflight = False
        self.inject_on_key = []
        self.delivered = []
        self.halted = False

    def emit(self, text):
        self.output += text
        assert 'Console Write failed' not in self.output

    def read(self, address):
        if address == 0x06000000:
            self.network_reads += 1
            return self.network_word
        return super().read(address)

    def write(self, address, value):
        if address == 0x06000000:
            self.network_out.append(value & 255)
        else:
            super().write(address, value)

    def syscall(self):
        if self.r[0] == 13:
            self.telnet_entry = self.r[1]
        if self.r[0] == 2:
            self.keyboard_entry = self.r[1]
        if self.r[0] == 7:
            assert self.callback_inflight
            self.callback_inflight = False
        super().syscall()

    def step(self):
        self.output_started = bool(self.pending)
        if self.ie and self.network_in:
            self.network_word = self.network_in.popleft()
            self.interrupt(self.read(0x03000002))
            self.network_irqs += 1
        # Arbitrate before executing extra ALU operations below, too.
        if self.ie:
            if self.steps >= self.next_tick:
                self.next_tick = self.steps + self.tick_period
                self.interrupt(self.read(0x03000007)); self.ticks += 1
            elif self.output_started and self.steps >= self.next_key:
                self.next_key = self.steps + self.key_period
                self.key = ord(self.pending.pop(0))
                self.interrupt(self.read(0x03000001)); self.keys += 1
        if self.telnet_entry and self.pc == self.telnet_entry:
            assert not self.callback_inflight, 'nested Telnet callback'
            self.callback_inflight = True
            self.delivered.append(self.r[0])
        if self.keyboard_entry and self.pc == self.keyboard_entry:
            assert not self.callback_inflight, 'nested keyboard callback'
            self.callback_inflight = True
            if self.inject_on_key:
                self.network_in.extend(self.inject_on_key)
                self.inject_on_key = []
        word = self.read(self.pc)
        op = word & 255
        if op == 0xf0:
            self.halted = True
            return
        if op in (0x22, 0x23, 0x27, 0x2e, 0x51, 0x52):
            x = self.r[(word >> 16) & 15]; y = self.r[(word >> 12) & 15]
            if op == 0x22: value = (x + 1) & MASK
            elif op == 0x23: value = (x - 1) & MASK
            elif op == 0x2e: value = (~x) & MASK
            elif op == 0x27:
                shift = signed(y)
                assert -31 <= shift <= 31
                value = (x << shift if shift >= 0 else x >> -shift) & MASK
            else:
                a, b = signed(x), signed(y)
                assert b != 0
                quotient = abs(a) // abs(b)
                if (a < 0) != (b < 0): quotient = -quotient
                value = (quotient if op == 0x51 else a - quotient*b) & MASK
            self.r[(word >> 8) & 15] = value
            self.flags = (value == 0, bool(value & 0x80000000), False, False)
            if op == 0x22: self.flags = (value == 0, bool(value & 0x80000000), x == 0x7fffffff, x == MASK)
            elif op == 0x23: self.sub_flags(x, 1, value)
            self.pc += 1; self.steps += 1
            return
        super().step()

    def until(self, predicate, limit=2000000):
        deadline = self.steps + limit
        while not predicate():
            assert not self.halted and self.steps < deadline, (self.delivered, self.output[-2000:])
            self.step()

    def command(self, text, expected):
        before = len(self.output)
        self.pending.extend(text + '\r')
        self.until(lambda: expected in self.output[before:])


def terminal():
    cpu = DemoCPU('terminal', ROOT / 'user-programs/programs/mini-terminal/build/mini-terminal.bin')
    cpu.until(lambda: cpu.output.endswith('aurora> '))
    cpu.command('echo Hello', '\nHello\naurora> ')
    cpu.command('send Aurora', 'Sent to device')
    assert bytes(cpu.network_out) == b'Aurora\r\n', cpu.network_out
    # Burst IRQs: first event enters a callback; remaining events arrive while
    # it is active and must be deferred, never nested, replayed or discarded.
    cpu.network_in.extend([ord('A'), 0, 255, ord('B')])
    cpu.until(lambda: len(cpu.delivered) == 4 and not cpu.callback_inflight)
    deadline = cpu.steps + 20000
    cpu.until(lambda: cpu.steps >= deadline)
    assert '[net]' not in cpu.output
    assert [e & 255 for e in cpu.delivered] == [65, 0, 255, 66], cpu.delivered
    assert all(e >> 24 == 0xa6 for e in cpu.delivered)
    assert len(set(cpu.delivered)) == 4
    assert cpu.network_irqs == cpu.network_reads == 4
    cpu.network_in.append(13)
    cpu.until(lambda: '[net] A..B\naurora> ' in cpu.output)
    cpu.network_in.append(10)
    cpu.until(lambda: len(cpu.delivered) == 6 and not cpu.callback_inflight)
    deadline = cpu.steps + 20000
    cpu.until(lambda: cpu.steps >= deadline)
    assert cpu.output.count('[net]') == 1
    cpu.inject_on_key = [ord('X'), ord('Y'), 10]
    cpu.command('echo race', '\nrace\naurora> ')
    assert [e & 255 for e in cpu.delivered] == [65, 0, 255, 66, 13, 10, 88, 89, 10]
    assert cpu.output.count('[net]') == 2 and '[net] XY\n' in cpu.output
    cpu.command('uptime', ' ms\naurora> ')
    cpu.command('exit -7', 'USER_PROCESS exited with code -7.')
    cpu.until(lambda: cpu.halted)
    assert len(cpu.delivered) == 9 and cpu.ticks > 0 and cpu.keys > 0
    print(f'PASS terminal BIN: Telnet newline-only display, split CRLF, keyboard overlap, {cpu.ticks} ticks, {cpu.keys} keyboard IRQs')


if __name__ == '__main__':
    terminal()
