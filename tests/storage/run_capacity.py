"""Check installed storage capacity reporting using the generated kernel."""
from run_storage import StorageCPU, image_words


for image in (image_words(), []):
    cpu = StorageCPU(image)
    cpu.until(lambda: cpu.prompt_count == 1)
    start = len(cpu.output)
    cpu.command_done('df')
    output = cpu.output[start:]
    assert 'Device words:  16777216 (64 MiB)' in output, output
    if image:
        assert f'Image words:   {image[3]}\n' in output, output
        assert f'Outside image: {16777216 - image[3]}\n' in output, output
    else:
        assert 'No valid image mounted; usage unknown.' in output, output
        assert 'Outside image:' not in output, output
    assert cpu.loaded_writes == 0
    print(output)
print('PASS storage capacity: mounted and invalid image')
