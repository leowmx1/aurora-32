"""Real kernel: optional Background, CRLF/default choice, concurrent FS reads."""
from run_compiler import compile_source, run_generated, process_exited, entry
from run_storage import StorageCPU

SOURCE='int main() { print(42); return 0; }'
for answer, role in [('N','foreground'),('Y','background'),('','background'),('bad\rN','foreground')]:
    c=compile_source(SOURCE,background_answer=answer)
    assert 'Run in background? [Y/n] > ' in c.output
    if role=='foreground':
        assert 'Compiler running in foreground.' in c.output
    else:
        assert 'Compiler running in background.' not in c.output
        assert 'Compiling...' not in c.output and 'Compiled ' not in c.output
    assert ' local '+role+'\n\n' in c.output
    if answer.startswith('bad'): assert 'Enter Y or N' in c.output
    run_generated(c,expected='42\n')
print('PASS compiler choice: Y/N, default Enter, invalid retry, compiled output',flush=True)

class ObservedCPU(StorageCPU):
    def __init__(self):
        super().__init__(); self.fs_request=0; self.fs_busy=[]; self.read_calls=0
    def step(self):
        if self.pc==self.entries['kernel_syscall_dispatch']:
            self.fs_request=self.r[0]
            if self.fs_request==32: self.read_calls+=1
        if self.pc==self.entries['finish_user_syscall'] and self.r[0]==0xfffffffb:
            if 30<=self.fs_request<=35: self.fs_busy.append(self.fs_request)
        super().step()

def net(c): return ''.join(map(chr,c.network_out)).replace('\r','')
REMOTE='aurora:remote:/ > '
c=ObservedCPU(); c.until(lambda:c.prompt_count==1 and net(c).endswith(REMOTE))
c.run_program('compiler','Source > ')
c.pending.extend('tinyc/compiler.c\r')
c.until(lambda:c.read_calls>=1)
for _ in range(12):
    n=net(c).count(REMOTE); c.network_in.extend(map(ord,'ls\r\n'))
    c.until(lambda:net(c).count(REMOTE)>n,limit=8000000)
    if 'Output > ' in c.output: break
c.until(lambda:'Output > ' in c.output,limit=20000000)
assert 32 in c.fs_busy,('did not exercise Read EBUSY',c.fs_busy)
assert 'File error:' not in c.output,c.output
# Cancel without installing a large self-host image; the read already finished.
c.pending.extend('q\r'); c.until(lambda:process_exited(c.output,'compiler'))
assert 'exited with code 0.' in c.output
print(f'PASS compiler reads full source during remote ls: {len(c.fs_busy)} actual kernel EBUSY results retried',flush=True)

# Bare LF must work for the default answer, while the LF of a CRLF pair must
# not accidentally accept a choice before the user has answered.
c=StorageCPU(); c.until(lambda:c.prompt_count==1 and net(c).endswith(REMOTE))
c.network_in.extend(map(ord,'compiler\r\n'))
c.until(lambda:'Source > ' in net(c))
c.network_in.extend(map(ord,'tinyc/hello.c\r\n'))
c.until(lambda:'Output > ' in net(c))
c.network_in.extend(map(ord,'linechoice\r\n'))
c.until(lambda:'Run in background? [Y/n] > ' in net(c))
end=c.steps+30000; c.until(lambda:c.steps>=end)
assert 'Compiling...' not in net(c),'CRLF accidentally accepted choice'
c.network_in.append(10)
c.until(lambda:process_exited(net(c),'compiler'),limit=20000000)
assert entry(c.image,'linechoice') and 'exited with code 0.' in net(c)
assert 'Compiled ' not in net(c) and ' remote background\n\n' in net(c)
print('PASS remote CRLF pairing and bare-LF default answer',flush=True)

c=StorageCPU(); c.until(lambda:c.prompt_count==1)
c.run_program('compiler','Source > ')
c.pending.extend('tinyc/hello.c\r'); c.until(lambda:'Output > ' in c.output)
before=c.storage_writes
c.pending.extend('cancelled\rq\r')
c.until(lambda:process_exited(c.output,'compiler'))
assert c.storage_writes==before and 'Compiling...' not in c.output
print('PASS cancel at background question installs nothing',flush=True)
