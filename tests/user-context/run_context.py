"""USP/UFP encoding and real BIN smoke test; ideal ISA, not Digital timing."""
import pathlib
import json
import zlib
import base64
import re
import struct
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
HERE = pathlib.Path(__file__).resolve().parent
BUILD = HERE / 'build'
BIN = BUILD / 'contract.bin'
sys.path.insert(0, str(ROOT / 'tests/kernel-events'))
from run_system import SystemCPU
from run_examples import DemoCPU


def encodings():
    tools = ROOT / 'build-tools/bin'
    assembly, expected = [], []
    for mnemonic, opcode, shift in [('lusp', 0x62, 16), ('lufp', 0x63, 16),
                                    ('ousp', 0x64, 8), ('oufp', 0x65, 8)]:
        for reg in range(16):
            assembly.append(f'{mnemonic} r{reg}')
            expected.append(opcode | (reg << shift))
    source = BUILD / 'encodings.s'
    source.write_text('\n'.join(assembly) + '\n', encoding='ascii')
    obj, binary = BUILD / 'encodings.o', BUILD / 'encodings.bin'
    subprocess.run([str(tools / 'llvm-mc.exe'), '-triple=aurora', '-filetype=obj',
                    str(source), '-o', str(obj)], check=True, capture_output=True)
    subprocess.run([str(tools / 'llvm-objcopy.exe'), '-O', 'binary', '--only-section=.text',
                    str(obj), str(binary)], check=True, capture_output=True)
    assert binary.read_bytes() == struct.pack('<64I', *expected)
    disassembly = subprocess.check_output([str(tools / 'llvm-objdump.exe'),
                                          '-d', '--triple=aurora', str(obj)], text=True)
    actual = re.findall(r'\b(lusp|lufp|ousp|oufp)\s+(r\d+)\b', disassembly)
    assert [f'{op} {reg}' for op, reg in actual] == assembly
    for bad in ['lusp', 'lufp 1', 'ousp r16', 'oufp r0, r1']:
        result = subprocess.run([str(tools / 'llvm-mc.exe'), '-triple=aurora'],
                                input=bad + '\n', text=True, capture_output=True)
        assert result.returncode != 0, bad
    print('PASS: all 64 register encodings/disassembly; invalid operands rejected.')


def state_contract():
    cpu = SystemCPU(BIN, BIN, 0)
    patterns = [0, 0xffffffff, 0x82000123, 0x82fedcba]
    cases = 0
    for opcode in range(0x62, 0x66):
        for reg in range(16):
            for flags in range(16):
                cpu.kernel_mode = True
                cpu.pc = cpu.steps = 0
                cpu.ie = bool(flags & 1)
                cpu.next_tick = cpu.next_key = 10**9
                cpu.r = [0x13570000 + i for i in range(16)]
                cpu.r[reg] = patterns[flags % 4]
                cpu.stack = [0x82000123, 0x02001234]
                cpu.frame_pointer = [0x82000456, 0x02005678]
                cpu.mem = {0x02000100: 0x1234, 0x82000100: 0x5678}
                cpu.flags = tuple(bool(flags & (1 << i)) for i in range(4))
                shift = 16 if opcode < 0x64 else 8
                cpu.kernel_rom = (opcode | (reg << shift),)
                expected_r = cpu.r.copy()
                expected_stack = cpu.stack.copy()
                expected_fp = cpu.frame_pointer.copy()
                before_mem, before_flags, before_ie = cpu.mem.copy(), cpu.flags, cpu.ie
                if opcode == 0x62: expected_stack[0] = cpu.r[reg]
                elif opcode == 0x63: expected_fp[0] = cpu.r[reg]
                elif opcode == 0x64: expected_r[reg] = cpu.stack[0]
                else: expected_r[reg] = cpu.frame_pointer[0]
                cpu.step()
                assert cpu.r == expected_r and cpu.stack == expected_stack
                assert cpu.frame_pointer == expected_fp and cpu.mem == before_mem
                assert cpu.flags == before_flags and cpu.ie == before_ie
                assert cpu.kernel_mode and cpu.pc == 1 and cpu.steps == 1
                # The model rejects privilege violations, not a precise fault ISR.
                cpu.kernel_mode = False
                cpu.pc = 0x80000000
                cpu.rom = cpu.kernel_rom
                before = (cpu.r.copy(), cpu.stack.copy(), cpu.frame_pointer.copy(),
                          cpu.pc, cpu.steps, cpu.flags, cpu.ie, cpu.mem.copy())
                try:
                    cpu.step()
                except AssertionError as error:
                    assert 'privileged user-pointer' in str(error)
                else:
                    raise AssertionError('User mode accepted privileged opcode')
                assert before == (cpu.r, cpu.stack, cpu.frame_pointer, cpu.pc,
                                  cpu.steps, cpu.flags, cpu.ie, cpu.mem)
                cases += 1
    print(f'PASS: {cases} ideal ISA cases, flags/IE/KSP/KFP/memory and privilege checks.')


def smoke():
    cpu = DemoCPU('user-context', BIN)
    raw = BIN.read_bytes()
    cpu.kernel_rom = struct.unpack('<' + 'I' * (len(raw) // 4), raw)
    cpu.stack[0], cpu.frame_pointer[0] = 0x82000abc, 0x82000def
    cpu.mem[0x82000120] = 0xdeadbeef
    while not cpu.halted and cpu.steps < 100000:
        assert cpu.kernel_mode and not cpu.ie, 'test must stay kernel-only with IRQs off'
        cpu.step()
    assert cpu.halted and 'USER CONTEXT ALL PASS.\n' in cpu.output, cpu.output
    assert 'FAIL' not in cpu.output
    assert (cpu.stack[0], cpu.frame_pointer[0]) == (0x82000abc, 0x82000def)
    assert cpu.mem[0x82000120] == 0xdeadbeef
    print(cpu.output, end='')
    print(f'PASS: actual ROM BIN, {cpu.steps} instructions, zero IRQs (ideal ISA only).')


def pushu_contract():
    # Lock the model to the saved microcode, not the historical temporary-mode
    # interpretation. Selecting USP does not select banked user addressing.
    project=json.loads((ROOT/'micro-code/kernel/micro_code.mcproj').read_text(encoding='utf-8-sig'))
    raw=zlib.decompress(base64.b64decode(project['microcode']['data']))
    rules={r['name']:r['lsb'] for r in project['preferences']['rules']}
    for opcode in (0x60,0x61):
        sequence=[int.from_bytes(raw[(opcode*16+t)*5:(opcode*16+t+1)*5],'big') for t in range(16)]
        assert any(w & (1<<rules['Kernel Use USP']) for w in sequence)
        assert not any(w & (1<<rules['CLR_KernelMode']) for w in sequence)
    cpu=SystemCPU(BIN,BIN,0)
    for bank in range(4):
        cpu.kernel_mode=True; cpu.ie=False; cpu.memory_bank=bank
        cpu.pc=0; cpu.kernel_rom=(0x60,0x161)
        cpu.stack[0]=0x82000100; cpu.r[0]=0x13579bdf
        cpu.mem[0x82000100]=0
        if bank: cpu.mem[0x82000100+bank*0x400000]=0x2468ace0
        cpu.step(); cpu.step()
        assert cpu.r[1]==0x13579bdf and cpu.stack[0]==0x82000100
        assert cpu.mem[0x82000100]==0x13579bdf and cpu.kernel_mode
        if bank: assert cpu.mem[0x82000100+bank*0x400000]==0x2468ace0
    print('PASS current PUSHU/POPU microcode and physical kernel addressing in all four banks.')


if __name__ == '__main__':
    encodings()
    state_contract()
    pushu_contract()
    smoke()
