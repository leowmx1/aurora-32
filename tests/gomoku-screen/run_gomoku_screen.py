"""Execute actual BINs with tick/keyboard and ideal graphics, not Digital."""
import pathlib
import subprocess
import sys
from types import SimpleNamespace

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tests/graphics-syscall'))
from run_graphics import GraphicsCPU
from font_reference import glyph_pixels, font_commands, logical_pixels
from run_events import CheckedCPU


class BoardCPU(GraphicsCPU):
    def __init__(self, duration=120, stuck=False):
        folder = ROOT / 'user-programs/programs/gomoku-screen/build'
        super().__init__(folder / 'gomoku-screen.bin', duration=duration, stuck=stuck)
        symbols = subprocess.check_output([str(ROOT/'build-tools/bin/llvm-nm.exe'),
            '-n', str(folder/'gomoku-screen.o')], text=True)
        self.draw_entry = 0x80000000 + next(int(line.split()[0], 16)//4
            for line in symbols.splitlines() if line.split()[-1] == 'draw_status')
        self.draw_return = None
        self.frames = []

    def syscall(self):
        if not self.kernel_mode and self.r[0] == 15:
            assert self.deadline is None, 'graphics wait must use Busy/Yield'
        CheckedCPU.syscall(self)

    def step(self):
        if not self.kernel_mode and self.pc == self.draw_entry and self.draw_return is None:
            self.draw_return = self.read(self.sp - 1)
        if not self.kernel_mode and self.draw_return is not None and self.pc == self.draw_return:
            assert self.deadline is None, 'last rectangle not completed'
            self.frames.append((self.r[:], self.screen[:]))
            self.draw_return = None
        super().step()

    def ready(self):
        self.until(lambda: bool(self.frames))
        self.baseline = self.output
        assert font_commands(self, 0xe28d, {0x9ca3,1,0,2}) == 16
        for i in range(5):
            glyph_pixels(self.screen, 29+i*16, 3, str(i+1), 0x9ca3, 0xe28d)
            glyph_pixels(self.screen, 12, 19+i*16, chr(65+i), 0x9ca3, 0xe28d)
        glyph_pixels(self.screen, 112, 8, 'P', 0x9ca3, 0xe28d)
        glyph_pixels(self.screen, 120, 8, '1', 1, 0xe28d)
        for i,c in enumerate('TURN'):
            glyph_pixels(self.screen, 112+i*8, 24, c, 0x9ca3, 0xe28d)
        assert self.baseline.count('Screen Gomoku:') == 1
        for line in ['Type A3 or 3A (A-E, 1-5); lowercase works.',
                     'Second character places a stone; no Enter needed.',
                     'First letter highlights a row; first digit a column.',
                     'Backspace / Delete / Esc clears the selection.',
                     'Q or q quits.']:
            assert line in self.baseline, line
        return self

    def press(self, value, final=False):
        restored = self.restores
        self.inject.extend((1, ord(c)) for c in value)
        if final:
            self.until(lambda: self.halted)
            assert 'exited with code 0.' in self.output and self.deadline is None
            assert self.screen==[0x8000]*115200
            # Board assertions below inspect the completed pre-exit image;
            # the normal shutdown itself must already have painted black.
            self.screen=self.exit_screen
            self.commands=self.exit_commands
            self.operations=self.exit_operations
            self.writes=self.exit_writes
        else:
            self.until(lambda: self.restores == restored + len(value))
            assert self.output == self.baseline, 'terminal output during play'


def board_pixels(cpu, expected, marked=()):
    screen=cpu.screen
    logical=logical_pixels(screen)
    assert all(logical[y*160+x]==0xe28d for y in range(86,96) for x in range(112,160))
    for i in range(6):
        assert all(logical[y*160+24+i*16]==0x9ca3 for y in range(16,97))
        assert all(logical[(16+i*16)*160+x]==0x9ca3 for x in range(24,105))
    for row in range(5):
        for col in range(5):
            piece=expected[row*5+col]
            lx,ly=24+col*16,16+row*16
            sx,sy=48+(lx+3)*12//5,(ly+3)*12//5
            for y in range((ly+1)*12//5,(ly+16)*12//5):
                for x in range(48+(lx+1)*12//5,48+(lx+16)*12//5):
                    color=0xe28d
                    if piece and 0<=x-sx<24 and 0<=y-sy<24 and (2*(x-sx)-23)**2+(2*(y-sy)-23)**2<=576:
                        color=1 if piece==1 else 0
                    if row*5+col in marked and 48+(lx+6)*12//5<=x<48+(lx+10)*12//5 and (ly+6)*12//5<=y<(ly+10)*12//5:
                        color=2
                    assert screen[y*480+x]==color,(row,col,x,y,piece)
    assert sum(c[:4]==(0,0,480,240) for c in cpu.commands)==1

def drawing_commands(cpu):
    pairs=[(op,bits) for op,bits in zip(cpu.operations,cpu.image_bits) if op[-1]!=2]
    font_commands(SimpleNamespace(operations=[o for o,b in pairs],image_bits=[b for o,b in pairs]),0xe28d,{0x9ca3,1,0,2})
    for op,bits in zip(cpu.operations,cpu.image_bits):
        x,y,w,h,c,bg,low,high,mode=op
        if mode!=2: continue
        assert (w,h)==(24,24) and c in (0,1) and bg==0xe28d
        assert x in [48+(27+i*16)*12//5 for i in range(5)]
        assert y in [(19+i*16)*12//5 for i in range(5)]
        expected=sum(1<<(dy*24+dx) for dy in range(24) for dx in range(24)
                     if (2*dx-23)**2+(2*dy-23)**2<=576)
        assert bits==expected

def winning_cells(board, player):
    lines = [[r*5+c for c in range(5)] for r in range(5)]
    lines += [[r*5+c for r in range(5)] for c in range(5)]
    lines += [[0,6,12,18,24], [4,8,12,16,20]]
    return next(line for line in lines if all(board[i] == player for i in line))


def coordinate(cell, reverse=False):
    r, c = divmod(cell, 5)
    pair = chr(65+r) + str(c+1)
    return pair[::-1] if reverse else pair


def selection_pixels(cpu, before, row=None, col=None):
    x, y, w, h = 24, 16, 81, 81
    if row is not None: y += row*16; h = 17
    else: x += col*16; w = 17
    expected=before[:]
    for rx,ry,rw,rh in [(x,y,w,1),(x,y+h-1,w,1),(x,y+1,1,h-2),(x+w-1,y+1,1,h-2)]:
        for py in range(ry*12//5,(ry+rh)*12//5):
            for px in range(48+rx*12//5,48+(rx+rw)*12//5):
                expected[py*480+px]=0x99d8
    assert cpu.screen==expected,(row,col)


def controls():
    cpu = BoardCPU().ready()
    expected = [0]*25
    board_pixels(cpu, expected)
    # Invalid letters/digits, double letters/numbers, Enter, backspace and Esc.
    for value in ['Z', '0', '6', 'AA', '11', 'A\b', '5\x1b', '\r\n']:
        cpu.press(value)
        board_pixels(cpu, expected)
    first = len(cpu.commands)
    before = cpu.screen[:]
    cpu.press('a')
    assert len(cpu.commands) == first + 4, 'highlight must use only four border lines'
    selection_pixels(cpu, before, row=0)
    first = len(cpu.commands)
    cpu.press('1')
    expected[0] = 1
    board_pixels(cpu, expected)
    restored_border = cpu.commands[first:first+4]
    assert all(color == 0x9ca3 and (2 <= w <= 3 or 2 <= h <= 3) for x,y,w,h,color in restored_border)
    assert len(cpu.commands)-first==6,'restore=4, disk=1, digit=1'
    assert cpu.operations[-1][-1]==1
    glyph_pixels(cpu.screen,120,8,'2',0,0xe28d)
    assert cpu.operations[first+4][2:4]==(24,24) and cpu.operations[first+4][-1]==2
    assert cpu.operations[first+5][2:4]==(12,16)
    # Occupied cell does not consume player 2's turn.
    cpu.press('1A')
    board_pixels(cpu, expected)
    before = cpu.screen[:]
    first = len(cpu.commands)
    cpu.press('5')
    selection_pixels(cpu, before, col=4)
    assert len(cpu.commands) == first + 4
    first = len(cpu.commands)
    cpu.press('e')
    piece_commands = [o for o in cpu.operations[first+4:] if o[-1]==2]
    assert len(piece_commands)==1 and piece_commands[0][4]==0
    expected[24] = 2
    board_pixels(cpu, expected)
    before = cpu.screen[:]
    cpu.press('A')
    selection_pixels(cpu, before, row=0)  # Existing disk remains intact.
    cpu.press('Q', final=True)
    board_pixels(cpu, expected)
    assert 'Quit.' in cpu.output and cpu.ticks > 0 and cpu.restores > 10
    for i,c in enumerate('QUIT'):
        glyph_pixels(cpu.screen, 112+i*8, 24, c, 0x9ca3, 0xe28d)
    drawing_commands(cpu)
    assert not any(9<=o[2]<=10 and 9<=o[3]<=10 and o[4]==2 and o[-1] == 0 for o in cpu.operations)
    print('PASS board controls: both coordinate orders/case, edit, invalid/occupied, local rendering, Q', flush=True)


def game(name, moves, winner, duration=120):
    cpu = BoardCPU(duration=duration).ready()
    expected = [0]*25
    for i, cell in enumerate(moves):
        assert expected[cell] == 0
        expected[cell] = 1 + i%2
        cpu.press(coordinate(cell, bool(i%2)), final=i==len(moves)-1)
        marked = winning_cells(expected, winner) if winner and i == len(moves)-1 else ()
        board_pixels(cpu, expected, marked)
    message = f'Player {winner} wins!' if winner else 'Draw!'
    assert message in cpu.output, cpu.output
    for i,c in enumerate('WIN' if winner else 'DRAW'):
        glyph_pixels(cpu.screen, 112+i*8, 24, c, 0x9ca3, 0xe28d)
    assert cpu.ticks > 0
    drawing_commands(cpu)
    marks = [o for o in cpu.operations if 9<=o[2]<=10 and 9<=o[3]<=10 and o[4]==2 and o[-1]==0]
    assert [(o[0],o[1]) for o in marks] == [(48+(30+i%5*16)*12//5,(22+i//5*16)*12//5) for i in marked]
    assert len([o for o in cpu.operations if o[-1] == 2]) == len(moves)
    print(f'PASS {name}: {len(moves)} moves, {cpu.restores} full callback restores', flush=True)


if __name__ == '__main__':
    controls()
    game('row / slow graphics', [0,5,1,6,2,7,3,8,4], 1, duration=2000)
    game('column', [0,1,5,2,10,3,15,4,20], 1)
    game('diagonal', [0,1,6,2,12,3,18,5,24], 1)
    game('anti diagonal', [4,0,8,1,12,2,16,5,20], 1)
    game('player 2', [5,0,6,1,7,2,8,3,10,4], 2)
    game('bottom edge', [20,0,21,1,22,2,23,3,24], 1)
    game('right edge', [4,0,9,1,14,2,19,3,24], 1)
    game('two winning lines: first row wins marking priority',
         [10,0,11,1,13,3,14,4,2,5,7,6,17,8,22,9,12], 1)
    pattern = '11221' '22112' '12121' '12212' '12121'
    ones = [i for i,v in enumerate(pattern) if v == '1']
    twos = [i for i,v in enumerate(pattern) if v == '2']
    draw = [item for pair in zip(ones, twos) for item in pair] + ones[len(twos):]
    assert len(draw) == 25
    game('draw', draw, 0)
    cpu = BoardCPU(stuck=True)
    cpu.until(lambda: cpu.halted)
    assert 'Graphics timeout' in cpu.output and 'exited with code 1.' in cpu.output
    print('PASS stuck graphics: bounded wait, Yield and safe error exit', flush=True)
