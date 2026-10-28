"""Actual kernel/user BIN event regression; ideal ISA, not Digital timing."""
import subprocess
from collections import deque
from run_examples import DemoCPU, ROOT


def symbols():
    out = subprocess.check_output([str(ROOT / 'build-tools/bin/llvm-nm.exe'), '-n',
                                   str(ROOT / 'kernel/build/kernel.o')], text=True)
    return {line.split()[2]: int(line.split()[0], 16)//4
            for line in out.splitlines() if len(line.split()) == 3}


class CheckedCPU(DemoCPU):
    def __init__(self, binary):
        super().__init__('events', binary)
        self.entries = symbols()
        self.inject = deque()
        self.snapshots = deque()
        self.restoring = False
        self.restores = self.user_steps = self.system_yields = 0

    def syscall(self):
        if self.kernel_mode and self.r[0] == 4:
            self.system_yields += 1
        super().syscall()

    def step(self):
        if self.ie and self.inject:
            kind, char = self.inject.popleft()
            if kind == 1:
                self.key = char
                self.interrupt(self.read(0x03000001)); self.keys += 1
            else:
                self.network_word = char
                self.interrupt(self.read(0x03000002)); self.network_irqs += 1
        if self.pc == self.entries['prepare_user_callback']:
            base = self.r[0] - 18
            frame = [self.read(base+i) for i in range(18)]
            self.snapshots.append((frame, self.stack[0], self.frame_pointer[0]))
        if self.pc == self.entries['take_callback_restore_stack_top']:
            self.restoring = True
        if not self.kernel_mode:
            self.user_steps += 1
            if self.restoring:
                frame, sp, fp = self.snapshots.popleft()
                z, n, v, c = self.flags
                flags = int(z) | int(c)<<1 | int(n)<<2 | int(v)<<3
                assert (self.pc, flags, self.r, self.sp, self.fp) == (frame[1], frame[0], frame[2:], sp, fp), 'callback context mismatch'
                self.restoring = False
                self.restores += 1
        super().step()


def contract(period):
    cpu = CheckedCPU(ROOT / 'tests/kernel-events/build/event_contract.bin')
    cpu.tick_period = cpu.next_tick = period
    cpu.until(lambda: 'P0 ready\n' in cpu.output)
    # A mixed burst while delivery is deferred: exactly 63 admitted, 7 lost.
    cpu.inject.extend((i % 2 + 1, i + 1) for i in range(70))
    cpu.until(lambda: 'P1 ready\n' in cpu.output)
    cpu.inject.extend((i % 2 + 1, i + 101) for i in range(10))
    cpu.until(lambda: 'P2 sleeping\n' in cpu.output)
    # Drain sticky wake credit and enter an actual blocked WaitEvent.
    deadline = cpu.steps + 30000
    cpu.until(lambda: cpu.steps >= deadline)
    before = cpu.user_steps, cpu.system_yields
    deadline = cpu.steps + 50000
    cpu.until(lambda: cpu.steps >= deadline)
    assert (cpu.user_steps, cpu.system_yields) == before, 'blocked user scheduled or idle syscall spinning'
    # No tick is due: idle SYSTEM must hand off immediately on keyboard wake.
    cpu.next_tick = cpu.steps + 100000
    cpu.inject.append((1, ord('Z')))
    cpu.until(lambda: cpu.halted, limit=50000)
    assert 'Event contract PASS' in cpu.output and 'exited with code 0.' in cpu.output, cpu.output
    assert cpu.restores == 69 and not cpu.snapshots
    assert cpu.system_yields > 100 and cpu.ticks > 0
    print(f'PASS events tick={period}: FIFO, overflow, unsubscribe, nested defer, sleep/wake without tick, 69 full-context restores, {cpu.system_yields} system yields', flush=True)


if __name__ == '__main__':
    contract(10007)
    contract(3001)
