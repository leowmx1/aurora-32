"""Packaged sources compile on Aurora and execute through the real kernel."""
from run_compiler import ROOT, compile_source, run_generated, entry
from run_storage import image_words
from run_storage import StorageCPU

image=image_words()
assert entry(image,'fibonacci.c') is None and entry(image,'gcd.c') is None
for name,expected in (
    ('hello.c','Hello from Aurora-32!\n1\n4\n9\n16\n25\n'),
    ('recursion.c','720\n55\n'),
    ('arrays.c','Sorted:\n1\n2\n3\n4\n5\n'),
    ('pointers.c','Word string length (6):\n6\nchar stores a full 32-bit word:\n1000\n'),
):
    at=entry(image,name)
    assert image[at]==2, 'example was accidentally precompiled'
    data=image[image[at+1]:image[at+1]+image[at+2]]
    source=''.join(map(chr,data))
    assert source==(ROOT/'storage/texts'/name).read_text()
    c=compile_source(source)
    run_generated(c,expected=expected)
    print('PASS packaged source:',name,flush=True)

at=entry(image,'keys.c')
source=''.join(map(chr,image[image[at+1]:image[at+1]+image[at+2]]))
c=compile_source(source)
c.run_program('native','Type keys; q exits.')
n=c.prompt_count; start=len(c.output); c.pending.extend('ABC123q')
c.until(lambda:c.prompt_count>n,limit=8000000)
assert 'ABC123\nBye!' in c.output[start:],c.output[-2000:]
print('PASS packaged keys.c: callback-driven blocking input',flush=True)

# CRLF remote compiler prompts, then a generated blocking-input application.
c=StorageCPU(); c.until(lambda:c.prompt_count==1)
def net(): return ''.join(map(chr,c.network_out)).replace('\r','')
prompt='aurora:remote:/ > '
c.until(lambda:net().endswith(prompt))
c.network_in.extend(map(ord,'run compiler\r\n'))
c.until(lambda:'Source > ' in net(),limit=12000000)
c.network_in.extend(map(ord,'tinyc/keys.c\r\n'))
c.until(lambda:'Output > ' in net(),limit=8000000)
n=net().count(prompt); c.network_in.extend(map(ord,'netkeys\r\ny\r\n'))
c.until(lambda:'name=compiler slot=' in net() and ' remote background\n\n' in net(),limit=20000000)
assert entry(c.image,'netkeys') and 'exited with code 0.' in net()
assert 'Compiled ' not in net() and 'Source > ' not in c.output
c.network_in.extend(map(ord,'run netkeys\r\n'))
c.until(lambda:'Type keys; q exits.' in net(),limit=8000000)
c.command_done('echo local-still-free')
assert 'local-still-free' in c.output and 'local-still-free' not in net()
n=net().count(prompt); start=len(net())
c.network_in.extend(map(ord,'XYZ987q'))
c.until(lambda:net().count(prompt)>n,limit=8000000)
assert 'XYZ987\nBye!' in net()[start:] and 'XYZ987' not in c.output,net()[-2000:]
print('PASS remote compiler + generated input program, CRLF and local-console isolation',flush=True)
