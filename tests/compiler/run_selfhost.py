"""Real kernel + Aurora binaries; no host compiler in the bootstrap chain.
Run with PyPy for speed. This is ideal ISA/device execution, not Digital timing.
"""
import hashlib
import json
import pathlib
import struct
import sys
import time

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tests/storage'))
from run_storage import StorageCPU, image_words
from run_compiler import entry, run_generated, process_exited
from run_startup import PoisonStorageCPU


def payload(image, name):
    at = entry(image, name)
    assert at is not None, name
    return image[image[at+1]:image[at+1]+image[at+2]]


def compile_file(c, compiler, source, output):
    start = len(c.output)
    c.pending.extend('run ' + compiler + '\r')
    c.until(lambda: 'Source > ' in c.output[start:], limit=12000000)
    c.pending.extend(source + '\r')
    c.until(lambda: 'Output > ' in c.output[start:], limit=30000000)
    n = c.prompt_count
    c.pending.extend(output + '\ry\r')
    concurrent = compiler == 'compiler' and source == 'tinyc/compiler.c'
    if concurrent:
        c.until(lambda:c.prompt_count>n,limit=8000000)
        c.run_program('calculator','>> ')
        c.command('8*(3+2)','Result: 40')
    begin = c.steps
    report_at = begin + 10000000
    started = time.monotonic()
    while not process_exited(c.output[start:], compiler):
        assert c.steps-begin < 1500000000, c.output[-2000:]
        c.step()
        if c.steps >= report_at:
            print(f'{compiler} -> {output}: {c.steps-begin:,} instructions, '
                  f'{time.monotonic()-started:.1f}s host time', flush=True)
            report_at += 10000000
    assert entry(c.image,output) and 'exited with code 0.' in c.output[start:],c.output[start:]
    progress=c.output[start:]
    assert '  Function: ' not in progress and 'Compiled ' not in progress,progress
    print(c.output[start:][-500:], flush=True)
    if concurrent:
        assert 'name=compiler slot=1 bank=0 local background' in c.output[start:]
        c.command('7*9','Result: 63')
        c.quit_key()
        print('PASS compiler self-hosts in background while bank 1 calculator remains interactive',flush=True)
    return payload(c.image, output)


def main():
    c = PoisonStorageCPU(image_words())
    c.instruction_limit = 4000000000
    c.until(lambda: c.prompt_count == 1)
    original = (ROOT/'user-programs/programs/compiler/compiler.c').read_text()
    assert ''.join(map(chr, payload(c.image, 'compiler.c'))) == original
    first = compile_file(c, 'compiler', 'tinyc/compiler.c', 'compiler2')
    build = ROOT/'tests/compiler/build'
    build.mkdir(exist_ok=True)
    (build/'stage2.bin').write_bytes(struct.pack('<'+'I'*(len(first)-22), *first[22:]))
    second = compile_file(c, 'compiler2', 'tinyc/compiler.c', 'compiler3')
    (build/'stage3.bin').write_bytes(struct.pack('<'+'I'*(len(second)-22), *second[22:]))
    if first != second:
        print('Differences (AEXE word, stage2, stage3):',
              [(i,hex(a),hex(b)) for i,(a,b) in enumerate(zip(first,second)) if a!=b][:30],flush=True)
    assert first == second, 'Stage 2 and 3 differ'
    # Both complete AEXE and raw code must be equal, not merely checksums.
    raw = struct.pack('<'+'I'*(len(first)-22), *first[22:])
    build = ROOT/'tests/compiler/build'
    build.mkdir(exist_ok=True)
    (build/'selfhost.bin').write_bytes(raw)
    report = dict(words=len(first)-22, sha256=hashlib.sha256(raw).hexdigest(),
                  source_sha256=hashlib.sha256(original.encode()).hexdigest(),
                  instructions=c.steps, ticks=c.ticks,
                  validation='Actual kernel/binaries on ideal ISA/devices, not Digital')
    (build/'selfhost.json').write_text(json.dumps(report, indent=2)+'\n')
    compile_file(c, 'compiler3', 'tinyc/arrays.c', 'native')
    run_generated(c, expected='Sorted:\n1\n2\n3\n4\n5\n')
    c.command_done('mount')
    c.command_done('verify compiler2')
    assert 'File verified.' in c.output[-2000:]
    print('PASS self-host fixed point: stage 2 == stage 3 byte-for-byte', report, flush=True)


if __name__ == '__main__':
    main()
