"""Actual kernel and maze BINs with ideal rectangle hardware, not Digital."""
import pathlib
import subprocess
import sys
from collections import deque

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tests/graphics-syscall'))
from run_graphics import GraphicsCPU
from font_reference import glyph_pixels, font_commands
from run_events import CheckedCPU

BACKGROUND, WALL, MARKER = 0xf798, 0xa1ee, 0xed27

FONT = [[7,5,5,5,7], [2,6,2,2,7], [7,1,7,4,7], [7,1,7,1,7], [5,5,7,1,1],
        [7,4,7,1,7], [7,4,7,5,7], [7,1,1,1,1], [7,5,7,5,7], [7,5,7,1,7]]

def score_pixels(screen, moves):
    for i, digit in enumerate(f'{moves % 1000:03d}'):
        for row, bits in enumerate(FONT[int(digit)]):
            for col in range(3):
                color = WALL if bits & (4 >> col) else BACKGROUND
                for dy in range(2):
                    for dx in range(2):
                        assert screen[(row*2+dy)*160+28+i*8+col*2+dx] == color, (moves,i,row,col)


class MazeCPU(GraphicsCPU):
    def __init__(self):
        super().__init__(ROOT / 'user-programs/programs/maze-screen/build/maze-screen.bin', duration=120)
        out = subprocess.check_output([str(ROOT/'build-tools/bin/llvm-nm.exe'), '-n',
            str(ROOT/'user-programs/programs/maze-screen/build/maze-screen.o')], text=True)
        self.draw_entry = 0x80000000 + next(int(line.split()[0],16)//4
            for line in out.splitlines() if line.split()[-1] == 'draw_maze')
        self.draw_return = None
        self.frames = []
        self.first_command = 0

    def syscall(self):
        if not self.kernel_mode and self.r[0] == 15:
            assert self.deadline is None, 'graphics wait must use Busy/Yield'
        CheckedCPU.syscall(self)

    def step(self):
        if not self.kernel_mode and self.pc == self.draw_entry and self.draw_return is None:
            self.draw_return = self.read(self.sp-1)
        if not self.kernel_mode and self.draw_return is not None and self.pc == self.draw_return:
            assert self.deadline is None
            self.frames.append((self.screen[:], self.commands[self.first_command:]))
            self.first_command = len(self.commands)
            self.draw_return = None
        super().step()

    def press(self, char):
        self.pending.append(char)
        self.next_key = self.steps

    def next_frame(self):
        count = len(self.frames)
        self.until(lambda: len(self.frames) > count)
        return self.frames[-1]


def inspect(screen):
    grid = {(x,y): screen[(12+y*8)*160+20+x*8] == BACKGROUND for y in range(11) for x in range(15)}
    assert sum(grid.values()) == 69, '35 rooms and 34 carved connecting cells'
    assert all(not grid[x,y] for x,y in grid if x in (0,14) or y in (0,10))
    seen = {(1,1)}; queue = deque([(1,1)])
    while queue:
        x,y = queue.popleft()
        for dx,dy in [(0,1),(0,-1),(1,0),(-1,0)]:
            p = x+dx,y+dy
            if grid.get(p) and p not in seen: seen.add(p); queue.append(p)
    assert len(seen) == 69 and (13,9) in seen
    return grid


def player(screen, position):
    x,y = position
    assert all(screen[(12+y*8+dy)*160+20+x*8+dx] == MARKER for dy in range(2,6) for dx in range(2,6))


def route(grid):
    queue = deque([((1,1),'')]); seen={(1,1)}
    while queue:
        (x,y), path = queue.popleft()
        if (x,y)==(13,9): return path
        for dx,dy,key in [(0,1,'s'),(0,-1,'w'),(1,0,'d'),(-1,0,'a')]:
            p=x+dx,y+dy
            if grid.get(p) and p not in seen: seen.add(p); queue.append((p,path+key))
    raise AssertionError('no exit route')


def run():
    cpu = MazeCPU()
    screen,_ = cpu.next_frame(); grid = inspect(screen); player(screen,(1,1))
    assert font_commands(cpu, BACKGROUND, {WALL}) == 8
    glyph_pixels(screen, 20, 0, 'S', WALL, BACKGROUND)
    glyph_pixels(screen, 100, 0, 'T', WALL, BACKGROUND)
    baseline = cpu.output
    assert baseline.count('Screen Maze:') == 1
    assert 'WASD move  R restart  N new maze  Q quit\n' in baseline
    assert 'New maze.' not in baseline and 'Restart.' not in baseline
    score_pixels(screen,0)
    count=len(cpu.frames); restored=cpu.restores
    cpu.press('a'); cpu.until(lambda: cpu.restores > restored)
    assert len(cpu.frames) == count, 'walking into wall must not redraw'
    assert cpu.output == baseline
    path=route(grid); position=(1,1)
    for i,key in enumerate(path[:3]):
        dx,dy={'w':(0,-1),'s':(0,1),'a':(-1,0),'d':(1,0)}[key]
        position=position[0]+dx,position[1]+dy
        cpu.press(key); screen, commands=cpu.next_frame()
        assert inspect(screen)==grid; player(screen,position)
        score_pixels(screen,i+1)
        map_commands=[c for c in commands if c[1]>=12]
        assert len(map_commands)==(3 if i==0 else 2), map_commands
        assert cpu.output == baseline, 'movement printed to terminal'
    # Leaving start restores the small teal start marker.
    assert screen[(12+8+3)*160+20+8+3] == WALL
    cpu.press('r'); screen,commands=cpu.next_frame()
    assert inspect(screen)==grid; player(screen,(1,1))
    score_pixels(screen,0)
    assert not any(c[2:4]==(120,88) for c in commands), 'restart must not regenerate'
    assert cpu.output == baseline, 'restart printed to terminal'
    layouts={tuple(grid.values())}
    for _ in range(3):
        cpu.press('n'); screen,_=cpu.next_frame(); grid=inspect(screen)
        player(screen,(1,1)); layouts.add(tuple(grid.values()))
        assert cpu.output == baseline, 'new maze printed to terminal'
    assert len(layouts)>1, 'new mazes must not all repeat'
    position=(1,1)
    for steps,key in enumerate(route(grid),1):
        dx,dy={'w':(0,-1),'s':(0,1),'a':(-1,0),'d':(1,0)}[key]
        position=position[0]+dx,position[1]+dy
        cpu.press(key); screen,_=cpu.next_frame()
        assert inspect(screen)==grid; player(screen,position)
        score_pixels(screen,steps)
        assert cpu.output == baseline, 'printed before finishing final frame'
    cpu.until(lambda: cpu.halted)
    assert 'Escaped! Well done.' in cpu.output and 'exited with code 0.' in cpu.output
    assert cpu.output.count('Screen Maze:') == 1 and cpu.output.count('Steps: ') == 1
    assert f'Steps: {steps}  Time: ' in cpu.output
    assert cpu.ticks>0 and cpu.restores>10 and cpu.deadline is None
    assert sum(c[:4]==(0,0,160,100) for c in cpu.commands)==1
    font_commands(cpu, BACKGROUND, {WALL})
    print(f'PASS maze BIN: walls/reachability, dirty moves, restart, {len(layouts)} distinct layouts, exit; {cpu.restores} restored callbacks')
    cpu=MazeCPU(); cpu.next_frame(); cpu.press('q'); cpu.until(lambda:cpu.halted)
    assert 'Quit.' in cpu.output and 'exited with code 0.' in cpu.output
    assert cpu.output.count('Screen Maze:') == 1 and cpu.output.count('Steps: ') == 1
    assert 'Steps: 0  Time: ' in cpu.output
    print('PASS quit from keyboard callback; no graphics IRQ required')


if __name__=='__main__': run()
