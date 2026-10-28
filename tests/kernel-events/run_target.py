"""Execute the generated Aurora user BIN against a small ISA/ABI test model.

This tests emitted machine code (unlike host C tests). Syscalls and context
restoration are idealized: this is NOT a Digital/microcode timing simulator.
"""
import argparse
import pathlib
import re
import struct
from collections import deque

MASK = 0xffffffff
BASE = 0x80000000


def signed(x):
    return x if x < 0x80000000 else x - 0x100000000


class CPU:
    def __init__(self, binary, frame_limit, route='rectangle'):
        data = pathlib.Path(binary).read_bytes()
        self.rom = struct.unpack('<' + 'I' * (len(data) // 4), data)
        self.mem = {}
        self.r = [0] * 16
        self.pc = BASE
        self.sp = self.fp = 0
        self.flags = (False, False, False, False)  # z, n, v, borrow
        self.callback = self.trampoline = 0
        self.saved = None
        self.pending = []
        self.sequence = self.clock = self.steps = self.frames = 0
        self.frame_limit = frame_limit
        self.frame = ''
        self.last_direction = 'd'
        self.route = route
        self.score = 0

    def read(self, address):
        if BASE <= address < BASE + len(self.rom):
            return self.rom[address - BASE]
        assert 0x81000000 <= address < 0x83000000, hex(address)
        return self.mem.get(address, 0)

    def write(self, address, value):
        assert 0x81000000 <= address < 0x83000000, hex(address)
        self.mem[address] = value & MASK

    def push(self, value):
        self.write(self.sp, value)
        self.sp += 1

    def pop(self):
        self.sp -= 1
        return self.read(self.sp)

    def sub_flags(self, a, b, result):
        self.flags = (result == 0, bool(result & 0x80000000),
                      bool((a ^ b) & (a ^ result) & 0x80000000), a < b)

    def condition(self, code):
        z, n, v, c = self.flags
        return {1: c, 2: z, 3: not c and not z, 4: not c, 5: not z,
                6: c or z, 7: n, 8: v, 9: n != v, 10: n == v,
                11: not z and n == v, 12: z or n != v}[code]

    def emit(self, text):
        for c in text:
            if c == '\f':
                self.frame = ''
            else:
                self.frame += c
        if 'Console Write failed' in self.frame:
            raise AssertionError(self.frame)
        if self.frame.endswith('+/- speed  Q quit\n'):
            self.check_frame()

    def check_frame(self):
        lines = self.frame.splitlines()
        score = int(re.fullmatch(r'SNAKE  Score:(\d+)', lines[0]).group(1))
        self.score = score
        rows = lines[2:10]
        assert len(rows) == 8 and all(len(row) == 25 for row in rows), rows
        assert all(row[0] == row[-1] == '|' and row[2:-1:2] == ' ' * 11
                   for row in rows), rows
        cells = {(x, y): c for y, row in enumerate(rows)
                 for x, c in enumerate(row[1:-1:2]) if c in 'o@'}
        heads = [p for p, c in cells.items() if c == '@']
        assert len(heads) == 1 and len(cells) == score + 3, (
            f'body/score mismatch at frame {self.frames}:\n{self.frame}')
        reached = {heads[0]}
        todo = list(reached)
        while todo:
            x, y = todo.pop()
            for p in [(x-1, y), (x+1, y), (x, y-1), (x, y+1)]:
                if p in cells and p not in reached:
                    reached.add(p)
                    todo.append(p)
        assert len(reached) == len(cells), f'disconnected body:\n{self.frame}'
        if self.frames == 0:
            assert cells == {(4, 4): 'o', (5, 4): 'o', (6, 4): '@'}, self.frame
            self.pending.append(' ')
        else:
            x, y = heads[0]
            direction = self.last_direction
            if self.route == 'food':
                food = next((xx, yy) for yy, row in enumerate(rows)
                            for xx, c in enumerate(row[1:-1:2]) if c == '*')
                todo = deque([(heads[0], '')])
                visited = {heads[0]}
                while todo:
                    (xx, yy), first = todo.popleft()
                    if (xx, yy) == food:
                        direction = first
                        break
                    for dx, dy, key in [(1, 0, 'd'), (0, 1, 's'), (-1, 0, 'a'), (0, -1, 'w')]:
                        p = (xx+dx, yy+dy)
                        if 0 <= p[0] < 12 and 0 <= p[1] < 8 and p not in cells and p not in visited:
                            visited.add(p)
                            todo.append((p, first or key))
                else:
                    raise AssertionError('test autoplayer has no free path to food')
            else:
                if direction == 'd' and x == 9: direction = 's'
                elif direction == 's' and y == 6: direction = 'a'
                elif direction == 'a' and x == 3: direction = 'w'
                elif direction == 'w' and y == 2: direction = 'd'
            if direction != self.last_direction:
                self.pending.append(direction)
                self.last_direction = direction
        assert 'Game over' not in self.frame, self.frame
        self.frames += 1

    def syscall(self):
        request, a, b, c = self.r[:4]
        if request == 2:
            self.callback, self.trampoline = a, b
            self.r[0] = 0
        elif request == 7:
            assert self.saved is not None
            regs, pc, flags, sp, fp = self.saved
            assert self.sp == sp and self.fp == fp, 'callback broke user frame'
            self.r, self.pc, self.flags = regs, pc, flags
            self.saved = None
        elif request == 10:
            self.clock = (self.clock + 100) & MASK
            self.r[0] = self.clock
        elif request == 9:
            assert 0 <= b <= 64
            assert 0x80000000 <= a < 0x82000000 - b + 1
            self.emit(''.join(chr(self.read(a+i) & 255) for i in range(b)))
            self.r[0] = b
        elif request == 1:
            self.emit(chr(a & 255)); self.r[0] = 0
        elif request == 5:
            self.emit(str(signed(a))); self.r[0] = 0
        elif request == 6:
            text = ''
            for i in range(4096):
                word = self.read(a+i)
                if word == 0: break
                text += chr(word & 255)
            self.emit(text); self.r[0] = 0
        elif request == 8:
            raise AssertionError(f'unexpected exit {a}:\n{self.frame}')
        else:
            raise AssertionError(f'unexpected syscall {request}')

    def step(self):
        # Deliver a real compiled callback at arbitrary user-instruction
        # boundaries. Model only the kernel's promised restoration contract.
        if self.callback and self.saved is None:
            key = self.pending.pop(0) if self.pending else None
            if key is None and self.steps % 1301 == 0:
                key = '!'  # ignored input stresses callback save/return
            if key is not None:
                self.saved = (self.r.copy(), self.pc, self.flags, self.sp, self.fp)
                self.sequence = self.sequence % 65535 + 1
                self.push(self.trampoline)
                self.r[0] = 0xa5000000 | self.sequence * 256 | ord(key)
                self.pc = self.callback
        self.steps += 1
        assert self.steps < 10000000, 'instruction budget exceeded'
        word = self.read(self.pc); self.pc += 1
        op = word & 255
        a, b, d = (word >> 16) & 15, (word >> 12) & 15, (word >> 8) & 15
        x, y = self.r[a], self.r[b]
        if op in (0x13, 0x14, 0x15, 0x16, 0x30, 0x31, 0x34, 0x37):
            immediate = self.read(self.pc); self.pc += 1
        if op == 0: pass
        elif op == 0x10: self.r[d] = x
        elif op == 0x11: self.r[d] = self.read(y)
        elif op == 0x12: self.write(y, x)
        elif op == 0x13: self.r[d] = self.read(immediate)
        elif op == 0x14: self.write(immediate, x)
        elif op in (0x15, 0x16): self.r[d] = immediate
        elif op == 0x17: self.r[d] = self.read((x+y) & MASK)
        elif op in (0x20, 0x21, 0x24, 0x25, 0x26, 0x28, 0x2a, 0x2c, 0x2f):
            if op == 0x20: result = (x+y) & MASK
            elif op in (0x21, 0x2f): result = (x-y) & MASK
            elif op == 0x24: result = (x*y) & MASK
            elif op == 0x25: result = x//y
            elif op == 0x26: result = x % y
            elif op == 0x28: result = x & y
            elif op == 0x2a: result = x | y
            else: result = x ^ y
            if op in (0x21, 0x2f): self.sub_flags(x, y, result)
            elif op == 0x20:
                self.flags = (result == 0, bool(result & 0x80000000),
                              bool(~(x ^ y) & (x ^ result) & 0x80000000), x+y > MASK)
            else: self.flags = (result == 0, bool(result & 0x80000000), False, False)
            if op != 0x2f: self.r[d] = result
        elif op == 0x30: self.pc = immediate
        elif op == 0x31:
            if self.condition((word >> 20) & 15): self.pc = immediate
        elif op == 0x34: self.push(self.pc); self.pc = immediate
        elif op == 0x36: self.pc = self.pop()
        elif op == 0x37: self.syscall()
        elif op == 0x40: self.push(x)
        elif op == 0x41: self.r[d] = self.pop()
        elif op == 0x42: self.sp = x
        elif op == 0x43: self.r[d] = self.sp
        elif op == 0x44: self.sp = self.fp
        elif op == 0x45: self.fp = self.sp
        elif op == 0x46: self.push(self.fp)
        elif op == 0x47: self.fp = self.pop()
        elif op == 0x48: self.fp = x
        elif op == 0x49: self.r[d] = self.fp
        else: raise AssertionError(f'unsupported opcode {op:x} at {self.pc-1:x}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('binary')
    parser.add_argument('--frames', type=int, default=100)
    parser.add_argument('--route', choices=['rectangle', 'food'], default='rectangle')
    args = parser.parse_args()
    cpu = CPU(args.binary, args.frames, args.route)
    while cpu.frames < args.frames:
        cpu.step()
    print(f'PASS: Aurora BIN, {cpu.frames} consistent frames, '
          f'score={cpu.score}, {cpu.sequence} compiled callbacks, {cpu.steps} instructions')
