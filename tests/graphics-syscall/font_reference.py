"""Offline regular-font reference and centered 12/5-scale layout sampling."""
import json, pathlib
PIXELS=json.loads(pathlib.Path(__file__).with_name('font_pixels.json').read_text())
FONT = {"0":[14,17,19,21,25,17,14],"1":[4,12,4,4,4,4,14],"2":[14,17,1,2,4,8,31],"3":[30,1,1,14,1,1,30],"4":[2,6,10,18,31,2,2],"5":[31,16,16,30,1,1,30],"6":[14,16,16,30,17,17,14],"7":[31,1,2,4,8,8,8],"8":[14,17,17,14,17,17,14],"9":[14,17,17,15,1,1,14],"A":[14,17,17,31,17,17,17],"B":[30,17,17,30,17,17,30],"C":[15,16,16,16,16,16,15],"D":[30,17,17,17,17,17,30],"E":[31,16,16,30,16,16,31],"F":[31,16,16,30,16,16,16],"G":[15,16,16,23,17,17,15],"H":[17,17,17,31,17,17,17],"I":[14,4,4,4,4,4,14],"J":[7,2,2,2,18,18,12],"K":[17,18,20,24,20,18,17],"L":[16,16,16,16,16,16,31],"M":[17,27,21,21,17,17,17],"N":[17,25,21,19,17,17,17],"O":[14,17,17,17,17,17,14],"P":[30,17,17,30,16,16,16],"Q":[14,17,17,17,21,18,13],"R":[30,17,17,30,20,18,17],"S":[15,16,16,14,1,1,30],"T":[31,4,4,4,4,4,4],"U":[17,17,17,17,17,17,14],"V":[17,17,17,17,17,10,4],"W":[17,17,17,21,21,21,10],"X":[17,17,10,4,10,17,17],"Y":[17,17,10,4,4,4,4],"Z":[31,1,2,4,8,16,31]}
CHARS = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'
ROWS = [row for c in CHARS for row in FONT[c]]

def logical_pixels(screen):
    return [screen[(y*12//5)*480+48+x*12//5] for y in range(100) for x in range(160)]

def glyph_bits(character, scale=2):
    return sum(row<<(y*6*scale) for y,row in enumerate(PIXELS[str(scale)][character]))

def glyph_pixels(screen, x, y, character, foreground, background):
    x,y=49+x*12//5,2+y*12//5
    bits=glyph_bits(character)
    for dy in range(16):
        for dx in range(12):
            bit=bits & (1<<(dy*12+dx))
            assert screen[(y+dy)*480+x+dx]==(foreground if bit else background), (character,x,y,dx,dy)

def font_commands(cpu, background, colors):
    allowed={glyph_bits(c) for c in CHARS}
    count=0
    for op,bits in zip(cpu.operations,cpu.image_bits):
        x,y,w,h,c,bg,low,high,mode=op
        if mode==0: continue
        assert mode==1 and (w,h)==(12,16), 'one readable glyph per bitmap'
        assert bg==background and c in colors
        assert bits in allowed, 'glyph differs from independent 5x7 font'
        count+=1
    return count
