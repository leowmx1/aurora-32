"""Actual kernel BIN: newest-first ten-entry history on both consoles."""
import re
from run_storage import StorageCPU

REMOTE = 'aurora:remote:/ > '
c = StorageCPU()
def output(port):
    return (''.join(map(chr, c.network_out)).replace('\r', '') if port else c.output)

c.until(lambda: c.prompt_count == 1 and output(1).endswith(REMOTE))

def command(port, text):
    start = len(output(port))
    if port:
        count = output(1).count(REMOTE)
        c.network_in.extend(map(ord, text + '\r\n'))
        c.until(lambda: output(1).count(REMOTE) > count, limit=4000000)
    else:
        c.command_done(text)
    return output(port)[start:]

def history(port, expected):
    text = command(port, 'history')
    rows = re.findall(r'^(\d+)  (.+)$', text, re.M)
    assert rows == [(str(i + 1), item) for i, item in enumerate(expected)], text

for port in (0, 1):
    history(port, [])
    for bad in ('!!', '!1', '!10'):
        assert 'No such history item.' in command(port, bad)
    prefix = 'remote' if port else 'local'
    expected = []
    for i in range(23):
        text = f'echo {prefix}-{i}'
        command(port, text)
        expected = [text] + expected[:9]
        if i in (0, 8, 9, 10, 22):
            history(port, expected)
    for bad in ('!0', '!11', '!01', '!100', '!1x', '!'):
        assert 'No such history item.' in command(port, bad)
    history(port, expected)
    for recall, index in (('!10', 9), ('!1', 0), ('!!', 0), ('!9', 8)):
        text = expected[index]
        result = command(port, recall)
        assert '\n' + text + '\n' + text[5:] + '\n' in result, result
        expected = [text] + expected[:9]
        history(port, expected)
    if not port:
        local_history = expected[:]
    else:
        history(0, local_history)
    print(f'PASS history port={port}: newest=1, limit=10, ring wrap, !10/!1/!!, invalid recalls, isolation', flush=True)
