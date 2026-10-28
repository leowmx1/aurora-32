"""Independent original 3x5 rows, not the program's packed 6x10 masks."""
CHARS = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'
ROWS = [
    7,5,5,5,7, 2,6,2,2,7, 7,1,7,4,7, 7,1,7,1,7, 5,5,7,1,1,
    7,4,7,1,7, 7,4,7,5,7, 7,1,1,1,1, 7,5,7,5,7, 7,5,7,1,7,
    2,5,7,5,5, 6,5,6,5,6, 7,4,4,4,7, 6,5,5,5,6, 7,4,6,4,7,
    7,4,6,4,4, 7,4,5,5,7, 5,5,7,5,5, 7,2,2,2,7, 1,1,1,5,7,
    5,5,6,5,5, 4,4,4,4,7, 5,7,7,5,5, 5,7,7,7,5, 7,5,5,5,7,
    7,5,7,4,4, 7,5,5,7,1, 6,5,6,5,5, 7,4,7,1,7, 7,2,2,2,2,
    5,5,5,5,7, 5,5,5,5,2, 5,5,7,7,5, 5,5,2,5,5, 5,5,2,2,2,
    7,1,2,4,7,
]


def glyph_pixels(screen, x, y, character, foreground, background):
    index = CHARS.index(character)
    for dy in range(10):
        for dx in range(6):
            bit = ROWS[index*5+dy//2] & (4 >> (dx//2))
            assert screen[(y+dy)*160+x+dx] == (foreground if bit else background), (character,x,y,dx,dy)


def font_commands(cpu, background, colors):
    allowed = set()
    for i in range(len(CHARS)):
        bits = sum(1 << (dy*6+dx) for dy in range(10) for dx in range(6)
                   if ROWS[i*5+dy//2] & (4 >> (dx//2)))
        allowed.add(bits)
    count = 0
    for x,y,w,h,c,bg,low,high,mode in cpu.operations:
        if mode == 0: continue
        assert mode == 1 and (w,h) == (6,10), 'one opaque bitmap per glyph'
        assert bg == background and c in colors
        assert low | (high << 32) in allowed, 'packed glyph differs from original font'
        count += 1
    return count
