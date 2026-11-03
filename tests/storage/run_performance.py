"""Instruction-count regression and poisoned-RAM safety; not wall-clock/Digital."""
import struct
from collections import Counter
from run_storage import ROOT,StorageCPU,image_words
from run_files import editor

class ProfileCPU(StorageCPU):
    def __init__(self):
        super().__init__(); self.calls=Counter(); self.batches=[]
    def read(self,a):
        if 0x81000000<=a<0x82000000 and a not in self.mem:
            return 0xdeadbeef  # noinit buffers MUST NOT depend on zero RAM
        return super().read(a)
    def syscall(self):
        if not self.kernel_mode:
            self.calls[self.r[0]]+=1
            if self.r[0]==9: self.batches.append(self.r[2])
        super().syscall()

c=ProfileCPU(); c.until(lambda:bool(c.output)); first=c.steps
assert first<100,'first boot indication regressed'
c.until(lambda:c.prompt_count==1); boot=c.steps
assert 'BIOS startup:' in c.output and '3 seconds' not in c.output
c.pending.extend('quickrun txt-viewer\r')
c.until(lambda:not c.kernel_mode and c.ie,limit=8000000); start=c.steps
c.until(lambda:c.output.endswith('txt > '),limit=4000000); startup=c.steps-start
assert startup<103780,'viewer regressed against pre-optimization baseline'
assert c.calls[6]==0 and 64 in c.batches,'help should use bounded Write batches'
n=len(c.batches); editor(c,'open docs/welcome.txt','Welcome to Aurora-32!')
assert c.batches[n:].count(64)>=3,'body lost 64-word batching'
editor(c,'clear'); editor(c,'append bounded-prefix'); editor(c,'show','bounded-prefix\n')
editor(c,'discard'); n=c.prompt_count; c.pending.extend('q\r'); c.until(lambda:c.prompt_count>n)
c.run_program('txt-viewer','txt > ')
editor(c,'show','Open a text file first.')
print(f'PASS performance: first log {first} instructions (baseline 30026); boot prompt {boot}; viewer startup {startup} (baseline 103780)',flush=True)
print('PASS viewer: poisoned unused RAM, batched help/filenames/body, restart does not expose old draft',flush=True)

words=image_words(); raw=(ROOT/'tests/storage/build/console_contract.bin').read_bytes()
code=list(struct.unpack('<'+'I'*(len(raw)//4),raw))
exe=[0x41455845,1,16,22+len(code),0x80000000,3,2,1,16,6,1,0,0,0,0,0,
     0x80000000,22,len(code),len(code),1,0]+code
words[17]=len(words); words[18]=words[53]=len(exe); words[19]=sum(exe)&0xffffffff
words.extend(exe); words[3]=len(words)
c=StorageCPU(words); c.until(lambda:c.prompt_count==1); c.command_done('quickrun calculator')
assert 'CONSOLE CONTRACT PASS' in c.output and 'CONSOLE FAIL' not in c.output
print('PASS output ABI: null/USTACK/end boundary, zero/oversize Write, 4096-word unterminated string cap',flush=True)
