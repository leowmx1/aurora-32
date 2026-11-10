"""Compiled ROM + ideal scale hardware; NOT a Digital timing simulation."""
import argparse,json,pathlib,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
HERE=pathlib.Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'tests/kernel-events'))
from run_examples import DemoCPU
ROWS=json.loads((HERE/'glyph_rows.json').read_text())
SCALES=(0,1,2,3,4,6)
BASE,DATA,SCALE,STAT=0x09000000,0x09600000,0x09700000,0x09900000
JOBS=[(0,0,480,240,0x98e8,0,0,1)]
for n,s in enumerate(SCALES):
    k=max(s,1); JOBS.append((16+80*n,24,48//k,72//k,1,0,1,s))
for n,s in enumerate(SCALES):
    k=max(s,1); x,w,h=16+80*n,48//k,72//k
    JOBS.extend(((x,136,w//2,h,3,0,0,1),(x+w//2,136,w//2,h,5,0,0,1),
                 (x,136,w,h,2,0,2,s)))

class ScaleCPU(DemoCPU):
    def __init__(self,duration=0,offset=0,stuck=False,initial_busy=False):
        super().__init__('graphics-scale',HERE/'build/scale_test.bin')
        self.kernel_rom=self.rom
        self.params={}; self.data={}; self.jobs=[]; self.writes=[]
        self.uploads=[]; self.snapshots=[]; self.screen=[-1]*115200
        self.duration,self.offset,self.stuck=duration,offset,stuck
        self.deadline=0 if initial_busy else None
    def complete(self):
        if self.deadline is None or self.steps<self.deadline or self.stuck: return
        x,y,w,h,fg,bg,mode,s=self.jobs[-1]; s=max(s,1)
        for dy in range(h):
            for dx in range(w):
                index=(dy*s)*(w*s)+dx*s
                bit=1 if mode==0 else self.data[index//32]>>(index%32)&1
                if mode==2 and not bit: continue
                self.screen[(y+dy)*480+x+dx]=fg if bit else bg
        self.snapshots.append(self.screen[:]); self.deadline=None
    def read(self,a):
        if a==0x08000000: return (self.steps//20+self.offset)&0xffffffff
        if a>>24==9:
            assert a==STAT
            self.complete(); return int(self.deadline is not None)
        return super().read(a)
    def write(self,a,v):
        if a>>24!=9: return super().write(a,v)
        assert self.kernel_mode and not self.ie
        self.complete(); assert self.deadline is None,'write while busy'
        self.writes.append((a,v))
        if DATA<=a<DATA+0x100000:
            assert len(self.jobs)==1,'data re-uploaded between scaled draws'
            self.uploads.append(a-DATA); self.data[a-DATA]=v
        elif a==STAT:
            assert v in (0,1)
            if not v: return
            p=tuple(self.params[i] for i in (0,1,2,3,4,5,8,7))
            assert p==JOBS[len(self.jobs)],p
            assert [a for a,v in self.writes[-10:]]==[STAT]+[BASE+(i<<20) for i in (0,1,2,3,4,5,7,8)]+[STAT]
            if p[6]:
                assert self.uploads==list(range(108))
                assert max(p[7],1)*p[2]==48 and max(p[7],1)*p[3]==72
            self.jobs.append(p); self.deadline=self.steps+self.duration
        else:
            assert a&0xfffff==0 and (a-BASE)>>20 in (0,1,2,3,4,5,7,8)
            self.params[(a-BASE)>>20]=v
    def step(self):
        assert self.kernel_mode and not self.ie
        super().step()

def reference():
    screen=[0x98e8]*115200
    for x,y,w,h,fg,bg,mode,s in JOBS[1:]:
        for dy in range(h):
            for dx in range(w):
                bit=mode==0 or ROWS[dy*max(s,1)][dx*max(s,1)]=='#'
                if mode!=2 or bit: screen[(y+dy)*480+x+dx]=fg if bit else bg
    return screen

def run(preview=False):
    for duration,offset in ((0,0),(5000,0),(5000,0xfffffff0)):
        c=ScaleCPU(duration,offset)
        c.until(lambda:'SCALE DONE.' in c.output,limit=2000000)
        assert c.jobs==JOBS and len(c.snapshots)==25
        assert c.uploads==list(range(108)) and c.params[7]==1
        bits=sum(v<<(32*i) for i,v in c.data.items())
        assert all((bits>>(y*48+x)&1)==(ROWS[y][x]=='#') for y in range(72) for x in range(48))
        assert c.screen==reference()
        for y in (24,136):
            assert all(c.screen[(y+dy)*480+16+dx]==c.screen[(y+dy)*480+96+dx] for dy in range(72) for dx in range(48))
        n=len(c.writes); end=c.steps+10000
        c.until(lambda:c.steps>=end,limit=12000)
        assert len(c.writes)==n and not c.halted
        print('PASS scale ROM:',duration,hex(offset),'one upload, 0=1, mode 1/2, 25 jobs, exact framebuffer, no redraw',flush=True)
    for initial in (False,True):
        c=ScaleCPU(stuck=True,initial_busy=initial)
        c.until(lambda:'TIMEOUT.' in c.output,limit=1000000)
        assert not c.uploads and len(c.jobs)==(0 if initial else 1)
        n=len(c.writes); end=c.steps+10000
        c.until(lambda:c.steps>=end,limit=12000)
        assert len(c.writes)==n
        print('PASS busy timeout: no cancellation/further writes, initially busy=',initial,flush=True)
    if preview:
        from PIL import Image
        pal=[(255,255,255),(0,0,0),(255,0,0),(0,255,0),(0,0,255),(255,255,0)]
        image=Image.new('RGB',(480,240))
        image.putdata([pal[c] if c<len(pal) else tuple(((c>>s)&31)*255//31 for s in (10,5,0)) for c in reference()])
        image.save(HERE/'build/preview-native.png')
        image.resize((960,480),Image.Resampling.NEAREST).save(HERE/'build/preview.png')
if __name__=='__main__':
    p=argparse.ArgumentParser(); p.add_argument('--preview',action='store_true')
    run(p.parse_args().preview)
