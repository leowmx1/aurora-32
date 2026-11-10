"""Exercise the executable-publication syscall using real compiled user code."""
import pathlib
import struct
import sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tests/storage'))
from run_storage import StorageCPU,image_words
from run_compiler import entry,compile_source,run_generated

raw=(ROOT/'tests/compiler/build/install_contract.bin').read_bytes()
code=list(struct.unpack('<'+'I'*(len(raw)//4),raw))
exe=[0x41455845,1,16,22+len(code),0x80000000,3,2,1,16,6,1,0,0,0,0,0,
     0x80000000,22,len(code),len(code),1,0]+code
words=image_words(); words[17]=len(words); words[18]=words[53]=len(exe); words[19]=sum(exe)&0xffffffff
words.extend(exe); words[3]=len(words)
c=StorageCPU(words); c.until(lambda:c.prompt_count==1); c.command_done('run calculator')
assert 'INSTALL CONTRACT PASS' in c.output and 'INSTALL FAIL' not in c.output,c.output[-3000:]
assert entry(c.image,'invalid') is None
c.command_done('mount'); assert 'Storage mounted.' in c.output
run_generated(c,'docs/installed',exit_code=23)
print('PASS Install bounds, create-only paths, AEXE layout, text-save protection, readback and execution',flush=True)

# Refuse read-only Storage and leave no visible executable even after remount.
c=StorageCPU(); c.until(lambda:c.prompt_count==1); c.run_program('compiler','Source > ')
c.pending.extend('tinyc/hello.c\r'); c.until(lambda:'Output > ' in c.output)
original=c.image.copy(); c.storage_writable=False; n=c.prompt_count
c.pending.extend('readonly\rn\r'); c.until(lambda:'name=compiler slot=' in c.output and ' local foreground\n\n' in c.output,limit=20000000)
assert 'File error: -7' in c.output and c.image==original,c.output[-2000:]
print('PASS read-only Storage: install fails without publishing or replacing a file',flush=True)
