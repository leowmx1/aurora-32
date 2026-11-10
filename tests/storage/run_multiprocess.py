"""Four real user contexts/banks with production kernel. Ideal ISA, not Digital."""
import struct
from run_storage import StorageCPU, image_words, ROOT

PROMPT='aurora:remote:/ > '
def net(c): return ''.join(map(chr,c.network_out)).replace('\r','')
def remote(c,cmd,expected):
    at=len(net(c)); n=net(c).count(PROMPT)
    c.network_in.extend(map(ord,cmd+'\r\n'))
    c.until(lambda:net(c).count(PROMPT)>n,limit=8000000)
    assert expected in net(c)[at:],net(c)[at:]
def local(c,cmd,expected):
    at=len(c.output); c.command_done(cmd)
    assert expected in c.output[at:],c.output[at:]

def fixture_image():
    image=image_words()
    raw=(ROOT/'tests/storage/build/multiprocess_contract.bin').read_bytes()
    code=list(struct.unpack('<'+'I'*(len(raw)//4),raw))
    exe=[0x41455845,1,16,22+len(code),0x80000000,3,2,1,16,6,1,0,0,0,0,0,
         0x80000000,22,len(code),len(code),1,0]+code
    # Use two existing directory entries, no changes to production storage.hex.
    for i,name in ((0,'probe'),(1,'worker')):
        at=16+i*40
        image[at+1]=len(image); image[at+2]=image[at+37]=len(exe)
        image[at+3]=sum(exe)&0xffffffff
        image[at+4:at+36]=list(map(ord,name))+[0]*(32-len(name))
        image.extend(exe)
    image[3]=len(image)
    return image

class MultiCPU(StorageCPU):
    def __init__(self):
        self.time_offset=0
        self.bank_steps=[0]*4
        self.bank_last=[None]*4
        self.sleep_banks=set()
        super().__init__(fixture_image())
        self.dma_duration=25000  # Existing users must not fetch during DMA.
    def read(self,a):
        if a==0x08000000: return ((self.steps//20)+self.time_offset)&0xffffffff
        return super().read(a)
    def syscall(self):
        if self.r[0]==29: self.sleep_banks.add(self.memory_bank)
        super().syscall()
    def step(self):
        if not self.kernel_mode:
            self.bank_steps[self.memory_bank]+=1
            self.bank_last[self.memory_bank]=(hex(self.pc),hex(self.sp),hex(self.fp))
        super().step()
    def until(self,predicate,limit=2000000):
        try: super().until(predicate,limit)
        except AssertionError:
            print('DEBUG pc/bank/steps/stacks:',hex(self.pc),self.memory_bank,self.bank_steps,
                  [hex(x) for x in self.stack],[hex(x) for x in self.frame_pointer],flush=True)
            print('DEBUG remote:',net(self)[-1500:],flush=True)
            print('DEBUG last user PCs:',self.bank_last,flush=True)
            raise

c=MultiCPU(); c.until(lambda:c.prompt_count==1 and net(c).endswith(PROMPT))
local(c,'worker &','background')
c.until(lambda:0 in c.sleep_banks)
remote(c,'run worker &','background')
c.until(lambda:1 in c.sleep_banks)
assert 'BG READY' not in c.output+net(c)
local(c,'worker &','Terminal slot busy.')
remote(c,'worker &','Terminal slot busy.')
local(c,'ps','worker')
assert '1    0    local    BG' in c.output and '2    1    remote   BG' in c.output
# Start foreground instances of exactly the same code in banks 2 and 3.
c.run_program('probe','FG READY')
start=len(net(c)); c.network_in.extend(map(ord,'probe\r\n'))
c.until(lambda:'FG READY' in net(c)[start:],limit=8000000)
assert [job[1] for job in c.dma_jobs]==[0,0x400000,0x800000,0xc00000]
assert all(c.bank_steps) and not c.halted
local_at=len(c.output); remote_at=len(net(c))
c.pending.extend('1234567890'); c.network_in.extend(map(ord,'ABCDEFGHIJ'))
c.until(lambda:'[0]\n' in c.output[local_at:] and '[J]\n' in net(c)[remote_at:],limit=8000000)
assert '[A]' not in c.output[local_at:] and '[1]' not in net(c)[remote_at:]
c.until(lambda:c.restores>=20)
before=c.bank_steps[:]; end=c.steps+120000
c.until(lambda:c.steps>=end)
assert all(c.bank_steps[i]>before[i] for i in range(4)),c.bank_steps
print('PASS four banks: LDMA exclusion, globals, nested USP/UFP frames, sleep/tick, full callback registers/flags, isolated ports',flush=True)

# Single GPU/MIDI owner; a background exit must not stop the foreground owner.
at=len(c.output); c.pending.extend('gm')
c.until(lambda:'GPU=0\n' in c.output[at:] and 'MIDI=0\n' in c.output[at:])
at=len(net(c)); c.network_in.extend(map(ord,'gm'))
c.until(lambda:'GPU=-9\n' in net(c)[at:] and 'MIDI=-9\n' in net(c)[at:])
screen=c.screen[:]
# Both foregrounds remain occupied. Natural BG exits MUST print immediately.
prompts=c.prompt_count
local_at=len(c.output); remote_at=len(net(c)); c.time_offset+=2000000
c.until(lambda:'name=worker slot=1 bank=0 local background' in c.output[local_at:] and
        'name=worker slot=2 bank=1 remote background' in net(c)[remote_at:],limit=8000000)
assert 'exited with code 17.' in c.output[local_at:] and 'exited with code 17.' in net(c)[remote_at:]
assert c.screen==screen
assert c.prompt_count==prompts,c.prompt_count
assert 'aurora:/ > ' not in c.output[local_at:]
assert PROMPT not in net(c)[remote_at:]
at=len(c.output); c.pending.append('z'); c.until(lambda:'[z]\n' in c.output[at:])
at=len(net(c)); c.network_in.append(ord('Z')); c.until(lambda:'[Z]\n' in net(c)[at:])
print('PASS background exit notifications while both foregrounds remain active; graphics owner survives',flush=True)

# Release local foreground; local ps lists the other running process and exits.
c.pending.append('q'); c.until(lambda:c.output.endswith('aurora:/ > '),limit=8000000)
local(c,'ps','remote   FG')
assert 'worker / exit=17' in c.output and 'probe / exit=0' in c.output
local(c,'rm docs/welcome.txt','File operation failed: -5')
at=len(net(c)); c.network_in.extend(map(ord,'gm'))
c.until(lambda:'GPU=0\n' in net(c)[at:] and 'MIDI=0\n' in net(c)[at:])
local(c,'kill 4','User process terminated.')
c.until(lambda:net(c).endswith(PROMPT),limit=8000000)
assert 'name=probe slot=4 bank=3 remote foreground' in net(c)
assert 'exited with code 137.' in net(c)
# Reuse bank zero after callback + process teardown; stale subscriptions cannot leak.
c.run_program('probe','FG READY'); c.pending.append('q')
c.until(lambda:c.output.endswith('aurora:/ > '),limit=8000000)
assert 'MULTIPROCESS FAIL' not in c.output+net(c)
assert all(not q for q in c.bank_snapshots)
assert not any(s['callback_inflight'] for s in c.bank_callbacks)
print('PASS ps names/slots/banks/roles/exit codes, kill slot 4, resource handoff and bank reuse',flush=True)

# Production example: Sleep-based worker exits naturally while calculator is
# still interactive. It must be usable without the test-only fixture image.
p=StorageCPU(); p.until(lambda:p.prompt_count==1 and net(p).endswith(PROMPT))
local(p,'process-demo &','background')
p.run_program('calculator','>> ')
p.command('8*(3+2)','Result: 40')
p.until(lambda:'name=process-demo slot=1 bank=0 local background' in p.output,limit=4000000)
p.command('7*9','Result: 63')
p.quit_key()
assert 'context check failed' not in p.output
assert 'Process demo: background task' not in p.output
print('PASS production process-demo & natural exit beside live calculator',flush=True)
