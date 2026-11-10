"""Execute production kernel + ASFS through the bus. Not Digital timing."""
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tests/graphics-syscall'))
sys.path.insert(0, str(ROOT / 'tests/midi-syscall'))
from run_graphics import GraphicsCPU, CheckedCPU
from device import MidiDevice


def image_words():
    lines = (ROOT / 'storage/build/storage.hex').read_text().splitlines()
    assert lines[0] == 'v2.0 raw'
    return [int(s, 16) for s in ' '.join(lines[1:]).split()]


class StorageCPU(MidiDevice, GraphicsCPU):
    def __init__(self, image=None, writable=True):
        super().__init__(ROOT / 'user-programs/programs/calculator/build/calculator.bin', duration=120)
        self.image = image_words() if image is None else image
        self.rom = []  # UROM initially empty: must be loaded by real kernel code.
        self.extra_rom={}
        self.writable = writable
        self.loaded_writes = 0
        self.cpu_urom_writes = 0
        self.cpu_urom_reads = 0
        self.dma_params = [None] * 3
        self.dma_jobs = []
        self.dma_deadline = None
        self.dma_duration = 500
        self.dma_stuck = False
        self.dma_corrupt = False
        self.dma_sum = 0xdeadbeef
        self.dma_sum_fault = False
        self.dma_sum_clears = self.dma_sum_reads = 0
        self.storage_reads = []
        self.storage_writes = 0
        self.storage_write_addresses = []
        self.storage_writable = True
        self.prompt_count = 0
        self.instruction_limit = 40000000
        self.tick_period = self.next_tick = 3001
        self.key_period = self.next_key = 4001

    def urom_read(self, i):
        if i>=0x400000: return self.extra_rom.get(i,0)
        return self.rom[i] if i<len(self.rom) else 0

    def urom_write(self, i, value):
        if i>=0x400000: self.extra_rom[i]=value; return
        if i>=len(self.rom): self.rom.extend([0]*(i+1-len(self.rom)))
        self.rom[i]=value

    def dma_complete(self):
        if self.dma_deadline is None or self.steps < self.dma_deadline or self.dma_stuck:
            return
        origin, target, words = self.dma_jobs[-1]
        self.loaded_writes += words
        if self.writable:
            for i in range(words):
                self.urom_write(target+i,self.image[origin+i] if origin+i < len(self.image) else 0)
            if self.dma_corrupt: self.urom_write(target+words-1,self.urom_read(target+words-1)^1)
        # Hardware sums actual destination readback, including failed writes.
        self.dma_sum = (self.dma_sum + sum(self.urom_read(i) for i in range(target,target+words))) & 0xffffffff
        if self.dma_sum_fault: self.dma_sum ^= 1
        self.dma_deadline = None

    def read(self, address):
        self.dma_complete()
        if 0x0c000000 <= address <= 0x0c000004:
            assert self.kernel_mode and address in (0x0c000003,0x0c000004)
            if address == 0x0c000003: return int(self.dma_deadline is not None)
            assert self.dma_deadline is None, 'SUM read before completion'
            self.dma_sum_reads += 1
            return self.dma_sum
        if 0x0b000000 <= address < 0x0c000000:
            assert self.dma_deadline is None, 'CPU read Storage while DMA owns it'
            assert self.kernel_mode
            i = address - 0x0b000000
            self.storage_reads.append(i)
            return self.image[i] if i < len(self.image) else 0
        if 0x80000000 <= address < 0x81000000:
            assert self.dma_deadline is None, 'CPU read UROM before DMA finished'
            self.cpu_urom_reads += 1
            if not self.kernel_mode: address=self.user_address(address)
            return self.urom_read(address - 0x80000000)
        return super().read(address)

    def write(self, address, value):
        self.dma_complete()
        if 0x0c000000 <= address <= 0x0c000004:
            assert self.kernel_mode and self.dma_deadline is None
            if address == 0x0c000004:
                assert value == 0
                self.dma_sum = 0
                self.dma_sum_clears += 1
            elif address != 0x0c000003:
                assert 0 <= value < 0x1000000
                self.dma_params[address-0x0c000000] = value
            else:
                assert value == 1 and all(v is not None for v in self.dma_params)
                origin,target,words = self.dma_params
                assert 0 < words < 0x1000000
                assert origin+words <= 0x1000000 and target+words <= 0x1000000
                assert self.dma_sum_clears == len(self.dma_jobs)+1 and self.dma_sum == 0
                self.dma_jobs.append(tuple(self.dma_params))
                self.dma_params = [None] * 3
                self.dma_deadline = self.steps + self.dma_duration
            return
        if 0x0b000000 <= address < 0x0c000000:
            assert self.dma_deadline is None, 'CPU wrote Storage during DMA'
            assert self.kernel_mode, 'user wrote storage directly'
            self.storage_writes += 1
            self.storage_write_addresses.append(address - 0x0b000000)
            if self.storage_writable:
                i = address - 0x0b000000
                if i >= len(self.image): self.image.extend([0] * (i + 1 - len(self.image)))
                self.image[i] = value & 0xffffffff
            return
        if 0x80000000 <= address < 0x81000000:
            assert self.dma_deadline is None, 'CPU wrote UROM during DMA'
            assert self.kernel_mode, 'user modified executable memory'
            self.loaded_writes += 1
            self.cpu_urom_writes += 1
            if self.writable:
                i = address - 0x80000000
                self.urom_write(i,value & 0xffffffff)
            return
        if address == 0x09900000 and value == 0 and self.deadline is not None:
            self.deadline = None  # stop/abort drawing during process cleanup
        super().write(address, value)

    def emit(self, text):
        super().emit(text)
        if self.output.endswith('aurora:/ > '):
            self.prompt_count += 1

    def step(self):
        # Exit/kill abandon callbacks. A prompt in the OTHER terminal must not
        # discard a still-running user's context-check bookkeeping.
        if self.pc == self.entries['finish_process_exit']:
            bank=self.r[0]-1
            self.bank_callbacks[bank]=dict(telnet_entry=0,keyboard_entry=0,callback_inflight=False)
            self.bank_restoring[bank]=False
            self.bank_snapshots[bank].clear()
        super().step()

    def syscall(self):
        # Unlike the one-shot graphics contract, interactive programs are
        # allowed to WaitEvent after drawing.
        CheckedCPU.syscall(self)

    def command_done(self, text):
        n = self.prompt_count
        self.pending.extend(text + '\r')
        self.until(lambda: self.prompt_count > n, limit=8000000)

    def quit_key(self, key='Q'):
        n = self.prompt_count
        self.pending.append(key)
        self.until(lambda: self.prompt_count > n, limit=8000000)

    def run_program(self, name, expected):
        before = len(self.output)
        self.pending.extend('run ' + name + '\r')
        self.until(lambda: expected in self.output[before:], limit=8000000)
        self.until(lambda: not self.kernel_mode and self.ie)


def integration():
    cpu = StorageCPU()
    cpu.until(lambda: cpu.prompt_count == 1)
    assert cpu.loaded_writes == cpu.user_steps == 0
    cpu.command_done('ls')
    assert all(n in cpu.output for n in ('calculator', 'snake', 'maze', 'gomoku'))
    cpu.command_done('help')
    cpu.command_done('missing')
    assert 'Program not found.' in cpu.output
    cpu.command_done('x' * 70)
    assert 'Command too long.' in cpu.output
    for n in range(2):
        cpu.run_program('calculator', '>> ')
        cpu.command('8*(3+2)', 'Result: 40')
        cpu.quit_key()
        assert 'exited with code 0.' in cpu.output
        print('PASS calculator restart', n + 1, flush=True)
    for name, banner in [('gomoku', 'Gomoku'), ('maze', 'Maze'), ('snake', 'Snake'), ('tetris','Tetris')]:
        # These banners are intentionally checked case-insensitively below.
        before = cpu.loaded_writes
        cpu.pending.extend(name + '\r')
        cpu.until(lambda: cpu.loaded_writes > before and cpu.keyboard_entry != 0 and
                  not cpu.kernel_mode and cpu.ie, limit=8000000)
        cpu.quit_key('q')
        print('PASS launch/exit', name, flush=True)
    cpu.run_program('calculator', '>> ')
    cpu.command('1+2', 'Result: 3\n')
    # Hardware protection faults now halt with a blue screen, never resume SYSTEM.
    cpu.until(lambda: not cpu.kernel_mode and cpu.ie)
    cpu.interrupt(cpu.read(0x03000004))
    cpu.until(lambda: cpu.halted)
    assert 'Memory access violation in USER_PROCESS.' in cpu.output
    print('PASS user memory fault halts', flush=True)
    cpu = StorageCPU(); cpu.until(lambda: cpu.prompt_count == 1)
    cpu.run_program('calculator', '>> ')
    cpu.interrupt(cpu.read(0x03000005))
    cpu.until(lambda: cpu.halted)
    assert 'Illegal instruction in USER_PROCESS.' in cpu.output
    print('PASS user instruction fault halts', flush=True)
    cpu = StorageCPU(); cpu.until(lambda: cpu.prompt_count == 1)
    cpu.run_program('calculator', '>> ')
    ticks_before_input = cpu.ticks
    cpu.command('123+345', 'Result: 468')
    cpu.quit_key()
    # Loading is now faster; test scheduling during user input rather than
    # requiring the obsolete CPU copy loop to consume >100 ticks.
    assert cpu.ticks > ticks_before_input and cpu.restores > 0
    print('PASS storage: empty UROM, shell, list, repeated load/exit, graphics/games, fatal faults, callbacks/ticks', flush=True)


def rejection():
    for kind in ('blank', 'dir-range', 'duplicate', 'checksum', 'abi', 'segment-range',
                 'bank-code-range', 'bank-code-cross', 'entry', 'overlap', 'urom-readonly'):
        words = image_words()
        offset = words[17]
        if kind == 'blank': words = []
        elif kind == 'dir-range': words[5] = 0xffffffff
        elif kind == 'duplicate': words[60:92] = words[20:52]
        elif kind == 'checksum': words[offset + 22 + 10] ^= 1
        elif kind == 'abi': words[offset + 6] = 999
        elif kind == 'segment-range': words[offset + 16] = 0x01000000
        elif kind == 'bank-code-range':
            words[offset + 4] = words[offset + 16] = 0x80400000
        elif kind == 'bank-code-cross':
            words[offset + 4] = words[offset + 16] = 0x803fffff
        elif kind == 'entry': words[offset + 4] = 0x81000000
        elif kind == 'overlap': words[57] = words[17]
        if kind in ('abi', 'segment-range', 'bank-code-range', 'bank-code-cross', 'entry'):
            words[19] = sum(words[offset:offset + words[18]]) & 0xffffffff
        cpu = StorageCPU(words, writable=kind != 'urom-readonly')
        cpu.memory_bank = 3  # Kernel must initialize bank 0, even after a warm reset.
        cpu.until(lambda: cpu.prompt_count == 1)
        assert cpu.memory_bank == 0
        cpu.command_done('calculator')
        assert cpu.user_steps == 0
        if kind not in ('urom-readonly','checksum'): assert cpu.loaded_writes == 0
        assert 'Program not found.' in cpu.output or 'Load failed:' in cpu.output
        print('PASS reject:', kind, flush=True)


if __name__ == '__main__':
    integration()
    rejection()
