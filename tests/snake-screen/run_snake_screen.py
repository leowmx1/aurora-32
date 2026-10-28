"""Real kernel/user BIN test with ideal fill device, not Digital timing."""
import pathlib
import subprocess
import sys
from collections import deque

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tests/graphics-syscall'))
from run_graphics import GraphicsCPU
from font_reference import glyph_pixels, font_commands
from run_events import CheckedCPU
sys.path.insert(0, str(ROOT / 'tests/midi-syscall'))
from device import MidiDevice

BACKGROUND, BODY, HEAD, FOOD = 0x8ca7, 0xb2f0, 0xff2c, 0xfdab


class SnakeCPU(MidiDevice, GraphicsCPU):
    def __init__(self, pause_on_food=False):
        self.pause_on_food = pause_on_food
        # About 10 ms per tick: the old 20 instructions/ms mapped one tick to
        # 500 ms, too coarse to test 70 ms notes serviced by actual tick IRQs.
        self.timer_divisor = 1000
        self.stop_calls = 0
        self.sleep_calls = []
        super().__init__(ROOT / 'user-programs/programs/snake-screen/build/snake-screen.bin', duration=120)
        out = subprocess.check_output([str(ROOT / 'build-tools/bin/llvm-nm.exe'), '-n',
            str(ROOT / 'user-programs/programs/snake-screen/build/snake-screen.o')], text=True)
        self.draw_entry = 0x80000000 + next(int(line.split()[0], 16)//4
            for line in out.splitlines() if line.split()[-1] == 'draw_screen')
        self.draw_return = None
        self.frames = []
        self.first_command = 0

    def read(self, address):
        if address == 0x08000000:
            return (self.steps // self.timer_divisor) & 0xffffffff
        return super().read(address)

    def write(self, address, value):
        super().write(address, value)
        if self.pause_on_food and address == 0x0a000054 and value == 218:
            self.press('p')

    def syscall(self):
        if not self.kernel_mode and self.r[0] == 29:
            self.sleep_calls.append((self.steps, self.r[1]))
        if not self.kernel_mode and self.r[0] == 25:
            self.stop_calls += 1
        # Snake legitimately waits for KEYBOARD events before start/when paused.
        if not self.kernel_mode and self.r[0] == 15:
            assert self.deadline is None, 'WaitEvent used for graphics completion'
            assert not self.midi_active, 'WaitEvent would prevent timed NoteOff'
        CheckedCPU.syscall(self)

    def step(self):
        if not self.kernel_mode and self.pc == self.draw_entry and self.draw_return is None:
            self.draw_return = self.read(self.sp - 1)
        if not self.kernel_mode and self.draw_return is not None and self.pc == self.draw_return:
            assert self.deadline is None
            self.frames.append((self.screen[:], self.commands[self.first_command:]))
            self.first_command = len(self.commands)
            self.draw_return = None
        super().step()

    def press(self, char):
        self.pending.append(char)
        self.next_key = self.steps

    def key_return(self, char):
        before = self.restores
        self.press(char)
        self.until(lambda: self.restores > before)

    def mute(self):
        before = self.stop_calls
        self.key_return('m')
        self.until(lambda: self.stop_calls > before)
        self.until(lambda: not self.midi_active)

    def next_frame(self):
        count = len(self.frames)
        self.until(lambda: len(self.frames) > count)
        return self.frames[-1]


def board(screen):
    result = {}
    for y in range(8):
        for x in range(12):
            pixels = [screen[(16+y*10+1+dy)*160+20+x*10+1+dx]
                      for dy in range(8) for dx in range(8)]
            if pixels.count(HEAD) == 64: kind = 'head'
            elif pixels.count(BODY) == 64: kind = 'body'
            elif pixels.count(FOOD) == 16 and pixels.count(BACKGROUND) == 48: kind = 'food'
            else:
                assert pixels == [BACKGROUND]*64, (x, y, pixels)
                kind = 'empty'
            result[x, y] = kind
    return result


FONT = [[7,5,5,5,7], [2,6,2,2,7], [7,1,7,4,7], [7,1,7,1,7], [5,5,7,1,1],
        [7,4,7,1,7], [7,4,7,5,7], [7,1,1,1,1], [7,5,7,5,7], [7,5,7,1,7]]


def number(screen, x, value):
    for i, digit in enumerate(f'{value:03d}'):
        for row, bits in enumerate(FONT[int(digit)]):
            for col in range(3):
                expected = BODY if bits & (4 >> col) else BACKGROUND
                for dy in range(2):
                    for dx in range(2):
                        assert screen[(2+row*2+dy)*160+x+i*8+col*2+dx] == expected, (value,i,row,col)


def controls():
    cpu = SnakeCPU()
    screen, commands = cpu.next_frame()
    assert font_commands(cpu, BACKGROUND, {BODY}) == 8
    glyph_pixels(screen, 20, 2, 'S', BODY, BACKGROUND)
    glyph_pixels(screen, 100, 2, 'T', BODY, BACKGROUND)
    initial = board(screen)
    assert initial[6,4] == 'head' and initial[4,4] == initial[5,4] == 'body'
    assert list(initial.values()).count('food') == 1
    number(screen, 28, 0); number(screen, 108, 20)
    silent_output = cpu.output
    assert silent_output.count('Screen Snake\n') == 1
    assert 'SPACE start / pause   WASD move\n' in silent_output
    assert 'P pause / resume     M music + sound on/off\n' in silent_output
    assert '+/- speed (10..150 ms, default 20)\n' in silent_output
    assert 'Q quit   S=score   T=ms/step\n' in silent_output
    cpu.key_return('m')
    assert not cpu.midi_active
    cpu.key_return('M')
    cpu.press(' '); cpu.next_frame()  # start, no immediate move
    screen, commands = cpu.next_frame()
    assert board(screen)[7,4] == 'head'
    assert len(commands) == 3 and all(c[2:4] == (8,8) for c in commands), commands
    cpu.press('w'); screen, commands = cpu.next_frame()
    assert board(screen)[7,3] == 'head'
    cpu.press('p'); screen, _ = cpu.next_frame()
    assert not cpu.midi_active, 'pause must immediately release music'
    music_count = len(cpu.midi_writes)
    frozen = board(screen)
    for expected in range(10, 0, -10):
        cpu.press('+'); screen, _ = cpu.next_frame(); number(screen,108,expected)
    before = len(cpu.frames)
    cpu.press('+')
    end = cpu.steps + 30000
    cpu.until(lambda: cpu.steps >= end)
    assert len(cpu.frames) == before
    assert cpu.output == silent_output, 'normal gameplay printed to terminal'
    for expected in range(20, 151, 10):
        cpu.press('-'); screen, _ = cpu.next_frame(); number(screen,108,expected)
    assert board(screen) == frozen
    assert len(cpu.midi_writes) == music_count, 'music played while paused'
    # Boundary presses do not change speed, and paused keyboard events must not move.
    before = len(cpu.frames)
    cpu.press('-')
    end = cpu.steps + 30000
    cpu.until(lambda: cpu.steps >= end)
    assert len(cpu.frames) == before
    cpu.press('p'); cpu.next_frame()
    cpu.press('q'); cpu.until(lambda: cpu.halted)
    assert 'Quit.' in cpu.output and 'exited with code 0.' in cpu.output
    assert cpu.restores >= 22 and cpu.ticks > 0 and not cpu.snapshots
    assert sum(c[:4] == (0,0,160,100) for c in cpu.commands) == 1
    assert not cpu.midi_active
    font_commands(cpu, BACKGROUND, {BODY})
    print(f'PASS BIN controls/HUD: dirty move=3 rectangles, turns, pause, speed 10..150, quit; {cpu.restores} context restores')


def food_and_wall(pause_on_food=False, muted=False):
    cpu = SnakeCPU(pause_on_food=pause_on_food)
    screen, _ = cpu.next_frame()
    food = next(p for p,k in board(screen).items() if k == 'food')
    body = ((4,4),(5,4),(6,4))
    queue = deque([(body, (1,0), '')]); seen = {(body,(1,0))}
    route = None
    moves = [('w',0,-1),('s',0,1),('a',-1,0),('d',1,0)]
    while queue:
        cells, direction, path = queue.popleft()
        for key, dx, dy in moves:
            if (dx,dy) == (-direction[0],-direction[1]): continue
            target = (cells[-1][0]+dx, cells[-1][1]+dy)
            if not (0 <= target[0] < 12 and 0 <= target[1] < 8): continue
            if target in cells[1:]: continue
            if target == food:
                route = path+key; break
            state = (cells[1:]+(target,), (dx,dy))
            if state not in seen:
                seen.add(state); queue.append((state[0],state[1],path+key))
        if route is not None: break
    assert route
    if muted:
        cpu.key_return('m')
    silent_output = cpu.output
    cpu.press(' '); cpu.next_frame()
    head = (6,4)
    for key in route:
        dx, dy = next((dx,dy) for k,dx,dy in moves if k == key)
        head = head[0]+dx, head[1]+dy
        cpu.press(key); screen, _ = cpu.next_frame()
        assert board(screen)[head] == 'head', (route,key,head)
    number(screen,28,1)
    assert list(board(screen).values()).count('body') == 3
    assert list(board(screen).values()).count('food') == 1
    assert cpu.output == silent_output, 'eating food printed to terminal'
    if pause_on_food:
        cpu.until(lambda: cpu.screen[14*160+18] == HEAD)
        cpu.until(lambda: not cpu.midi_active)
        cpu.press('q')
    cpu.until(lambda: cpu.halted)
    assert ('Quit.' if pause_on_food else 'Hit wall. Game over.') in cpu.output
    assert 'exited with code 0.' in cpu.output
    assert cpu.deadline is None
    notes = [(n,v,t) for n,v,t in cpu.midi_writes if v != 256]
    if muted:
        assert not notes and not cpu.midi_active, notes
        print('PASS muted gameplay: food/score/wall exit without any NoteOn or NoteOff')
        return
    presses = [n for n,v,t in notes if v >= 128]
    assert 84 in presses and not cpu.midi_active, presses
    if not pause_on_food:
        assert presses[-4:] == [67,63,60,48], presses
    print(f'PASS BIN growth/food placement/score/exit; route={route}, pause-on-food={pause_on_food}')
    print('PASS MIDI: food effect, pause cancellation/end jingle and all notes released')


def mute_controls():
    cpu = SnakeCPU()
    cpu.timer_divisor = 1000
    cpu.next_frame()
    cpu.press(' '); cpu.next_frame()
    cpu.until(lambda: bool(cpu.midi_active))
    cpu.mute()
    assert not cpu.midi_active
    cpu.press('p'); cpu.next_frame()
    count = len(cpu.midi_writes)
    cpu.key_return('M')
    end = cpu.steps + 30000
    cpu.until(lambda: cpu.steps >= end)
    assert len(cpu.midi_writes) == count and not cpu.midi_active
    cpu.press('p'); cpu.next_frame()
    cpu.until(lambda: bool(cpu.midi_active))
    # A running unmute resumes the background, not the initial start effect.
    assert cpu.midi_writes[-1][1] == 170
    cpu.until(lambda: any(n == 67 and v == 218 for n,v,t in cpu.midi_writes))
    count = len(cpu.midi_writes)
    cpu.press('m'); cpu.until(lambda: cpu.halted)
    assert not cpu.midi_active and 'exited with code 0.' in cpu.output
    assert not any(v >= 128 for n,v,t in cpu.midi_writes[count:]), 'muting ending started another note'
    print('PASS M: running release, paused toggle stays silent, resume, ending cancellation')


def timed_sleep_input():
    cpu = SnakeCPU()
    cpu.next_frame(); cpu.key_return('m')
    cpu.press(' '); cpu.next_frame()
    cpu.press('p'); cpu.next_frame()
    for _ in range(13):
        cpu.press('-'); cpu.next_frame()
    before = len(cpu.sleep_calls)
    cpu.press('p'); cpu.next_frame()
    cpu.until(lambda: len(cpu.sleep_calls) > before and cpu.kernel_mode and cpu.ie)
    assert cpu.sleep_calls[-1][1] >= 100
    frozen = cpu.user_steps, cpu.system_yields, len(cpu.frames)
    end = cpu.steps + 30000
    cpu.until(lambda: cpu.steps >= end)
    assert (cpu.user_steps, cpu.system_yields, len(cpu.frames)) == frozen, 'snake polled instead of blocking'
    # A key must wake it without another tick; it must not await the move deadline.
    ticks = cpu.ticks
    cpu.next_tick = cpu.steps + 1000000
    before = len(cpu.sleep_calls)
    cpu.key_return('w')
    cpu.until(lambda: len(cpu.sleep_calls) > before and cpu.kernel_mode and cpu.ie, limit=30000)
    assert cpu.ticks == ticks and len(cpu.frames) == frozen[2]
    cpu.press('q'); cpu.until(lambda: cpu.halted, limit=40000)
    assert 'Quit.' in cpu.output and not cpu.midi_active
    print('PASS snake Sleep: no user/idle-Yield polling, early turn wake and Q without tick')


if __name__ == '__main__':
    timed_sleep_input()
    controls()
    food_and_wall()
    food_and_wall(pause_on_food=True)
    mute_controls()
    food_and_wall(muted=True)
