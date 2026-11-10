"""Execute the standalone BIN with ideal bitmap RAM hardware, not Digital."""
import pathlib
import sys

ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tests/kernel-events'))
from run_examples import DemoCPU
from text_patterns import FONT, TEXT_CASES, FRAME_JOBS, text_jobs, header

BIN=ROOT/'tests/graphics-data-ram/build/data_ram_test.bin'
BASE,DATA,STAT=0x09000000,0x09600000,0x09900000
JOBS=[
    (0,0,480,240,0,0,0), (16,88,35,42,2,4,1),
    (80,84,28,50,3,0,0), (108,84,30,50,5,0,0),
    (90,88,35,42,1,4,2), (90,88,35,42,2,2,2),
    (16,202,65,17,1,0,1),
    (12,158,84,26,2,0,0), (108,158,84,26,3,0,0),
    (204,158,84,26,4,0,0), (300,158,168,26,5,0,0),
    (276,22,192,38,1,0,0), (12,68,456,1,4,0,0),
    (420,202,48,18,3,0,0),
]+text_jobs()+FRAME_JOBS
WIDTH,HEIGHT=480,240

class DataRAMCPU(DemoCPU):
    def __init__(self,duration=1000,offset=0,stuck=False,initial_busy=False):
        super().__init__('data-ram',BIN)
        self.kernel_rom=self.rom
        self.params={}
        self.data={}
        self.jobs=[]
        self.writes=[]
        self.snapshots=[]
        self.screen=[-1]*(WIDTH*HEIGHT)
        self.duration,self.offset,self.stuck=duration,offset,stuck
        self.timer_origin=None
        self.deadline=0 if initial_busy else None
        self.uploads=[]
        self.current_upload=[]

    def complete(self):
        if self.deadline is None or self.steps<self.deadline or self.stuck: return
        x,y,w,h,fg,bg,mode=self.jobs[-1]
        for i in range(w*h):
            on=1 if mode==0 else (self.data[i//32]>>(i%32))&1
            if mode==2 and not on: continue
            self.screen[(y+i//w)*WIDTH+x+i%w]=fg if on else bg
        self.snapshots.append(self.screen[:])
        self.deadline=None

    def read(self,a):
        if a==0x08000000:
            if self.timer_origin is None: self.timer_origin=self.steps
            return ((self.steps-self.timer_origin)//20+self.offset)&0xffffffff
        if a>>24==9:
            assert a==STAT,('unexpected graphics read',hex(a))
            self.complete()
            return int(self.deadline is not None)
        return super().read(a)

    def write(self,a,v):
        if a>>24!=9: return super().write(a,v)
        assert self.kernel_mode and not self.ie
        self.complete()
        assert self.deadline is None,'modified graphics while busy'
        self.writes.append((a,v))
        if DATA<=a<DATA+0x100000:
            self.data[a-DATA]=v
            self.current_upload.append(a-DATA)
        elif a==STAT:
            assert v in (0,1)
            if not v: return
            p=tuple(self.params[r] for r in (0,1,2,3,4,5,8))
            assert p==JOBS[len(self.jobs)],p
            x,y,w,h,fg,bg,mode=p
            assert 0<=x<x+w<=WIDTH and 0<=y<y+h<=HEIGHT
            if p[-1]:
                words=(p[2]*p[3]+31)//32
                assert self.current_upload==list(range(words+1))
                assert self.data[words]==0xffffffff,'missing overrun sentinel'
                tail=p[2]*p[3]%32
                if tail: assert self.data[words-1]>>tail==(1<<(32-tail))-1
            else: assert not self.current_upload
            self.uploads.append(self.current_upload)
            self.current_upload=[]
            self.jobs.append(p)
            self.deadline=self.steps+self.duration
        else:
            reg=(a-BASE)>>20
            assert a&0xfffff==0 and reg in (0,1,2,3,4,5,8),('wrong register/old high address',hex(a))
            self.params[reg]=v

    def step(self):
        assert self.kernel_mode and not self.ie
        assert self.read(self.pc)&255 not in (0x37,0x39,0x3a,0xf0)
        super().step()

def reference():
    # Independent pixel reference: scale strings, do not reuse packed masks.
    screen=[0]*(WIDTH*HEIGHT)
    def rectangle(x,y,w,h,c):
        for row in range(y,y+h): screen[row*WIDTH+x:row*WIDTH+x+w]=[c]*w
    def letter(x,y,rows,color):
        for row,line in enumerate(rows):
            for col,ch in enumerate(line):
                if ch=='#': rectangle(x+col*7,y+row*6,7,6,color)
    rectangle(16,88,35,42,4)
    letter(16,88,['.###.','#...#','#...#','#####','#...#','#...#','#...#'],2)
    rectangle(80,84,28,50,3); rectangle(108,84,30,50,5)
    letter(90,88,['#####','#....','#....','####.','#....','#....','#....'],1)
    rectangle(16,202,65,17,0)
    rectangle(16,202,65,1,1); rectangle(16,218,65,1,1)
    rectangle(16,202,1,17,1); rectangle(80,202,1,17,1)
    for i in range(65*17):
        if i%32 in (0,31): screen[(202+i//65)*WIDTH+16+i%65]=1
    rectangle(12,158,84,26,2); rectangle(108,158,84,26,3)
    rectangle(204,158,84,26,4); rectangle(300,158,168,26,5)
    rectangle(276,22,192,38,1); rectangle(12,68,456,1,4)
    rectangle(420,202,48,18,3)
    # Draw font cells directly, independently of the packed C data stream.
    for x,y,fg,bg,mode,scale,text in TEXT_CASES:
        if mode==1: rectangle(x,y,len(text)*6*scale,8*scale,bg)
        for index,char in enumerate(text):
            for row,mask in enumerate(FONT[char]):
                for col in range(5):
                    if mask&(16>>col): rectangle(x+(index*6+col)*scale,y+row*scale,scale,scale,fg)
    rectangle(0,0,WIDTH,1,1); rectangle(0,HEIGHT-1,WIDTH,1,1)
    rectangle(0,0,1,HEIGHT,1); rectangle(WIDTH-1,0,1,HEIGHT,1)
    rectangle(WIDTH-1,HEIGHT-1,1,1,2)
    return screen

def run(duration,offset=0,stuck=False,initial_busy=False):
    c=DataRAMCPU(duration,offset,stuck,initial_busy)
    marker='No further writes.\n' if stuck else 'Compare the preview.\n'
    c.until(lambda:marker in c.output,limit=5000000)
    count=len(c.writes); end=c.steps+3000
    c.until(lambda:c.steps>=end)
    assert len(c.writes)==count and c.ticks==0,'unexpected redraw/interrupt'
    if stuck:
        assert 'TIMEOUT' in c.output and 'DONE' not in c.output
        assert len(c.jobs)==(0 if initial_busy else 1)
        if initial_busy: assert not c.writes
    else:
        assert c.jobs==JOBS
        assert c.screen==reference(),[(i,a,b) for i,(a,b) in enumerate(zip(c.screen,reference())) if a!=b][:20]
        assert c.snapshots[4]==c.snapshots[5],'transparent zero damaged picture'
        assert len(c.uploads[1])==47 and len(c.uploads[6])==36
        assert c.screen[-1]==2 and all(n==1 for n in c.screen[:WIDTH])
        assert any(p[0]>255 and p[6]==1 for p in c.jobs)
        assert any(p[2]==480 and p[6]==1 for p in c.jobs),'missing wide bitmap'
    print(f'PASS data RAM BIN: duration={duration}, timer={offset:#x}, stuck={stuck}, initial_busy={initial_busy}',flush=True)
    return c

if __name__=='__main__':
    assert (BIN.parents[1]/'text_data.h').read_text().strip()==header().strip(),'stale generated text data'
    c=run(0)
    run(1000,0xfffffff0)
    run(1000,stuck=True)
    run(1000,stuck=True,initial_busy=True)
    if '--preview' in sys.argv:
        from PIL import Image
        colors=[(255,255,255),(0,0,0),(255,0,0),(0,255,0),(0,0,255),(255,255,0)]
        im=Image.new('RGB',(WIDTH,HEIGHT)); im.putdata([colors[p] for p in c.screen])
        im.save(BIN.with_name('preview-native.png'))
        im.resize((960,480),Image.Resampling.NEAREST).save(BIN.with_name('preview.png'))
