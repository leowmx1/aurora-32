"""Execute compiled game logic and production graphics under tick/callbacks."""
import pathlib, struct, sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tests/storage'))
from run_storage import StorageCPU, image_words

def logic():
    words=image_words()
    raw=(ROOT/'tests/tetris/build/selftest.bin').read_bytes()
    code=list(struct.unpack('<'+'I'*(len(raw)//4),raw))
    exe=[0x41455845,1,16,22+len(code),0x80000000,3,2,1,16,6,1,0,0,0,0,0,
         0x80000000,22,len(code),len(code),1,0]+code
    words[17]=len(words); words[18]=words[53]=len(exe); words[19]=sum(exe)&0xffffffff
    words.extend(exe); words[3]=len(words)
    c=StorageCPU(words); c.until(lambda:c.prompt_count==1)
    c.command_done('quickrun calculator')
    assert 'TETRIS LOGIC PASS' in c.output and 'TETRIS FAIL' not in c.output,c.output[-600:]
    print('PASS compiled Tetris: rotations, bounds, wall kick, bag, lock, 1/4 lines, score, top-out, pause/drop/quit',flush=True)

class TetrisCPU(StorageCPU):
    def __init__(self):
        super().__init__(); self.frames=[]
    def syscall(self):
        if not self.kernel_mode and self.r[0] in (15,29):
            assert self.deadline is None
            self.frames.append((self.screen[:],len(self.commands),self.r[0]))
        super().syscall()
    def press(self,key):
        restored=self.restores; f=len(self.frames)
        self.pending.append(key)
        self.until(lambda:self.restores>restored and len(self.frames)>f,limit=2000000)

def render():
    c=TetrisCPU(); c.until(lambda:c.prompt_count==1)
    c.pending.extend('quickrun tetris\r'); c.until(lambda:bool(c.frames),limit=8000000)
    assert '4-pixel cells' in c.output
    game_start=len(c.commands)
    # 4 active cells, 3x3 colored interiors, all gutters stay WELL.
    def blocks(pixels):
        return {(x,y) for y in range(20) for x in range(10)
                if pixels[(8+4*y)*160+58+4*x]!=0x8864}
    initial=blocks(c.frames[-1][0]); assert len(initial)==4
    assert all(c.screen[(8+4*y+3)*160+58+4*x+3]==0x8864 for y in range(20) for x in range(10))
    before=c.user_steps; end=c.steps+20000
    c.until(lambda:c.steps>=end); stable=c.user_steps
    end=c.steps+20000; c.until(lambda:c.steps>=end)
    assert c.user_steps==stable,'waiting Tetris must block, not poll'
    c.press(' '); c.press('p')
    frozen=blocks(c.frames[-1][0]); n=len(c.commands)
    c.press('a'); assert blocks(c.frames[-1][0])==frozen and len(c.commands)==n
    c.press('p'); c.press('a'); c.press(' ')
    assert len(blocks(c.frames[-1][0]))==4,'Space must rotate, not lock a piece'
    c.press('w')
    assert len(blocks(c.frames[-1][0]))>=8,'hard drop must leave a locked piece and spawn another'
    assert not any(r[:4]==(0,0,160,100) for r in c.commands[game_start:]),'full redraw during play'
    c.quit_key('q'); assert 'Tetris quit.' in c.output
    assert c.ticks>0 and c.restores>=7
    print('PASS Tetris graphics: 20 rows, small cells/gutters, no idle polling/full-frame redraw, input/tick, hard drop, exit',flush=True)

if __name__=='__main__': logic(); render()
