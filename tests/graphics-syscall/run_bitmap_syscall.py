"""Compiled kernel/user BIN regression; ideal model, not Digital hardware."""
from run_graphics import GraphicsCPU, ROOT


def expected_screen():
    screen = [0] * 16000
    def rect(x,y,w,h,c):
        for yy in range(y,y+h):
            for xx in range(x,x+w): screen[yy*160+xx] = c
    rect(96,12,12,24,3)
    rect(108,12,12,24,5)
    rect(56,20,8,8,4)
    for y, width in enumerate((6,1,1,5,1,1,1,0)):
        for x in range(width):
            screen[(20+y)*160+56+x] = 2
            screen[(20+y)*160+104+x] = 2
    rect(16,56,8,4,2); rect(16,60,8,4,4)
    rect(40,56,8,4,4); rect(40,60,8,4,3)
    for x,y in ((71,59),(64,60),(84,56),(88,62)): screen[y*160+x] = 1
    rect(96,56,24,16,3)
    screen[99*160+159] = 2
    return screen


def run():
    binary = ROOT/'tests/graphics-syscall/build/bitmap_contract.bin'
    for duration, inject in ((0,False),(80000,True)):
        cpu = GraphicsCPU(binary, duration=duration, inject=inject)
        cpu.until(lambda: cpu.halted)
        assert 'BITMAP CONTRACT PASS' in cpu.output, cpu.output
        assert 'exited with code 0.' in cpu.output, cpu.output
        assert len(cpu.commands) == 13
        assert [op[-1] for op in cpu.operations] == [0,0,0,1,2,2,1,1,1,1,0,2,1]
        assert len(cpu.writes) == 4*8+9*11
        assert cpu.screen == expected_screen(), 'pixel mismatch'
        if inject:
            assert cpu.injected_keys == cpu.restores == 13
            assert cpu.busy_ticks > 0 and cpu.user_yields > 0 and not cpu.snapshots
        print(f'PASS bitmap syscall: duration={duration}, pixels, bounds, packet snapshot, callbacks={cpu.restores}')
    cpu = GraphicsCPU(binary, duration=80000, inject=True, stuck=True)
    cpu.until(lambda: cpu.halted)
    assert 'Bitmap timeout (10 s).' in cpu.output and 'exited with code 1.' in cpu.output
    assert len(cpu.commands) == 1 and len(cpu.writes) == 8
    print('PASS bitmap client timeout: no overwrite, tick/callbacks remain live')


if __name__ == '__main__': run()
