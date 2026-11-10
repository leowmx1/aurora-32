"""Offline font reference generator (Pillow + locally installed regular font).

Prints JSON only. No TTF or rasterizer is included in the target firmware;
the C tables contain ready-to-upload 1bpp glyphs. No runtime scaling.
"""
import argparse, json
from PIL import Image, ImageDraw, ImageFont
p=argparse.ArgumentParser()
p.add_argument('--font',default='C:/Windows/Fonts/consola.ttf')
args=p.parse_args()
result={}
for scale,size in ((2,18),(3,26),(4,36)):
    w,h=6*scale,8*scale
    font=ImageFont.truetype(args.font,size)
    top,bottom=font.getbbox('H')[1::2]
    result[str(scale)]={}
    for c in '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-':
        image=Image.new('L',(w,h),0)
        draw=ImageDraw.Draw(image)
        draw.text(((w-font.getlength(c))//2,(h-(bottom-top))//2-top),c,font=font,fill=255)
        result[str(scale)][c]=[sum(1<<x for x in range(w) if image.getpixel((x,y))>=112) for y in range(h)]
print(json.dumps(result,separators=(',',':')))
