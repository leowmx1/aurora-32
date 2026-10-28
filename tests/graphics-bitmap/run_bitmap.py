"""Standalone kernel BIN with ideal new graphics registers; NOT Digital."""
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tests/kernel-events'))
from run_examples import DemoCPU

BASE, STAT = 0x09000000, 0x09000009
BIN = ROOT / 'tests/graphics-bitmap/build/bitmap_test.bin'
EXPECTED = [
    (0,0,160,100,0,2,0,0,0),
    (16,12,24,24,4,2,0,0,0), (48,12,24,24,3,2,0,0,0),
    (96,12,12,24,3,2,0,0,0), (108,12,12,24,5,2,0,0,0),
    (56,20,8,8,2,4,0x1f01013f,0x00010101,1),
    (104,20,8,8,2,4,0x1f01013f,0x00010101,2),
    (16,56,8,8,2,4,0xffffffff,0,1), (40,56,8,8,3,4,0,0xffffffff,1),
    (64,56,8,8,1,0,0x80000000,1,1), (84,56,5,7,1,0,1,4,1),
    (104,20,8,8,1,1,0,0,2), (96,56,24,16,3,2,0,0,0),
    (159,99,1,1,2,4,1,0,2),
]


class BitmapCPU(DemoCPU):
    def __init__(self, duration=1000, stuck=False, offset=0):
        super().__init__('bitmap', BIN)
        self.kernel_rom = self.rom  # Test itself runs at ROM 0, no OS image.
        self.duration, self.stuck, self.offset = duration, stuck, offset
        self.params = [None]*9
        self.commands, self.writes, self.snapshots = [], [], []
        self.screen = [-1]*16000
        self.deadline = None

    def complete(self):
        if self.deadline is None or self.steps < self.deadline or self.stuck: return
        x,y,w,h,c,bg,low,high,mode = self.commands[-1]
        mask = low | (high << 32)
        for dy in range(h):
            for dx in range(w):
                bit = (mask >> (dy*w+dx)) & 1 if mode else 1
                if mode == 2 and not bit: continue
                self.screen[(y+dy)*160+x+dx] = c if bit else bg
        self.snapshots.append(self.screen[:])
        self.deadline = None

    def read(self, address):
        if address == 0x08000000: return (self.offset+self.steps//20) & 0xffffffff
        if 0x09000000 <= address < 0x10000000:
            assert address == STAT, hex(address)
            self.complete()
            return int(self.deadline is not None)
        return super().read(address)

    def write(self, address, value):
        if 0x09000000 <= address < 0x10000000:
            assert self.kernel_mode and not self.ie
            assert BASE <= address <= STAT, hex(address)
            self.complete()
            assert self.deadline is None, 'parameters overwritten while busy'
            self.writes.append((address,value))
            if address == STAT:
                assert value in (0,1)
                if value == 1:
                    assert [a for a,v in self.writes[-11:]] == [STAT]+list(range(BASE,BASE+9))+[STAT]
                    assert self.writes[-11][1] == 0
                    p = tuple(self.params)
                    assert p == EXPECTED[len(self.commands)], p
                    x,y,w,h,c,bg,lo,hi,mode = p
                    assert 0 <= x < 160 and 0 <= y < 100 and 0 < w <= 160-x and 0 < h <= 100-y
                    assert mode == 0 or w*h <= 64
                    self.commands.append(p)
                    self.deadline = self.steps+self.duration
            else: self.params[address-BASE] = value
        else: super().write(address,value)

    def step(self):
        assert self.kernel_mode and not self.ie, 'test must remain kernel-only, IRQs disabled'
        assert self.read(self.pc) & 255 not in (0x37,0x39,0x3a,0xf0), 'unexpected INT/IRET/EI/HALT'
        super().step()


def reference():
    screen = [0]*16000
    def rect(x,y,w,h,c):
        for dy in range(h):
            for dx in range(w): screen[(y+dy)*160+x+dx] = c
    rect(16,12,24,24,4); rect(48,12,24,24,3)
    rect(96,12,12,24,3); rect(108,12,12,24,5)
    rect(56,20,8,8,4)
    for x in (56,104):
        for y,width in enumerate([6,1,1,5,1,1,1,0]):
            if width: rect(x,20+y,width,1,2)
    rect(16,56,8,4,2); rect(16,60,8,4,4)
    rect(40,56,8,4,4); rect(40,60,8,4,3)
    rect(71,59,1,1,1); rect(64,60,1,1,1)
    rect(84,56,1,1,1); rect(88,62,1,1,1)
    rect(96,56,24,16,3); rect(159,99,1,1,2)
    return screen


def run(duration, offset=0, stuck=False):
    cpu = BitmapCPU(duration, stuck, offset)
    marker = 'No more commands.\n' if stuck else 'Bottom-right pixel is red. No redraw.\n'
    cpu.until(lambda: marker in cpu.output)
    count = len(cpu.writes)
    end = cpu.steps+3000
    cpu.until(lambda: cpu.steps >= end)
    assert len(cpu.writes) == count and not cpu.ie and cpu.ticks == 0
    if stuck:
        assert 'TIMEOUT at job 0.' in cpu.output and 'DONE' not in cpu.output
        assert len(cpu.commands) == 1
    else:
        assert cpu.commands == EXPECTED and cpu.screen == reference()
        assert cpu.snapshots[10] == cpu.snapshots[11], 'transparent zero must preserve backing'
        assert count == 1+11*len(EXPECTED)
    print(f'PASS bitmap kernel BIN: duration={duration}, offset={offset:#x}, stuck={stuck}', flush=True)
    return cpu


if __name__ == '__main__':
    cpu = run(0)
    run(1000, 0xfffffff0)
    run(1000, stuck=True)
    if '--preview' in sys.argv:
        from PIL import Image
        colors = [(255,255,255),(0,0,0),(255,0,0),(0,255,0),(0,0,255),(255,255,0)]
        im = Image.new('RGB',(160,100))
        im.putdata([colors[p] for p in cpu.screen])
        im.resize((960,600), Image.Resampling.NEAREST).save(BIN.with_name('preview.png'))
