"""Run BOTH emitted binaries with banked stacks, tick and keyboard IRQs.

Instruction-level contract model, not a microcode/electrical simulator.
No syscall, scheduler or callback restoration is implemented by the harness:
those execute from kernel.bin. IRQ entry/IRET follow the hardware contract.
"""
import argparse
import pathlib
import struct
from run_target import CPU, MASK


class SystemCPU(CPU):
    def __init__(self, kernel, user, frames, tick_period=997, key_period=1301):
        self.kernel_mode = True
        self.stack = [0, 0]
        self.frame_pointer = [0, 0]
        super().__init__(user, frames)
        data = pathlib.Path(kernel).read_bytes()
        self.kernel_rom = struct.unpack('<' + 'I' * (len(data) // 4), data)
        self.pc = 0
        self.ie = False
        self.tick_period, self.key_period = tick_period, key_period
        self.next_tick = tick_period
        self.next_key = key_period
        self.key = 0
        self.ticks = self.keys = 0
        self.output_started = False

    @property
    def sp(self):
        return self.stack[int(self.kernel_mode)]

    @sp.setter
    def sp(self, value):
        self.stack[int(self.kernel_mode)] = value

    @property
    def fp(self):
        return self.frame_pointer[int(self.kernel_mode)]

    @fp.setter
    def fp(self, value):
        self.frame_pointer[int(self.kernel_mode)] = value

    def read(self, address):
        if address < len(self.kernel_rom):
            return self.kernel_rom[address]
        if address == 0x04000000:
            return self.key
        if address == 0x07000000:
            return 0
        if address == 0x08000000:
            return (self.steps // 20) & MASK
        if 0x01000000 <= address < 0x04000000:
            return self.mem.get(address, 0)
        return super().read(address)

    def write(self, address, value):
        if address == 0x05000000:
            self.emit(chr(value & 255))
        elif 0x01000000 <= address < 0x04000000:
            self.mem[address] = value & MASK
        else:
            super().write(address, value)

    def emit(self, text):
        if '\f' in text:
            self.output_started = True
        if self.output_started:
            super().emit(text)

    def interrupt(self, handler):
        z, n, v, c = self.flags
        flags = int(z) | int(c) << 1 | int(n) << 2 | int(v) << 3
        flags |= int(self.kernel_mode) << 4
        self.kernel_mode = True
        self.ie = False
        self.push(flags)
        self.push(self.pc)
        self.pc = handler

    def syscall(self):
        self.interrupt(self.read(self.pc - 1))

    def step(self):
        if self.ie:
            if self.steps >= self.next_tick:
                self.next_tick = self.steps + self.tick_period
                self.interrupt(self.read(0x03000007))
                self.ticks += 1
            elif self.output_started and self.steps >= self.next_key:
                self.next_key = self.steps + self.key_period
                self.key = ord(self.pending.pop(0) if self.pending else '!')
                self.interrupt(self.read(0x03000001))
                self.keys += 1
        word = self.read(self.pc)
        op = word & 255
        if op in (0x39, 0x3a, 0x3b, 0x60, 0x61, 0xf0):
            self.steps += 1
            self.pc += 1
            if op == 0x39:
                self.pc = self.pop()
                flags = self.pop()
                self.flags = (bool(flags & 1), bool(flags & 4),
                              bool(flags & 8), bool(flags & 2))
                self.kernel_mode = bool(flags & 16)
                self.ie = True
            elif op == 0x3a:
                self.ie = True
            elif op == 0x3b:
                self.ie = False
            elif op == 0x60:
                self.write(self.stack[0], self.r[(word >> 16) & 15])
                self.stack[0] += 1
            elif op == 0x61:
                self.stack[0] -= 1
                self.r[(word >> 8) & 15] = self.read(self.stack[0])
            else:
                raise AssertionError(f'HALT at {self.pc-1:x}:\n{self.frame}')
        else:
            super().step()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('kernel')
    parser.add_argument('user')
    parser.add_argument('--frames', type=int, default=100)
    parser.add_argument('--tick-period', type=int, default=997)
    parser.add_argument('--key-period', type=int, default=1301)
    args = parser.parse_args()
    cpu = SystemCPU(args.kernel, args.user, args.frames,
                    args.tick_period, args.key_period)
    while cpu.frames < args.frames:
        cpu.step()
    print(f'PASS: kernel + user BIN, {cpu.frames} frames, '
          f'{cpu.ticks} tick IRQs, {cpu.keys} keyboard IRQs, {cpu.steps} instructions')
