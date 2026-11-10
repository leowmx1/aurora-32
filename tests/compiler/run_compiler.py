"""Actual kernel + native compiler BIN + generated native EXE, ideal ISA/devices.
No host compiler substitutes for the Aurora compiler under test.
"""
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT/'tests/storage'))
from run_storage import StorageCPU, image_words


def entry(image, name):
    for i in range(image[4]):
        at=image[5]+i*image[6]
        if image[at] and ''.join(chr(x) for x in image[at+4:at+36] if x)==name:
            return at
    return None


def process_exited(text, name):
    # Background() returns a shell prompt before compilation has finished.
    marker='\n  name='+name+' slot='
    if marker not in text: return False
    tail=text[text.index(marker):]
    return ' background\n\n' in tail or ' foreground\n\n' in tail


def compile_source(source, output='native', expected_error=None, image=None, background_answer='n'):
    words=image_words() if image is None else image.copy()
    at=entry(words,'notes.txt'); off=words[at+1]
    data=list(map(ord,source)); assert len(data)<=words[at+37]
    words[off:off+len(data)]=data; words[at+2]=len(data); words[at+3]=sum(data)&0xffffffff
    before=words.copy()
    c=StorageCPU(words); c.until(lambda:c.prompt_count==1)
    c.run_program('compiler','Source > ')
    c.pending.extend('docs/notes.txt\r')
    c.until(lambda:'Output > ' in c.output,limit=8000000)
    n=c.prompt_count; start=len(c.output); c.pending.extend(output+'\r'+background_answer+'\r')
    c.until(lambda:process_exited(c.output[start:],'compiler'),limit=20000000)
    c.until(lambda:c.prompt_count>n)
    if expected_error:
        assert expected_error in c.output, c.output[-2500:]
        assert c.image==before and c.storage_writes==0, 'failed compile changed storage'
    else:
        assert 'exited with code 0.' in c.output[start:], c.output[-2500:]
        if background_answer.lower() not in ('y',''):
            assert 'Compiled ' in c.output, c.output[-2500:]
        assert entry(c.image,output), 'missing generated EXE'
    return c


def run_generated(c, output='native', expected='', exit_code=0):
    start=len(c.output); c.command_done('run '+output)
    report=c.output[start:]
    assert expected in report, report
    assert f'exited with code {exit_code}.' in report, report
    assert not c.halted
    return report


def main():
    c=compile_source('''int twice(int value);
int twice(int value) { return value*2; }
int main() { print(twice(21)); return 0; }''')
    # A background progress line may follow the already-printed shell prompt.
    progress=[s for s in c.output.replace('aurora:/ > ','').splitlines() if s.startswith('  Function: ')]
    assert progress==['  Function: twice','  Function: main'],c.output
    assert c.output.index('Compiling...')<c.output.index(progress[0])<c.output.index('Saving executable...')
    run_generated(c,expected='42\n')
    bad=compile_source('int main() { int broken = ; }',expected_error='Expected an integer expression')
    assert bad.output.index('  Function: main')<bad.output.index('Compile error at line')
    print('PASS live function progress: definition names/order, prototypes skipped, error location retained',flush=True)

    c=compile_source('int main() { print("Hello, native!"); print(2+3*4); return 0; }')
    run_generated(c,expected='Hello, native!\n14\n')
    c.command_done('mount'); assert 'Storage mounted.' in c.output
    c.command_done('verify native'); assert 'File verified.' in c.output
    again=StorageCPU(c.image.copy()); again.until(lambda:again.prompt_count==1)
    run_generated(again,expected='Hello, native!\n14\n')
    print('PASS machine-native compile/save/load, mount, verify and reset persistence',flush=True)

    source='''int main() {
      int i=0; int total=0;
      while(i<10) { i=i+1; if(i==3) continue; if(i==8) break; total=total+i; }
      print(total); print(-8*(2+3-(-3-1+5))); print(-7/3); print(-7%3);
      print(7/-3); print(7%-3); print(-2147483648/-1);
      print(0 && (1/0)); print(1 || (1/0)); print(!0); print(!5);
      if(total==25 && i==8) print("branch-ok"); else print("bad");
      return 7;
    }'''
    c=compile_source(source)
    run_generated(c,expected='25\n-32\n-2\n-1\n-2\n1\n-2147483648\n0\n1\n1\n0\nbranch-ok\n',exit_code=7)
    print('PASS arithmetic, signed div/mod, wrap, short circuit, branches and loop fixups',flush=True)

    c=compile_source('int main() { int i=0; while(i<3) { int j=0; while(j<3) { print(i*10+j); j=j+1; } i=i+1; } return 0; }')
    run_generated(c,expected='0\n1\n2\n10\n11\n12\n20\n21\n22\n')
    print('PASS nested loops and repeated variable initialization',flush=True)

    c=compile_source('''
      int g[8];
      int sum(int *p,int n) { int s=0; int i=0; while(i<n) { s=s+p[i]; i=i+1; } return s; }
      int fact(int n) { if(n<2) return 1; int keep=n; return keep*fact(n-1); }
      int odd(int n);
      int even(int n) { if(n==0) return 1; return odd(n-1); }
      int odd(int n) { if(n==0) return 0; return even(n-1); }
      int set(int *p,int x) { *p=x; return *p; }
      int main() {
        int a[4]; a[0]=3; a[1]=5; a[2]=7;
        print(sum(a,4)); print(fact(6)); print(even(12)); print(odd(9));
        int *p=&a[1]; print(set(p,11)); print(a[1]);
        int **pp=&p; **pp=13; print(a[1]);
        g[2]=99; print(g[2]); print(g[0]);
        print((0xff & 15) | 32); print(~0); print(3^5);
        { int temp=42; print(temp); } { int temp=43; print(temp); }
        return 0;
      }''')
    run_generated(c,expected='15\n720\n1\n1\n11\n11\n13\n99\n0\n47\n-1\n6\n42\n43\n')
    print('PASS functions, recursion, forward calls, arrays, pointers, block lifetimes and bitwise ops',flush=True)

    for source, error in [
        ('int main() { int x = ; }','Expected an integer expression'),
        ('int main() { int x=1\nprint(x); }','Unexpected token'),
        ('int main() { print(missing); }','Unknown variable'),
        ('int main() { int x; int x; }','Duplicate local'),
        ('int main() { break; }','requires a loop'),
        ('int main() { print(4294967296); }','exceeds 32 bits'),
        ('int main() { print("unterminated); }','Unclosed literal'),
        ('/* missing','Unclosed comment'),
        ('int main() { int a[0]; }','Array size'),
        ('int main() { print('+ '(' * 100 + '1'+ ')' * 100 +'); }','nesting exceeds'),
        ('int main() { int a[3]; a=1; }','scalar lvalue'),
        ('int f(int a); int main() { return f(1); }','not defined'),
        ('int f(int a) { return a; } int main() { return f(); }','Wrong number'),
        ('int main() { int *p; p[0][1]=0; }','Indexing needs'),
        ('#define X 1\nint main() { return 0; }','preprocessor'),
    ]:
        compile_source(source,expected_error=error)
    compile_source('int main() { print(1); return 0; }',output='calculator',expected_error='Output already exists')
    compile_source('int main() { print(1); return 0; }',output='docs/notes.txt',expected_error='Output already exists')
    print('PASS malformed/unsupported input and output collisions leave storage unchanged',flush=True)

    c=compile_source('int main() { print(10/0); return 0; }')
    run_generated(c,expected='Runtime error:',exit_code=2)
    print('PASS generated division-by-zero exits safely to shell',flush=True)

    c=compile_source('int main() { print(sleep(1)); print(sleep(-1)); return 0; }')
    run_generated(c,expected='0\n-1\n')
    print('PASS Sleep reserved-register ABI and invalid duration',flush=True)

    c=compile_source('int main() { print("Keys ready"); int c=0; while(c!=\'q\') { c=getchar(); if(c!=\'q\') putchar(c); } return 0; }')
    c.run_program('native','Keys ready\n')
    ticks=c.ticks; start=len(c.output); n=c.prompt_count; c.pending.extend('abcXYZq')
    c.until(lambda:c.prompt_count>n,limit=8000000)
    assert 'abcXYZ' in c.output[start:] and c.ticks>ticks, c.output[-2000:]
    assert 'exited with code 0.' in c.output[start:]
    print('PASS generated callback/WaitEvent input with tick scheduling',flush=True)


if __name__=='__main__': main()
