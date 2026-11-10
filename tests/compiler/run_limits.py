"""Isolated emitted-code ABI and capacity checks; see run_native.py scope."""
import struct
from run_native import ROOT, CompilerCPU, compile_binary

seed=ROOT/'user-programs/programs/compiler/build/compiler.bin'

class BusyCompilerCPU(CompilerCPU):
    def __init__(self):
        super().__init__(seed,'int main() { print(42); return 0; }')
        self.busy={31:2,32:2,34:2,35:3}
        self.yields=0
    def syscall(self):
        req=self.r[0]
        if self.busy.get(req,0):
            self.busy[req]-=1
            self.r[0]=0xfffffffb # EBUSY, no filesystem side effects
            return
        if req==4: self.yields+=1
        super().syscall()

retry=BusyCompilerCPU()
while retry.exit_code is None: retry.step()
assert retry.exit_code==0 and retry.compiled and retry.yields==9,retry.output
assert not any(retry.busy.values())
print('PASS compiler filesystem EBUSY: Open/Info/Read/Install retry with Yield, then save',flush=True)

source='''
int f(int a,int b,int c,int d,int e,int f,int g,int h) { return a+b+c+d+e+f+g+h; }
int *choose(int *p) { return p; }
char *msg() { return "pointer-ok"; }
int main() {
    int x=5; *choose(&x)=7; print(x);
    print(f(1,2,3,4,5,6,7,8)); print(msg()); return 0;
}
'''
code,c=compile_binary(seed,source)
out=ROOT/'tests/compiler/build/extra.bin'
out.write_bytes(struct.pack('<'+'I'*len(code),*code))
r=CompilerCPU(out,'')
while r.exit_code is None: r.step()
assert r.exit_code==0 and r.output=='7\n36\npointer-ok\n',r.output
print('PASS eight arguments, pointer and string returns',flush=True)

code,c=compile_binary(seed,'int main() { int frame[64]; return 0; }')
assert 0x823fff00 in code and 0x82ffff00 not in code
out.write_bytes(struct.pack('<'+'I'*len(code),*code))
r=CompilerCPU(out,'')
# Enter the generated main with a nearly-full logical 22-bit user stack.
while (r.read(r.pc)&255)!=0x34: r.step()
r.step()
r.sp=0x823fff00-32
while r.exit_code is None: r.step()
assert r.exit_code!=0 and r.sp<0x82400000,(r.exit_code,r.output,hex(r.sp))
print('PASS generated stack guard rejects bank-boundary overflow before frame allocation',flush=True)

cases=[
    ('int main() { int x=0; '+ 'if(x)x=x+1;'*5900+' return 0; }','Generated code exceeds'),
    ('int main() { int huge[65537]; return 0; }','frame exceeds'),
    (''.join(f'int big{i}[131072];\n' for i in range(9))+'int main() { return 0; }', 'Global data exceeds'),
    ('int main() { int n=2; int a[n]; return 0; }','Array size'),
    ('int main() { return 0; }'+' '*65536,'at most 65536'),
]
for source,message in cases:
    c=CompilerCPU(seed,source)
    while c.exit_code is None: c.step()
    assert c.exit_code==1 and c.compiled is None and message in c.output,c.output
    print('PASS rejected without installing:',message,flush=True)
