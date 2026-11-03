import struct
from run_storage import ROOT, StorageCPU, image_words
from run_files import shell

words=image_words()
raw=(ROOT/'tests/storage/build/fs_contract.bin').read_bytes()
code=list(struct.unpack('<'+'I'*(len(raw)//4),raw))
exe=[0x41455845,1,16,22+len(code),0x80000000,3,2,1,16,6,1,0,0,0,0,0,
     0x80000000,22,len(code),len(code),1,0]+code
words[17]=len(words); words[18]=words[53]=len(exe); words[19]=sum(exe)&0xffffffff
words.extend(exe); words[3]=len(words)
cpu=StorageCPU(words)
cpu.until(lambda:cpu.prompt_count==1)
shell(cpu,'run calculator','FS CONTRACT PASS')
assert 'FS FAIL' not in cpu.output
shell(cpu,'verify docs/notes.txt','File verified.')
shell(cpu,'mount','Storage mounted.')
print('PASS file syscalls: traversal, enumeration, EOF, invalid pointers/counts, executable protection, save/checksum')

# Metadata-only move/delete, stable IDs/extents, cyclic move rejection, hole reuse.
cpu=StorageCPU(); cpu.until(lambda:cpu.prompt_count==1)
shell(cpu,'mkdir work','Done.')
shell(cpu,'mkdir work/nested','Done.')
def entry(name):
    for i in range(cpu.image[4]):
        at=16+i*40
        if cpu.image[at] and ''.join(chr(c) for c in cpu.image[at+4:at+36] if c)==name:
            return i+1,at
    raise AssertionError(name)
id,at=entry('notes.txt'); offset=cpu.image[at+1]; cap=cpu.image[at+37]
before=cpu.image[cpu.image[7]:].copy(); writes=len(cpu.storage_write_addresses)
shell(cpu,'mv docs/notes.txt work/nested/moved.txt','Done.')
assert entry('moved.txt')[0]==id and cpu.image[at+1]==offset and cpu.image[at+37]==cap
assert cpu.image[cpu.image[7]:]==before
assert all(a<cpu.image[7] for a in cpu.storage_write_addresses[writes:])
assert 1<=len(cpu.storage_write_addresses)-writes<=33
for command in ('mv work/nested/moved.txt work','mv work/moved.txt work/nested'):
    writes=len(cpu.storage_write_addresses)
    shell(cpu,command,'Done.')
    assert len(cpu.storage_write_addresses)-writes==1, 'same-name move should update only parent'
    assert cpu.image[cpu.image[7]:]==before
shell(cpu,'mv work work/nested','File operation failed: -1')
shell(cpu,'mount','Storage mounted.')
writes=len(cpu.storage_write_addresses)
shell(cpu,'rm work/nested/moved.txt','Done.')
assert len(cpu.storage_write_addresses)-writes==1
shell(cpu,'touch work/reused.txt','Done.')
newid,newat=entry('reused.txt')
assert cpu.image[newat+1]==offset, 'deleted extent was not reused'
assert cpu.image[newat+2]==0
shell(cpu,'mount','Storage mounted.')
# Moving an older directory under a newer directory must remain valid.
shell(cpu,'mv docs work/nested','Done.')
shell(cpu,'mount','Storage mounted.')
shell(cpu,'cat work/nested/docs/welcome.txt','Welcome to Aurora-32!')
print('PASS same-name move/delete each write 1 metadata word; stable payloads/IDs; freed space reused; cycle rejected')
