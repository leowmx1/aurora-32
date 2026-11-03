"""Check ls/programs column alignment using the generated kernel, not host formatting."""
from run_storage import StorageCPU, image_words


def check_listing(image):
    expected = []
    for i in range(image[4]):
        at = image[5] + i * image[6]
        if not image[at] or (image[1] == 2 and image[at + 36]): continue
        name = ''.join(chr(c) for c in image[at + 4:at + 36] if c)
        expected.append((name, image[at + 2], {1:'EXE',2:'TXT',3:'DIR'}[image[at]]))
    width = max([4] + [len(name) for name, _, _ in expected])
    table = '\n'.join([f"{'NAME':<{width}}  TYPE  SIZE (words)"] +
                      [f'{name:<{width}}  {kind:<4}  {size:>12}' for name, size, kind in expected]) + '\n'
    cpu = StorageCPU(image)
    cpu.until(lambda: cpu.prompt_count == 1)
    for command in ('ls', 'programs'):
        start = len(cpu.output)
        cpu.command_done(command)
        actual = cpu.output[start:]
        assert table in actual, repr(actual)
        assert cpu.loaded_writes == 0
    print(table, end='')
    print('PASS ls/programs: aligned names, right-aligned sizes, no UROM writes', flush=True)


if __name__ == '__main__':
    image = image_words()
    check_listing(image)
    # Longest legal filename must not collide with the size column.
    image[20:52] = [ord('z')] * 31 + [0]
    check_listing(image)
    # Empty mounted directories still produce a properly formatted heading.
    image[4] = 0
    check_listing(image)
