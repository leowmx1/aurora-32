"""Fast isolated Aurora-binary compiler harness (ideal syscall contract).
The source is consumed only by the Aurora compiler, never by a host parser.
Full-kernel/tick integration is separately covered by run_selfhost.py.
"""
import pathlib
import struct
import sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tests/kernel-events'))
from run_target import CPU, signed

class CompilerCPU(CPU):
    def __init__(self, binary, source):
        super().__init__(binary,0)
        self.input_text=list('source.c\rresult\ry\r')
        self.source=list(map(ord,source))
        self.output=''
        self.compiled=None
        self.instruction_limit=500000000
        self.exit_code=None

    def emit(self,text): self.output+=text

    def syscall(self):
        req,a,b,c=self.r[:4]
        if req==15:
            assert self.saved is None and self.input_text, self.output
            self.saved=(self.r.copy(),self.pc,self.flags,self.sp,self.fp)
            if self.trampoline: self.push(self.trampoline)
            self.r[0]=0xa5000100|ord(self.input_text.pop(0))
            self.pc=self.active_callback
        elif req in (4,16,17,18,36): self.r[0]=0
        elif req==31: self.r[0]=1
        elif req==34:
            for i in range(37): self.write(b+i,0)
            self.write(b,1); self.write(b+1,2); self.write(b+2,len(self.source))
            self.write(b+3,65536); self.r[0]=0
        elif req==32:
            offset=self.read(a+1); dest=self.read(a+2); n=self.read(a+3)
            assert n<=64
            data=self.source[offset:offset+n]
            for i,word in enumerate(data): self.write(dest+i,word)
            self.r[0]=len(data)
        elif req==35:
            assert 0<c<=131072
            self.compiled=[self.read(b+i) for i in range(c)]; self.r[0]=2
        elif req==8:
            self.exit_code=signed(a)
        else: super().syscall()

    def step(self):
        # Callback delivery happens at blocking WaitEvent above. Avoid the
        # base game harness's synthetic periodic '!' keyboard injection.
        cb=self.callback
        self.active_callback=cb
        word=self.read(self.pc)
        if word & 255 == 0x27:
            x=self.r[(word>>16)&15]; shift=signed(self.r[(word>>12)&15])
            assert -31<=shift<=31
            value=(x<<shift if shift>=0 else x>>-shift)&0xffffffff
            self.r[(word>>8)&15]=value
            self.flags=(value==0,bool(value&0x80000000),False,False)
            self.pc+=1; self.steps+=1
            return
        self.callback=0
        super().step()
        if not self.callback: self.callback=cb


def compile_binary(binary,source):
    c=CompilerCPU(binary,source)
    while c.exit_code is None: c.step()
    assert c.exit_code==0 and c.compiled is not None,c.output
    return c.compiled,c


if __name__=='__main__':
    source=(ROOT/'user-programs/programs/compiler/compiler.c').read_text()
    build=ROOT/'tests/compiler/build'; build.mkdir(exist_ok=True)
    binary=ROOT/'user-programs/programs/compiler/build/compiler.bin'
    previous=None
    for stage in (2,3):
        code,c=compile_binary(binary,source)
        print(f'Stage {stage}: {len(code)} words, {c.steps} instructions',flush=True)
        binary=build/f'fast-stage{stage}.bin'
        binary.write_bytes(struct.pack('<'+'I'*len(code),*code))
        if previous is not None:
            differences=[(i,hex(a),hex(b)) for i,(a,b) in enumerate(zip(previous,code)) if a!=b]
            assert len(previous)==len(code) and not differences,differences[:30]
        previous=code
    print('PASS isolated native bootstrap fixed point')
