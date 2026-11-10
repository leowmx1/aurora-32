"""Two physical terminals, real kernel/user BINs; no Digital/socket simulation."""
from run_storage import StorageCPU

PROMPT='aurora:remote:/ > '
def net(cpu): return ''.join(map(chr,cpu.network_out)).replace('\r','')
def remote(cpu, command, expected=None):
    start=len(net(cpu)); n=net(cpu).count(PROMPT)
    cpu.network_in.extend(map(ord,command+'\r\n'))
    cpu.until(lambda: net(cpu).count(PROMPT)>n,limit=12000000)
    if expected: assert expected in net(cpu)[start:],net(cpu)[start:]
def boot():
    c=StorageCPU(); c.until(lambda:c.prompt_count==1 and net(c).endswith(PROMPT))
    assert 'BIOS startup:' in c.output and '3 seconds' not in c.output
    return c

c=boot()
remote(c,'echo remote-only','remote-only')
assert 'remote-only' not in c.output
c.command_done('echo local-only'); assert 'local-only' not in net(c)
remote(c,'history','echo remote-only')
assert 'local-only' not in net(c)
remote(c,'netstat','RX: ')
assert 'Last RX: never' not in net(c) and c.network_reads>0
c.run_program('calculator','>> ')
remote(c,'ps','local    FG')
before=c.loaded_writes
start=len(net(c)); c.network_in.extend(map(ord,'quickrun calculator\r\n'))
c.until(lambda:net(c)[start:].endswith('>> '),limit=8000000)
assert c.loaded_writes>before and c.dma_jobs[-1][1]==0x400000
start=len(net(c)); c.network_in.extend(map(ord,'7*9\r\n'))
c.until(lambda:'Result: 63\n' in net(c)[start:],limit=8000000)
c.network_in.append(ord('Q')); c.until(lambda:net(c).endswith(PROMPT),limit=8000000)
c.command('8*3+2','Result: 26')
remote(c,'mkdir remote-test','Done.')
remote(c,'touch remote-test/note.txt','Done.')
remote(c,'rm remote-test/note.txt','Done.')
remote(c,'rm remote-test','Done.')
remote(c,'kill 1','User process terminated.')
c.until(lambda:c.output.endswith('aurora:/ > '))
assert 'exited with code 137.' in c.output
remote(c,'kill','No user program running.')
print('PASS local + remote users concurrently, independent globals/stacks/callbacks, metadata commands, kill',flush=True)

start=len(net(c)); c.network_in.extend(map(ord,'quickrun calculator\r\n'))
c.until(lambda:net(c)[start:].endswith('>> '),limit=8000000)
c.until(lambda:not c.kernel_mode and c.ie)
# No local characters may enter the remote user's keyboard callback.
c.command_done('echo local-free')
assert 'local-free' not in net(c)
c.command_done('ps'); assert 'remote   FG' in c.output
start=len(net(c)); c.network_in.extend(map(ord,'-8*(2+3)\r\n'))
c.until(lambda:'Result: -40\n' in net(c)[start:],limit=8000000)
assert 'Result: -40' not in c.output
c.until(lambda:net(c).endswith('>> ') and not c.callback_inflight)
c.command_done('kill user')
c.until(lambda:net(c).endswith(PROMPT))
assert 'exited with code 137.' in net(c)
remote(c,'history','quickrun calculator')
print('PASS remote user + independent local shell, CRLF handoff, isolated I/O, local kill',flush=True)

# A busy/paused GPU and MIDI must be cleaned on kill as on Exit.
c.run_program('snake','Q quit')
c.until(lambda:not c.kernel_mode and c.ie)
remote(c,'kill','User process terminated.')
c.until(lambda:c.output.endswith('aurora:/ > '))
remote(c,'ps','EMPTY')
print('PASS kill game, cleanup, return to owner console; callbacks/ticks preserved',flush=True)

# A referenced ID cannot be deleted/reused while the editor could still Save it.
c.run_program('editor','editor > ')
start=len(c.output); c.pending.extend('open docs/notes.txt\r')
c.until(lambda:c.output[start:].endswith('editor > '),limit=4000000)
remote(c,'rm docs/notes.txt','File operation failed: -5')
remote(c,'kill','User process terminated.')
c.until(lambda:c.output.endswith('aurora:/ > '))
remote(c,'rm docs/notes.txt','Done.')
print('PASS live file IDs pinned until exit/kill; concurrent deletion cannot recycle editor target',flush=True)
