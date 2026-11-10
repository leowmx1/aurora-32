"""Actual production BIN: ASFS2 management, editor persistence, quickrun."""
from run_storage import StorageCPU, image_words


def shell(cpu, command, expected):
    start = len(cpu.output)
    cpu.command_done(command)
    assert expected in cpu.output[start:], cpu.output[start:]


def editor(cpu, command, expected='editor'):
    start = len(cpu.output)
    cpu.pending.extend(command + '\r')
    cpu.until(lambda: expected in cpu.output[start:] and
              (cpu.output.endswith('editor > ') or cpu.output.endswith('editor* > ')), limit=4000000)


def main():
    cpu = StorageCPU()
    cpu.until(lambda: cpu.prompt_count == 1)
    shell(cpu,'ls docs','welcome.txt')
    shell(cpu,'tree','  docs/\n    welcome.txt\n    notes.txt\n')
    shell(cpu,'cat docs/welcome.txt','Welcome to Aurora-32!')
    shell(cpu,'run docs/welcome.txt','Not an executable.')
    shell(cpu,'mkdir work','Done.')
    shell(cpu,'mkdir work/nested','Done.')
    shell(cpu,'touch work/nested/note.txt','Done.')
    shell(cpu,'rm work','File operation failed: -5')
    shell(cpu,'rename work/nested/note.txt edited.txt','Done.')
    shell(cpu,'touch work/nested/edited.txt','File operation failed: -10')
    shell(cpu,'rm /','File operation failed: -9')
    shell(cpu,'mount','Storage mounted.')
    cpu.run_program('editor','editor > ')
    editor(cpu,'ls work/nested','edited.txt')
    editor(cpu,'open work/nested/edited.txt')
    editor(cpu,'append first')
    editor(cpu,'append second')
    editor(cpu,'set 1 updated')
    editor(cpu,'delete 2')
    editor(cpu,'show','updated\n')
    editor(cpu,'q','Unsaved edits:')
    editor(cpu,'save','Saved.')
    n=cpu.prompt_count; cpu.pending.extend('q\r'); cpu.until(lambda:cpu.prompt_count>n)
    shell(cpu,'cat work/nested/edited.txt','updated\n')
    shell(cpu,'verify work/nested/edited.txt','File verified.')
    shell(cpu,'mount','Storage mounted.')
    shell(cpu,'cat work/nested/edited.txt','updated\n')
    cpu.run_program('editor','editor > ')
    editor(cpu,'open work/nested/edited.txt','updated\n')
    editor(cpu,'clear')
    editor(cpu,'save','Saved.')
    n=cpu.prompt_count; cpu.pending.extend('q\r'); cpu.until(lambda:cpu.prompt_count>n)
    shell(cpu,'rm work/nested/edited.txt','Done.')
    shell(cpu,'rm work/nested','Done.')
    shell(cpu,'rm work','Done.')
    shell(cpu,'mount','Storage mounted.')
    print('PASS nested directories, management, editor, save/reopen/remount, empty text', flush=True)
    # A reset re-reads modified EEPROM rather than depending on kernel cache.
    again=StorageCPU(cpu.image.copy()); again.until(lambda:again.prompt_count==1)
    shell(again,'ls work','Directory not found.')
    words=image_words(); words[19]^=1  # wrong checksum, executable bytes unchanged
    quick=StorageCPU(words); quick.until(lambda:quick.prompt_count==1)
    shell(quick,'run calculator','Load failed:')
    assert quick.loaded_writes>0 and quick.user_steps==0  # checksum now completes during Copy
    start=len(quick.output); quick.pending.extend('quickrun calculator\r')
    quick.until(lambda:'>> ' in quick.output[start:],limit=8000000)
    report=quick.output[start:]
    assert 'Copy   [' in report and 'Check  [' not in report and 'Verify [' not in report
    quick.command('2+3','Result: 5'); quick.quit_key()
    print('PASS quickrun skips checksum comparison and executes; normal run rejects',flush=True)
    words=image_words(); words[words[17]+16]=0x01000000
    bad=StorageCPU(words); bad.until(lambda:bad.prompt_count==1)
    shell(bad,'quickrun calculator','Load failed:')
    assert bad.loaded_writes==0
    ro=StorageCPU(); ro.storage_writable=False; ro.until(lambda:ro.prompt_count==1)
    shell(ro,'touch fail.txt','File operation failed: -7')
    print('PASS quickrun still rejects kernel targets; readonly device fails safely',flush=True)


if __name__=='__main__': main()
