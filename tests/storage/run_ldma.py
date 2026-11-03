"""Production loader with ideal LDMA: actual BIN, not Digital timing."""
import struct
from collections import Counter
from run_storage import StorageCPU, image_words, ROOT


for quick, duration in ((False, 50000), (True, 0)):
    c = StorageCPU(); c.dma_duration = duration
    c.until(lambda:c.prompt_count == 1)
    c.command_done('verify calculator')
    assert not c.dma_jobs and c.loaded_writes == 0
    c.storage_reads.clear()
    before_urom_reads = c.cpu_urom_reads
    c.pending.extend(('quickrun ' if quick else 'run ') + 'calculator\r')
    c.until(lambda:not c.kernel_mode, limit=8000000)
    off = c.image[17]
    assert c.dma_jobs == [(off + c.image[off+17], 0, c.image[off+18])]
    assert c.cpu_urom_writes == 0 and c.storage_writes == 0
    assert c.dma_deadline is None
    assert c.dma_sum_clears == c.dma_sum_reads == 1
    origin,target,size = c.dma_jobs[0]
    reads = Counter(c.storage_reads)
    assert all(reads[i] == 0 for i in range(origin,origin+size)), 'CPU still scans source code'
    assert c.cpu_urom_reads == before_urom_reads, 'CPU still verifies UROM'
    load_output = c.output[c.output.index('Loading calculator'):]
    assert 'Check  ' not in load_output
    assert ('Verify ' in load_output) == (not quick)
    c.until(lambda:'>> ' in c.output and not c.kernel_mode and c.ie, limit=8000000)
    c.command('8*(3+2)', 'Result: 40')
    c.quit_key()
    print(f'PASS loader LDMA: quick={quick}, duration={duration}, offsets/count, no CPU UROM copy, execution/exit', flush=True)

# Multiple code segments, nonzero target; data/BSS use the CPU, not LDMA.
image = image_words()
raw = (ROOT/'user-programs/programs/calculator/build/calculator.bin').read_bytes()
code = list(struct.unpack('<'+'I'*(len(raw)//4), raw))
data = [0x12345678, 0xffffffff, 0, 0xa5a55a5a]
size = 34 + len(code) + len(data)
exe = [0x41455845,1,16,size,0x80000000,3,2,3,16,6,1,0,0,0,0,0,
       0x80000000,34,8,8,1,0,
       0x80000008,42,len(code)-8,len(code)-8,1,0,
       0x81004000,34+len(code),4,9,2,0] + code + data
origin = len(image)
image[17] = origin; image[18] = image[53] = len(exe); image[19] = sum(exe)&0xffffffff
image.extend(exe); image[3] = len(image)
c = StorageCPU(image)
for i in range(9): c.mem[0x81004000+i] = 0xdeadbeef
c.until(lambda:c.prompt_count == 1)
c.pending.extend('run calculator\r')
c.until(lambda:not c.kernel_mode, limit=8000000)
assert c.rom == code
assert [c.mem[0x81004000+i] for i in range(9)] == data + [0]*5
assert c.dma_jobs == [(origin+34,0,8),(origin+42,8,len(code)-8)]
assert c.cpu_urom_writes == 0
assert c.dma_sum_clears == c.dma_sum_reads == 2
print('PASS loader LDMA: segmented UROM, nonzero destination, CPU URAM data + BSS', flush=True)

# A bad later segment must be rejected before even the first DMA is submitted.
for bad_dest in (0x01004000, 0x80000004):
    broken = image[:]
    broken[origin+28] = bad_dest
    broken[19] = sum(broken[origin:origin+len(exe)]) & 0xffffffff
    c = StorageCPU(broken); c.until(lambda:c.prompt_count == 1)
    c.command_done('quickrun calculator')
    assert 'Load failed:' in c.output and not c.dma_jobs and c.loaded_writes == 0
print('PASS loader LDMA: invalid later segment rejected before any copy', flush=True)

for fault in ('corrupt', 'sum', 'tail', 'stuck', 'busy'):
    class FaultCPU(StorageCPU):
        def dma_complete(self):
            was_busy = self.dma_deadline is not None
            super().dma_complete()
            if fault == 'tail' and was_busy and self.dma_deadline is None:
                _,target,size = self.dma_jobs[-1]
                self.dma_sum = (self.dma_sum - self.rom[target+size-1]) & 0xffffffff

        def read(self, address):
            if fault == 'busy' and address == 0x0c000003: return 1
            return super().read(address)
    c = FaultCPU()
    c.dma_corrupt = fault == 'corrupt'; c.dma_stuck = fault == 'stuck'
    c.dma_sum_fault = fault == 'sum'
    c.until(lambda:c.prompt_count == 1)
    c.pending.extend('run calculator\r')
    c.until(lambda:c.halted if fault in ('stuck','busy') else c.prompt_count == 2, limit=8000000)
    assert c.user_steps == 0 and c.cpu_urom_writes == 0
    if fault in ('corrupt','sum','tail'):
        assert 'Load failed:' in c.output
        assert 'LDMA UROM checksum' in c.output
    else:
        assert 'Reset required.' in c.output
        assert len(c.dma_jobs) == (fault == 'stuck')
    print(f'PASS loader LDMA failure: {fault}, no partial user execution', flush=True)


def run_layout(executable, jobs):
    image = image_words(); origin = len(image)
    image[17] = origin; image[18] = image[53] = len(executable)
    image[19] = sum(executable)&0xffffffff
    image.extend(executable); image[3] = len(image)
    c = StorageCPU(image)
    c.until(lambda:c.prompt_count == 1)
    c.pending.extend('run calculator\r')
    c.until(lambda:not c.kernel_mode, limit=8000000)
    assert c.rom[:len(code)] == code
    assert c.dma_jobs == [(origin+src,dest,n) for src,dest,n in jobs]
    assert c.dma_sum_clears == c.dma_sum_reads == len(jobs)
    return c

# Non-code payload, gaps and trailing bytes must count exactly once; table order
# need not follow source offsets or destination order.
size = 55 + len(code)-8 + 3
exe = [0x41455845,1,16,size,0x80000000,3,2,3,16,6,1,0,0,0,0,0,
       0x80000008,55,len(code)-8,len(code)-8,1,0,
       0x81004000,34,4,9,2,0,
       0x80000000,42,8,8,1,0] + data + [0xffffffff,9,0x80000000,3] + code[:8] + [1,2,3,4,5] + code[8:] + [7,8,9]
c=run_layout(exe, [(55,8,len(code)-8),(42,0,8)])
assert [c.mem[0x81004000+i] for i in range(9)] == data+[0]*5
print('PASS readback SUM: unordered segments, header/data/gaps/tail, modulo-32 sum', flush=True)

# Legal shared source is copied twice but belongs to the file checksum once.
size=28+len(code); duplicate=len(code)+16
exe=[0x41455845,1,16,size,0x80000000,3,2,2,16,6,1,0,0,0,0,0,
     0x80000000,28,len(code),len(code),1,0,
     0x80000000+duplicate,28,8,8,1,0]+code
c=run_layout(exe, [(28,0,len(code)),(28,duplicate,8)])
assert c.rom[duplicate:duplicate+8] == code[:8]
origin = c.image[17]
reads = Counter(c.storage_reads)
assert all(reads[origin+28+i] == (1 if i<8 else 0) for i in range(len(code)))
print('PASS readback SUM: shared-source correction without full source scan', flush=True)

# Three overlapping source intervals, unordered table, distinct destinations.
size=34+len(code); duplicate=len(code)+16
exe=[0x41455845,1,16,size,0x80000000,3,2,3,16,6,1,0,0,0,0,0,
     0x80000000+duplicate,38,12,12,1,0,
     0x80000000,34,len(code),len(code),1,0,
     0x80000000+duplicate+20,42,12,12,1,0]+code
c=run_layout(exe, [(38,duplicate,12),(34,0,len(code)),(42,duplicate+20,12)])
assert c.rom[duplicate:duplicate+12] == code[4:16]
assert c.rom[duplicate+20:duplicate+32] == code[8:20]
print('PASS readback SUM: partial triple overlap and unordered ranges', flush=True)
