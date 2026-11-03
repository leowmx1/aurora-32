"""Validate ASFS2 parent graphs, reserved extents and legacy read-only mount."""
from run_storage import StorageCPU, image_words
from run_files import shell

for kind in ('cycle','parent-range','parent-file','text-length','reserved-overlap'):
    image=image_words()
    directory=next(16+i*40 for i in range(image[4]) if image[16+i*40]==3)
    directory_id=(directory-16)//40+1
    text=next(16+i*40 for i in range(image[4]) if image[16+i*40]==2)
    if kind=='cycle': image[directory+36]=directory_id
    elif kind=='parent-range': image[text+36]=65
    elif kind=='parent-file': image[text+36]=1
    elif kind=='text-length': image[text+2]=image[text+37]+1
    elif kind=='reserved-overlap': image[text+37]=4096
    cpu=StorageCPU(image); cpu.until(lambda:cpu.prompt_count==1)
    shell(cpu,'mount','Invalid storage image.')
    assert cpu.loaded_writes==cpu.storage_writes==0
    print('PASS reject metadata:',kind,flush=True)

image=image_words(); image[1]=1; image[8]=1
image[4]=next(i for i in range(image[4]) if image[16+i*40]!=1)
for i in range(image[4]): image[16+i*40+36:16+i*40+40]=[0]*4
cpu=StorageCPU(image); cpu.until(lambda:cpu.prompt_count==1)
shell(cpu,'ls','calculator')
shell(cpu,'touch test.txt','File operation failed: -9')
shell(cpu,'mkdir test','File operation failed: -9')
assert cpu.storage_writes==0
print('PASS ASFS1 read-only compatibility',flush=True)
