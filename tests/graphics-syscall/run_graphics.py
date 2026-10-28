"""Actual BIN execution; ideal ISA/device model, NOT a Digital circuit test."""
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tests/kernel-events'))
from run_events import CheckedCPU

STATUS = 0x09000009
REGS = [0x09000000, 0x09000001, 0x09000002, 0x09000003, 0x09000004]
PARAMS = list(range(0x09000000, 0x09000009))
MODE = 0x09000008
RECTS = [(40, 25, 80, 50, 4), (5, 5, 25, 15, 2), (125, 80, 30, 15, 3)]


class GraphicsCPU(CheckedCPU):
    def __init__(self, binary, *, inject=False, duration=80000, stuck=False):
        super().__init__(binary)
        self.params = {}
        self.commands = []
        self.operations = []
        self.writes = []
        self.screen = [-1] * 16000
        self.deadline = None
        self.duration = duration
        self.inject_keyboard = inject
        self.stuck = stuck
        self.injected = False
        self.ticks_at_start = 0
        self.busy_ticks = 0
        self.injected_keys = 0
        self.user_yields = 0

    def complete(self):
        if self.deadline is not None and self.steps >= self.deadline and not self.stuck:
            x, y, w, h, color, bg, low, high, mode = self.operations[-1]
            bits = low | (high << 32)
            for row in range(y, y+h):
                for col in range(x, x+w):
                    bit = (bits >> ((row-y)*w + col-x)) & 1
                    if mode == 0 or bit:
                        self.screen[row*160+col] = color
                    elif mode == 1:
                        self.screen[row*160+col] = bg
            self.busy_ticks += self.ticks - self.ticks_at_start
            self.deadline = None

    def read(self, address):
        assert not 0x0a000000 <= address < 0x10000000, 'retired graphics / write-only MIDI read'
        if 0x09000000 <= address < 0x0a000000:
            assert address == STATUS, 'reserved or unexpected graphics register read'
        if address == STATUS:
            assert self.kernel_mode, 'user MMIO'
            self.complete()
            return int(self.deadline is not None)
        return super().read(address)

    def write(self, address, value):
        # MidiDevice handles valid 0x0A writes before reaching this layer.
        assert not 0x0a000000 <= address < 0x10000000, 'retired graphics / unhandled MIDI write'
        if 0x09000000 <= address < 0x0a000000:
            assert address in PARAMS or address == STATUS, 'unexpected graphics register'
        assert address != 0x03000003, 'graphics IVT entry must not be installed'
        if address in PARAMS or address == STATUS:
            assert self.kernel_mode and not self.ie
            self.complete()
            assert self.deadline is None, 'overwriting active fill'
            self.writes.append((address, value))
            if address != STATUS:
                self.params[address] = value
            elif value == 1:
                rect = tuple(self.params[r] for r in REGS)
                x, y, w, h, color = rect
                assert 0 <= x < 160 and 0 <= y < 100
                assert 0 < w <= 160-x and 0 < h <= 100-y
                mode = self.params[MODE]
                assert mode in (0, 1, 2)
                if mode: assert w*h <= 64
                sequence = [STATUS] + (PARAMS if mode else REGS + [MODE]) + [STATUS]
                assert [a for a, v in self.writes[-len(sequence):]] == sequence
                assert self.writes[-len(sequence)][1] == 0
                self.commands.append(rect)
                self.operations.append(tuple(self.params.get(r, 0) for r in PARAMS))
                self.deadline = self.steps + self.duration
                self.ticks_at_start = self.ticks
                self.injected = False
            else:
                assert value == 0
        else:
            super().write(address, value)

    def step(self):
        self.complete()
        if self.inject_keyboard and self.deadline is not None and not self.injected and self.ie:
            self.key = ord('k')
            self.interrupt(self.read(0x03000001))
            self.injected = True
            self.injected_keys += 1
        # Arbitrate tick before observing entry PC, just as DemoCPU does for
        # keyboard/Telnet; a tick before the first callback instruction is not
        # a second callback entry when that PC is later resumed.
        if self.ie and self.steps >= self.next_tick:
            self.next_tick = self.steps + self.tick_period
            self.interrupt(self.read(0x03000007))
            self.ticks += 1
        super().step()

    def syscall(self):
        if not self.kernel_mode:
            assert self.r[0] != 15 or not self.commands, 'graphics path must not block in WaitEvent'
            if self.r[0] == 4: self.user_yields += 1
        super().syscall()


def run():
    demo = ROOT / 'user-programs/programs/graphics-demo/build/graphics-demo.bin'
    for duration in (0, 80000):
        cpu = GraphicsCPU(demo, duration=duration)
        cpu.until(lambda: cpu.halted)
        assert cpu.commands == RECTS + [(56,40,8,8,2), (104,40,8,8,2), (56,52,8,2,3)]
        assert len(cpu.writes) == 54
        assert 'Graphics complete:' in cpu.output and 'exited with code 0.' in cpu.output
        expected = [-1]*16000
        for x,y,w,h,c in RECTS + [(56,40,8,8,5), (56,52,8,2,3)]:
            for yy in range(y,y+h):
                for xx in range(x,x+w): expected[yy*160+xx] = c
        for yy, width in enumerate((6,1,1,5,1,1,1,0)):
            for xx in range(width):
                expected[(40+yy)*160+56+xx] = 2
                expected[(40+yy)*160+104+xx] = 2
        assert cpu.screen == expected
        assert cpu.restores == 0
        if duration: assert cpu.busy_ticks > 0 and cpu.user_yields > 0
        print(f'PASS demo: duration={duration}, preserved pixels, busy ticks={cpu.busy_ticks}')

    cpu = GraphicsCPU(ROOT / 'tests/graphics-syscall/build/contract.bin', inject=True)
    cpu.until(lambda: cpu.halted)
    assert 'GRAPHICS CONTRACT PASS' in cpu.output, cpu.output
    assert 'exited with code 0.' in cpu.output
    assert cpu.commands == [(0, 0, 160, 100, 0)] + RECTS + [(159, 99, 1, 1, 4)]
    assert len(cpu.writes) == 40 and cpu.injected_keys == 5 and cpu.busy_ticks > 0
    assert cpu.restores == 5 and not cpu.snapshots
    assert cpu.screen.count(4) == 4001 and cpu.screen.count(2) == 375
    assert cpu.screen.count(3) == 450 and cpu.screen.count(0) == 11174
    print(f'PASS contract: wrapper/raw validation, no busy writes, 5 keyboard callbacks, {cpu.busy_ticks} busy ticks')

    cpu = GraphicsCPU(ROOT / 'tests/graphics-syscall/build/contract.bin', inject=True, stuck=True)
    cpu.until(lambda: cpu.halted)
    assert 'Graphics timeout (10 s).' in cpu.output and 'exited with code 1.' in cpu.output
    assert len(cpu.commands) == 1 and len(cpu.writes) == 8
    print('PASS stuck status: user timeout without an in-kernel busy loop or command overwrite')

    cpu = GraphicsCPU(demo, stuck=True)
    cpu.until(lambda: cpu.halted)
    assert 'Graphics timeout (10 s).' in cpu.output and 'exited with code 1.' in cpu.output
    assert len(cpu.commands) == 1 and cpu.ticks > 0 and cpu.user_yields > 0
    print('PASS demo timeout: polling yields with normal tick, no completion IRQ or WaitEvent')


if __name__ == '__main__':
    run()
