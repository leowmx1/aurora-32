"""Foreground -> background using emitted BIN and real kernel, not Digital."""
import struct
from run_storage import StorageCPU, ROOT, image_words

def net(c): return ''.join(map(chr,c.network_out)).replace('\r','')
def remote(c, command):
    n=net(c).count('aurora:remote:/ > ')
    c.network_in.extend(map(ord,command+'\r\n'))
    c.until(lambda:net(c).count('aurora:remote:/ > ')>n,limit=8000000)

class BackgroundCPU(StorageCPU):
    def __init__(self, words):
        self.sleeps=[]
        self.muted_banks=set()
        self.output_checks=0
        self.request=None
        super().__init__(words)
    def step(self):
        if self.pc==self.entries['finish_process_exit']:
            self.muted_banks.discard(self.r[0]-1)
        if self.pc==self.entries['kernel_syscall_dispatch']:
            self.request=(self.r[0],self.memory_bank,len(self.output),len(self.network_out))
        if self.pc==self.entries['finish_user_syscall'] and self.request:
            request,bank,local_count,remote_count=self.request
            if request in (1,5,6,9) and bank in self.muted_banks:
                assert len(self.output)==local_count and len(self.network_out)==remote_count, \
                    ('background syscall wrote to terminal',bank,request)
                self.output_checks+=1
            if request==36 and self.r[0]==0: self.muted_banks.add(bank)
            self.request=None
        super().step()
    def syscall(self):
        if self.r[0]==29: self.sleeps.append((self.memory_bank,self.r[1]))
        super().syscall()

def silent(c):
    text=c.output+net(c)
    for forbidden in ('BG READY','DETACHED context OK','BACKGROUND STDOUT',
                      '~','987654321','BACKGROUND FAIL','exited with code 1.'):
        assert forbidden not in text, text[-3000:]

words=image_words()
raw=(ROOT/'tests/storage/build/background_contract.bin').read_bytes()
code=list(struct.unpack('<'+'I'*(len(raw)//4),raw))
exe=[0x41455845,1,16,22+len(code),0x80000000,3,2,1,16,6,1,0,0,0,0,0,
     0x80000000,22,len(code),len(code),1,0]+code
at=16+40 # replace guess-number only in test memory
words[at+1]=len(words); words[at+2]=words[at+37]=len(exe)
words[at+3]=sum(exe)&0xffffffff
words[at+4:at+36]=list(map(ord,'detach'))+[0]*26
words.extend(exe); words[3]=len(words)
c=BackgroundCPU(words); c.until(lambda:c.prompt_count==1 and net(c).endswith('aurora:remote:/ > '))
c.muted_banks.add(0)
c.command_done('detach &'); c.until(lambda:(0,1234) in c.sleeps); silent(c)
c.run_program('detach','FG READY')
c.pending.append('b'); c.until(lambda:'BACKGROUND BUSY\n' in c.output)
assert c.prompt_count==2,'failed Background must retain foreground'
remote(c,'kill 1')
start=len(c.output); jobs=len(c.dma_jobs); n=c.prompt_count
c.pending.append('b'); c.until(lambda:(1,2345) in c.sleeps); silent(c)
c.until(lambda:c.prompt_count>n)
assert len(c.dma_jobs)==jobs,'Background must not reload/migrate'
c.command_done('ps')
assert '2    1    local    BG' in c.output
c.run_program('calculator','>> ')
c.command('8*(3+2)','Result: 40')
remote(c,'kill 2')
assert 'name=detach slot=2 bank=1 local background' in c.output
c.command('7*9','Result: 63'); c.quit_key()
assert 'BACKGROUND FAIL' not in c.output+net(c)
print('PASS Background: callback/defer rejection, busy atomicity, same bank/stack, idempotence, released input, live FG and exit notice',flush=True)

# Remote detach also returns its own prompt, not the local one; CRLF consumed.
start=len(net(c)); c.network_in.extend(map(ord,'detach\r\n'))
c.until(lambda:'FG READY' in net(c)[start:])
n=net(c).count('aurora:remote:/ > '); c.network_in.extend(map(ord,'b\r\n'))
c.until(lambda:(0,2345) in c.sleeps); silent(c)
c.until(lambda:net(c).count('aurora:remote:/ > ')>n)
c.command_done('ps'); assert 'remote   BG' in c.output
c.command_done('kill 1')
assert 'name=detach slot=1 bank=0 remote background' in net(c)
print('PASS remote Background: ownership, prompt, CRLF handoff and kill',flush=True)

# With an existing background task, compiler must keep foreground and finish.
c.sleeps.clear()
c.muted_banks.add(0)
c.command_done('detach &'); c.until(lambda:(0,1234) in c.sleeps); silent(c)
c.run_program('compiler','Source > ')
start=len(c.output); c.pending.extend('tinyc/hello.c\r')
c.until(lambda:'Output > ' in c.output[start:])
n=c.prompt_count; c.pending.extend('busycompile\ry\r')
c.until(lambda:c.prompt_count>n,limit=20000000)
assert 'Background unavailable; continuing in foreground.' in c.output[start:]
assert 'Compiled ' in c.output[start:] and 'name=compiler slot=2 bank=1 local foreground' in c.output[start:]
print('PASS compiler busy-background fallback stays foreground and saves executable',flush=True)
silent(c)
assert c.output_checks>=40,c.output_checks
print('PASS silent local/remote background stdout: scalar, string, Write, buffered batches, normal returns and invalid arguments',flush=True)
