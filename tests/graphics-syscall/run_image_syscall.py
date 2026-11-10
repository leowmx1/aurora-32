"""Actual BIN: RAM-backed >64-bit images and new 480x240 boundaries."""
from run_graphics import GraphicsCPU, ROOT
def run():
    for duration in (0,80000):
        c=GraphicsCPU(ROOT/'tests/graphics-syscall/build/image_contract.bin',duration=duration)
        c.until(lambda:c.halted)
        assert 'IMAGE CONTRACT PASS' in c.output,c.output
        assert 'exited with code 0.' in c.output
        assert [o[2:4] for o in c.exit_operations]==[(480,240),(65,17),(33,1),(1,1)]
        assert len(c.exit_writes)==8+(9+35)+(9+2)+(9+1)
        expected=[3]*115200
        bits=sum((0x80000001+i)<<(32*i) for i in range(35))
        for y in range(17):
            for x in range(65): expected[(180+y)*480+300+x]=2 if bits>>(y*65+x)&1 else 4
        for x in (0,31,32): expected[200*480+400+x]=2
        expected[-1]=2
        assert c.exit_screen==expected
        assert c.screen==[0x8000]*115200
        if duration: assert c.busy_ticks>0 and c.user_yields>0
        print('PASS Image:',duration,'word/row boundaries, transparent bits, packet/data snapshot, invalid bounds/pointers',flush=True)
if __name__=='__main__': run()
