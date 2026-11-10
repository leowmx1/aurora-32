"""Production 480x240 BINs: font sizes/layout, IRQ-safe uploads, preview.

Ideal ISA/device model only; not a Digital timing test.
"""
import pathlib, sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tests/storage'))
from run_storage import StorageCPU
from font_reference import FONT, glyph_bits

class LayoutCPU(StorageCPU):
    def __init__(self):
        super().__init__(); self.frames=[]
    def syscall(self):
        if not self.kernel_mode and self.r[0] in (15,29):
            assert self.deadline is None
            self.frames.append(self.screen[:])
        super().syscall()

def preview(name,pixels):
    target=ROOT/'tests/graphics-syscall/build'
    target.mkdir(exist_ok=True,parents=True)
    palette=[(255,255,255),(0,0,0),(255,0,0),(0,255,0),
             (0,0,255),(255,255,0),(0,255,255),(255,0,255)]
    rgb=bytearray()
    for c in pixels:
        rgb.extend(palette[c] if 0<=c<8 else
                   tuple(((c>>shift)&31)*255//31 for shift in (10,5,0)))
    (target/(name+'-480.ppm')).write_bytes(b'P6\n480 240\n255\n'+rgb)

def run():
    for name in ('tetris','snake','maze','gomoku'):
        c=LayoutCPU(); c.until(lambda:c.prompt_count==1)
        logo=[(o,b) for o,b in zip(c.operations,c.image_bits) if o[-1]==1]
        assert len(logo)==9 and all(o[2:4]==(24,32) for o,b in logo)
        for (o,b),ch in zip(logo,'AURORA-32'):
            if ch!='-': assert b==glyph_bits(ch,4)
        if name=='tetris': preview('console',c.screen)
        first=len(c.operations)
        c.pending.extend('quickrun '+name+'\r')
        c.until(lambda:bool(c.frames),limit=8000000)
        jobs=c.operations[first:]; bits=c.image_bits[first:]
        assert jobs[0][:4]==(0,0,480,240)
        glyphs=[(o,b) for o,b in zip(jobs,bits) if o[-1]==1]
        assert glyphs
        for op,b in glyphs:
            assert op[2:4] in ((12,16),(18,24))
            scale=op[2]//6
            assert b in {glyph_bits(ch,scale) for ch in FONT}
        assert not any('failed' in line.lower() for line in c.output.splitlines())
        preview(name,c.frames[-1])
        c.quit_key('q')
        assert 'exited with code 0.' in c.output and c.ticks>0
        print('PASS readable layout:',name,'medium HUD / large title / large console logo; clean exit',flush=True)

if __name__=='__main__': run()
