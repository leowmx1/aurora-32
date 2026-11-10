"""Offline reference only: print header or pixel rows; never scale on target."""
import argparse,json
from PIL import Image,ImageDraw,ImageFont
p=argparse.ArgumentParser()
p.add_argument('--header',action='store_true')
p.add_argument('--font',default='C:/Windows/Fonts/consola.ttf')
a=p.parse_args()
font=ImageFont.truetype(a.font,72)
image=Image.new('L',(48,72))
left,top,right,bottom=font.getbbox('A')
ImageDraw.Draw(image).text(((48-font.getlength('A'))//2,(72-bottom+top)//2-top),'A',font=font,fill=255)
rows=[''.join('#' if image.getpixel((x,y))>=112 else '.' for x in range(48)) for y in range(72)]
if a.header:
    bits=sum(1<<(y*48+x) for y,row in enumerate(rows) for x,c in enumerate(row) if c=='#')
    words=[(bits>>(32*i))&0xffffffff for i in range(108)]
    print('/* 48x72 A, 108 words; bit0 top-left, tightly packed rows. */')
    print('static const aurora_u32 letter_a[108] = {')
    for i in range(0,108,6): print('  '+','.join('0x%08xu'%w for w in words[i:i+6])+',')
    print('};')
else: print(json.dumps(rows))
