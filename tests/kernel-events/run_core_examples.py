"""Small real-BIN regressions for existing apps after compiler optimization."""
from run_examples import DemoCPU, ROOT


def calculator(name):
    cpu = DemoCPU(name, ROOT / f'user-programs/programs/calculator/build/{name}.bin')
    cpu.until(lambda: cpu.output.endswith('>> '))
    for text, result in [('8*3+2', 26), ('-8*(2+3-(-3-1+5))', -32)]:
        cpu.command(text, f'Result: {result}\n')
        cpu.until(lambda: cpu.output.endswith('>> '))
    cpu.pending.append('Q')
    cpu.until(lambda: cpu.halted)
    assert 'Quit.' in cpu.output and 'exited with code 0.' in cpu.output, cpu.output
    print(f'PASS {name}: expressions, callback input, exit; {cpu.ticks} ticks', flush=True)


def guess_number():
    cpu = DemoCPU('guess', ROOT / 'user-programs/programs/guess-number/build/guess-number.bin')
    cpu.until(lambda: cpu.output.endswith('Lower bound > '))
    cpu.command('-3', 'Upper bound > ')
    cpu.command('-3', 'Guess > ')
    cpu.command('-3', 'Correct! The number was -3.')
    cpu.until(lambda: cpu.halted)
    assert 'exited with code 0.' in cpu.output, cpu.output
    print(f'PASS guess-number: negative inclusive range, win; {cpu.ticks} ticks', flush=True)


if __name__ == '__main__':
    calculator('calculator')
    calculator('calculator-trace')
    guess_number()
