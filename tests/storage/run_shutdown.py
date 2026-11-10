"""Production BIN framebuffer assertions; not a Digital electrical simulation."""
from run_storage import StorageCPU
from font_reference import glyph_bits

BLUE, WHITE, BLACK = 0x8015, 0xffff, 0x8000


def expected_blue(vector, user=True):
    pixels=[BLUE]*115200
    for x,y,w,h in [(12,13,4,4),(12,26,4,4),(30,8,5,4),(26,12,4,5),
                    (23,17,4,9),(26,26,4,5),(30,31,5,4)]:
        x,y,w,h=48+x*2,y*2,w*2,h*2
        for dy in range(h):
            for dx in range(w): pixels[(y+dy)*480+x+dx]=WHITE
    lines=[(100,'USER PROCESS FAULT' if user else 'KERNEL FAULT'),
           (130,'MEMORY VIOLATION' if vector==0x03000004 else 'ILLEGAL INSTRUCTION'),
           (160,'STOP 03000004' if vector==0x03000004 else 'STOP 03000005'),
           (200,'HALTED  PRESS RESET')]
    for y,text in lines:
        assert 48+(len(text)-1)*16+12<=480
        for i,c in enumerate(text):
            if c==' ': continue
            for dy in range(16):
                for dx in range(12):
                    bit=glyph_bits(c) & (1<<(dy*12+dx))
                    pixels[(y+dy)*480+48+i*16+dx]=WHITE if bit else BLUE
    return pixels


def fault(vector, graphics=False):
    cpu=StorageCPU(); cpu.until(lambda:cpu.prompt_count==1)
    if graphics:
        cpu.duration=10000  # keep a draw busy across syscall return to user mode
        cpu.pending.extend('quickrun snake\r')
        cpu.until(lambda:cpu.loaded_writes and cpu.deadline is not None and not cpu.kernel_mode,limit=8000000)
    else: cpu.run_program('calculator','>> ')
    prompts=cpu.prompt_count; ticks=cpu.ticks; steps=cpu.user_steps
    cpu.interrupt(cpu.read(vector))
    cpu.until(lambda:cpu.halted,limit=4000000)
    assert cpu.screen==expected_blue(vector), 'framebuffer differs from independent font/rectangle reference'
    assert cpu.deadline is None and not cpu.ie and cpu.kernel_mode
    assert cpu.prompt_count==prompts and cpu.ticks==ticks and cpu.user_steps==steps
    assert 'System halted. Reset to restart.' in cpu.output
    print('PASS blue screen:',hex(vector),'active graphics' if graphics else 'calculator',flush=True)


if __name__=='__main__':
    for vector in (0x03000004,0x03000005): fault(vector)
    fault(0x03000004,graphics=True)
    for stuck in (False,True):
        cpu=StorageCPU(); cpu.until(lambda:cpu.prompt_count==1)
        cpu.stuck=stuck
        cpu.pending.extend('halt\r'); cpu.until(lambda:cpu.halted,limit=4000000)
        assert not cpu.ie and cpu.deadline is None
        if not stuck: assert cpu.screen==[BLACK]*115200
        else: assert 'Graphics unavailable during shutdown.' in cpu.output
        print('PASS normal shutdown:', 'stalled GPU fallback' if stuck else 'black fill completed',flush=True)
