"""Exercise full existing pointer contracts in bank 3, with other users alive."""
import struct
from run_storage import StorageCPU, ROOT, image_words

PROMPT='aurora:remote:/ > '
def net(c): return ''.join(map(chr,c.network_out)).replace('\r','')
def remote(c,cmd,expected):
    at=len(net(c)); n=net(c).count(PROMPT)
    c.network_in.extend(map(ord,cmd+'\r\n'))
    c.until(lambda:net(c).count(PROMPT)>n,limit=8000000)
    assert expected in net(c)[at:],net(c)[at:][-2000:]

for fixture,expected in [('storage/fs_contract','FS CONTRACT PASS'),
                         ('compiler/install_contract','INSTALL CONTRACT PASS'),
                         ('storage/console_contract','CONSOLE CONTRACT PASS'),
                         ('graphics-syscall/image_contract','IMAGE CONTRACT PASS')]:
    directory,name=fixture.split('/')
    raw=(ROOT/f'tests/{directory}/build/{name}.bin').read_bytes()
    code=list(struct.unpack('<'+'I'*(len(raw)//4),raw))
    exe=[0x41455845,1,16,22+len(code),0x80000000,3,2,1,16,6,1,0,0,0,0,0,
         0x80000000,22,len(code),len(code),1,0]+code
    image=image_words(); at=16+5*40  # Replace tetris only, preserve calculator.
    image[at+1]=len(image); image[at+2]=image[at+37]=len(exe)
    image[at+3]=sum(exe)&0xffffffff; image.extend(exe); image[3]=len(image)
    c=StorageCPU(image); c.until(lambda:c.prompt_count==1 and net(c).endswith(PROMPT))
    c.command_done('process-demo &')
    remote(c,'process-demo &','background')
    c.run_program('calculator','>> ')
    remote(c,'run tetris',expected)
    assert c.dma_jobs[-1][1]==0xc00000
    assert 'name=tetris slot=4 bank=3 remote foreground' in net(c)
    assert 'exited with code 0.' in net(c)
    c.command('8*3+2','Result: 26')
    assert 'context check failed' not in c.output+net(c)
    c.quit_key()
    print(f'PASS bank 3: {fixture}, other users survive, no cross-bank buffer writes',flush=True)
