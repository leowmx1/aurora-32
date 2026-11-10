"""Compiler first-prompt budget and noinit safety; ideal ISA, not Digital timing."""
import struct
import sys
from run_native import CompilerCPU, ROOT
sys.path.insert(0, str(ROOT/'tests/storage'))
from run_storage import StorageCPU


class PoisonStorageCPU(StorageCPU):
    def read(self, address):
        physical=self.user_address(address) if not self.kernel_mode else address
        if 0x81000000 <= address < 0x82000000 and physical not in self.mem:
            return 0xdeadbeef
        return super().read(address)


class PoisonCompilerCPU(CompilerCPU):
    def read(self, address):
        if 0x81000000 <= address < 0x82000000 and address not in self.mem:
            return 0xdeadbeef
        return super().read(address)


def main():
    seed=ROOT/'user-programs/programs/compiler/build/compiler.bin'
    c=PoisonStorageCPU()
    c.until(lambda:c.prompt_count==1)
    for attempt in range(2):
        start=len(c.output)
        c.pending.extend('run compiler\r')
        c.until(lambda:not c.kernel_mode and c.ie,limit=1000000)
        entered=c.steps
        c.until(lambda:'Source > ' in c.output[start:],limit=80000)
        latency=c.steps-entered
        assert latency<30000,latency
        n=c.prompt_count; c.pending.extend('q\r')
        c.until(lambda:c.prompt_count>n)
        print(f'PASS compiler launch {attempt+1}: Source prompt in {latency} instructions, dirty URAM',flush=True)

    source='''int a;
int scratch[4096] __attribute__((section(".noinit")));
int b[2];
int main() { print(a); print(b[0]); print(b[1]); scratch[0]=42; print(scratch[0]); return 0; }
'''
    cpu=PoisonCompilerCPU(seed,source)
    while cpu.exit_code is None: cpu.step()
    assert cpu.exit_code==0 and cpu.compiled,cpu.output
    out=ROOT/'tests/compiler/build/noinit.bin'
    out.write_bytes(struct.pack('<'+'I'*len(cpu.compiled),*cpu.compiled))
    generated=PoisonCompilerCPU(out,'')
    while generated.exit_code is None: generated.step()
    assert generated.output=='0\n0\n0\n42\n',generated.output
    # Only scratch[0] may have been written; default globals on both sides clear.
    assert all(0x81000401+i not in generated.mem for i in range(1,4096))
    print('PASS noinit skips workspace, normal globals still zero on poisoned RAM',flush=True)
    for declaration in [
        'int x __attribute__((section(".noinit")));',
        'int x[2] __attribute__((section(".other")));',
        'int x[2] __attribute__((aligned(16)));',
    ]:
        cpu=PoisonCompilerCPU(seed,declaration+'int main(){return 0;}')
        while cpu.exit_code is None: cpu.step()
        assert cpu.exit_code==1 and cpu.compiled is None,cpu.output
    print('PASS unsupported attributes rejected without installation',flush=True)


if __name__=='__main__': main()
