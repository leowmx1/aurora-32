"""Readable 5x7 test font; pack entire text strips into RAM offline.
Run --header to print the C header; no rasterization is done by the CPU.
"""
FONT_HEX = {
 ' ': '00000000000000', '!':'04040404040004', '.':'00000000000004',
 ',':'00000000000408', '-':'0000001F000000', '=':'00001F001F0000',
 '>':'10080402040810', ':':'00040000040000',
 '0':'0E11131519110E', '1':'040C040404040E', '2':'0E11010204081F',
 '3':'1E01010E01011E', '4':'02060A121F0202', '5':'1F10101E01011E',
 '6':'0E10101E11110E', '7':'1F010204080808', '8':'0E11110E11110E',
 '9':'0E11110F01010E',
 'A':'0E11111F111111', 'B':'1E11111E11111E', 'C':'0F10101010100F',
 'D':'1E11111111111E', 'E':'1F10101E10101F', 'F':'1F10101E101010',
 'G':'0F10101711110F', 'H':'1111111F111111', 'I':'0E04040404040E',
 'J':'0702020212120C', 'K':'11121418141211', 'L':'1010101010101F',
 'M':'111B1515111111', 'N':'11191513111111', 'O':'0E11111111110E',
 'P':'1E11111E101010', 'Q':'0E11111115120D', 'R':'1E11111E141211',
 'S':'0F10100E01011E', 'T':'1F040404040404', 'U':'1111111111110E',
 'V':'11111111110A04', 'W':'1111111515150A', 'X':'11110A040A1111',
 'Y':'11110A04040404', 'Z':'1F01020408101F',
 'a':'00000E010F110F', 'b':'10101E1111111E', 'c':'00000E1110110E',
 'd':'01010F1111110F', 'e':'00000E111F100E', 'f':'0608081C080808',
 'g':'00000F110F010E', 'h':'10101E11111111', 'i':'04000C0404040E',
 'j':'0200060202120C', 'k':'10101214181412', 'l':'0C04040404040E',
 'm':'00001A15151515', 'n':'00001E11111111', 'o':'00000E1111110E',
 'p':'00001E111E1010', 'q':'00000F110F0101', 'r':'00001619101010',
 's':'00000F100E011E', 't':'08081C08080906', 'u':'0000111111130D',
 'v':'00001111110A04', 'w':'0000111115150A', 'x':'0000110A040A11',
 'y':'000011110F010E', 'z':'00001F0204081F',
}
FONT={c:[int(s[i:i+2],16) for i in range(0,14,2)] for c,s in FONT_HEX.items()}
# x,y,foreground,background,mode,scale,text. Text cells are 6x8.
TEXT_CASES=[
 (12,10,1,0,1,1,'480x240 GRAPHICS TEST - 6x8 PIXEL CELLS'),
 (12,24,1,0,1,1,'ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789'),
 (12,36,1,0,1,1,'abcdefghijklmnopqrstuvwxyz 0123456789'),
 (12,48,1,0,1,1,'Hello, Aurora-32! Small text, no scaling.'),
 (288,26,0,1,1,1,'White text on black'),
 (288,42,0,1,1,1,'X = 288 > 255'),
 (280,88,1,5,1,4,'ABC'),
 (280,128,1,0,1,1,'Large letters: 4x scale'),
 (20,167,1,0,2,1,'RED'), (116,167,1,0,2,1,'GREEN'),
 (212,167,0,0,2,1,'BLUE'), (308,167,1,0,2,1,'YELLOW'),
 (288,206,1,0,1,1,'Right edge test'),
 (0,228,1,0,1,1,'1234567890'*8),
]
FRAME_JOBS=[(0,0,480,1,1,0,0),(0,239,480,1,1,0,0),
            (0,0,1,240,1,0,0),(479,0,1,240,1,0,0),(479,239,1,1,2,0,0)]

def pixels(text,scale):
    w=len(text)*6*scale; h=8*scale
    out=[]
    for y in range(h):
        for x in range(w):
            cell=x//(6*scale); col=(x//scale)%6; row=y//scale
            out.append(int(col<5 and row<7 and (FONT[text[cell]][row]>>(4-col))&1))
    return w,h,out

def text_jobs():
    return [(x,y,len(t)*6*s,8*s,fg,bg,m) for x,y,fg,bg,m,s,t in TEXT_CASES]

def header():
    data=[]; offsets=[]
    for x,y,fg,bg,m,s,t in TEXT_CASES:
        w,h,bits=pixels(t,s); offsets.append(len(data))
        for i in range(0,len(bits),32):
            data.append(sum((bits[j] if j<len(bits) else 1)<<(j-i) for j in range(i,i+32)))
    lines=['/* Generated from text_patterns.py --header; bit0 first, no row padding. */',
           '#define EXTRA_JOB_COUNT '+str(len(TEXT_CASES)+len(FRAME_JOBS))+'u',
           'static const aurora_u32 text_offsets[] = {'+','.join(map(str,offsets))+'};',
           'static const aurora_u32 text_data[] = {']
    for i in range(0,len(data),6): lines.append('  '+','.join(f'0x{n:08x}u' for n in data[i:i+6])+',')
    lines+=['};','static const aurora_u32 extra_jobs[EXTRA_JOB_COUNT][8] = {']
    for i,p in enumerate(text_jobs()): lines.append('  {'+','.join(map(str,p+(i+4,)))+'},')
    for p in FRAME_JOBS: lines.append('  {'+','.join(map(str,p+(0,)))+'},')
    return '\n'.join(lines+['};',''])

if __name__=='__main__':
    import sys
    if '--header' in sys.argv: print(header(),end='')
