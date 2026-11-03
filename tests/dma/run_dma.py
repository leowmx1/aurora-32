"""Run standalone ROM BIN with an ideal DMA; NOT a Digital timing simulation."""
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tests/kernel-events'))
from run_examples import DemoCPU

BIN = ROOT / 'tests/dma/build/dma_test.bin'
STORAGE, UROM, DMA = 0x0b000000, 0x80000000, 0x0c000000
CASES = [(0, 0, 1), (1, 63, 2), (31, 255, 17),
         (251, 4093, 257), (4096, 8191, 4096)]


class DmaCPU(DemoCPU):
    def __init__(self, fault=None, immediate=False, zeros=False):
        super().__init__('dma', BIN)
        raw = BIN.read_bytes()
        self.kernel_rom = struct.unpack('<' + 'I' * (len(raw) // 4), raw)
        self.storage = [0 if zeros else ((i * 2654435761) ^ 0xa5c37e19) & 0xffffffff
                        for i in range(8193)]
        self.urom = {}
        self.params = [None] * 3
        self.deadline = None
        self.jobs = []
        self.fault, self.immediate = fault, immediate
        self.stat_writes = []
        self.sum = 0xdeadbeef
        self.clears = 0

    def complete(self):
        if self.deadline is None or self.steps < self.deadline or self.fault == 'stuck':
            return
        origin, target, size = self.jobs[-1]
        if self.fault != 'noop':
            count = size - 1 if self.fault == 'short' else size
            for i in range(count):
                self.urom[target + i] = self.storage[origin + i + (self.fault == 'offset')]
            if self.fault == 'extra':
                self.urom[target + size] = 0
            if self.fault == 'source':
                self.storage[origin] ^= 1
        self.sum = (self.sum + sum(self.urom.get(target+i,0) for i in range(size))) & 0xffffffff
        if self.fault == 'sum': self.sum ^= 1
        self.deadline = None

    def read(self, address):
        self.complete()
        if STORAGE <= address < STORAGE + 0x1000000:
            return self.storage[address - STORAGE]
        if UROM <= address < UROM + 0x1000000:
            return self.urom.get(address - UROM, 0)
        if DMA <= address < DMA + 5:
            assert address in (DMA+3,DMA+4)
            if address == DMA+4: return self.sum
            return int(self.deadline is not None and self.fault != 'early')
        return super().read(address)

    def write(self, address, value):
        self.complete()
        assert not STORAGE <= address < STORAGE + 0x1000000, 'test must not write EEPROM'
        if UROM <= address < UROM + 0x1000000:
            assert self.deadline is None, 'CPU must not modify active DMA destination'
            self.urom[address - UROM] = value & 0xffffffff
        elif DMA <= address < DMA + 5:
            assert self.kernel_mode and not self.ie and self.deadline is None
            if address == DMA+4:
                assert value == 0
                self.sum = 0; self.clears += 1
            elif address == DMA + 3:
                assert value == 1 and all(v is not None for v in self.params)
                assert tuple(self.params) == CASES[len(self.jobs)]
                assert all(0 <= v < 0x1000000 for v in self.params)
                assert self.clears == len(self.jobs)+1 and self.sum == 0
                self.jobs.append(tuple(self.params))
                self.stat_writes.append(value)
                self.deadline = self.steps + (0 if self.immediate else 500)
                self.params = [None] * 3
            else:
                self.params[address - DMA] = value
        else:
            super().write(address, value)


def run(fault=None, immediate=False, zeros=False):
    cpu = DmaCPU(fault, immediate, zeros)
    original = cpu.storage[:]
    while not (cpu.output.endswith('LDMA ALL PASS (5 tests).\n') or
               ('FAIL ' in cpu.output and cpu.output.endswith('\n'))):
        assert cpu.steps < 3000000, cpu.output
        cpu.step()
    if fault:
        assert 'FAIL ' in cpu.output and len(cpu.jobs) == 1, cpu.output
        if fault == 'stuck': assert 'TIMEOUT' in cpu.output
    else:
        assert 'LDMA ALL PASS' in cpu.output and cpu.jobs == CASES, cpu.output
        assert cpu.output.count('PASS copy+guards+sum') == 5 and cpu.clears == 5
        assert cpu.storage == original
    jobs = cpu.jobs[:]
    for _ in range(1000): cpu.step()
    assert cpu.jobs == jobs and not cpu.ie and cpu.kernel_mode
    print(f'PASS DMA test BIN: fault={fault}, immediate={immediate}, zero-source={zeros}')
    if fault is None and not immediate and not zeros:
        print(cpu.output)


if __name__ == '__main__':
    run()
    run(immediate=True)
    run(zeros=True)
    for fault in ('noop', 'short', 'extra', 'offset', 'source', 'early', 'stuck', 'sum'):
        run(fault)
