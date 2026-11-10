#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
static aurora_word_t pixels[35];
static aurora_graphics_image image;
static void check(int ok,int code) {
  if(!ok) { system.io.PutString("IMAGE FAIL "); system.io.PutInt(code); system.Exit(code); }
}
static void idle(void) {
  aurora_word_t start=system.TimeMs();
  while(system.graphics.Busy()) {
    check(system.TimeMs()-start<10000u,90); system.Yield();
  }
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  image.x=300; image.y=180; image.width=65; image.height=17;
  image.color=2; image.background=4; image.data=pixels; image.words=35;
  for(aurora_word_t i=0;i<35;++i) pixels[i]=0x80000001u+i;
  check(aurora_syscall(38,0,1,0)==AURORA_EFAULT,1);
  check(aurora_syscall(38,0x82000000u,1,0)==AURORA_EFAULT,2);
  check(aurora_syscall(38,0x813ffff9u,1,0)==AURORA_EFAULT,3);
  check(aurora_syscall(38,(aurora_word_t)&image,0,0)==AURORA_EINVAL,4);
  check(aurora_syscall(38,(aurora_word_t)&image,1,1)==AURORA_EINVAL,5);
  image.words=34; check(system.graphics.Image(&image)==AURORA_EINVAL,6);
  image.words=36; check(system.graphics.Image(&image)==AURORA_EINVAL,7);
  image.words=35; image.data=(const aurora_word_t*)0x813fffdeu;
  check(system.graphics.Image(&image)==AURORA_EFAULT,8);
  image.data=(const aurora_word_t*)0x82000000u;
  check(system.graphics.Image(&image)==AURORA_EFAULT,9);
  image.data=pixels; image.x=416;
  check(system.graphics.Image(&image)==AURORA_EINVAL,10);
  image.x=300; image.y=224;
  check(system.graphics.Image(&image)==AURORA_EINVAL,11);
  image.y=180;
  check(system.graphics.FillRect(0,0,480,240,3)==0,12); idle();
  check(system.graphics.Image(&image)==0,13);
  /* Both packet and source are reusable after submit, not retained by kernel. */
  image.color=1; pixels[0]=0; idle();
  image.x=400; image.y=200; image.width=33; image.height=1; image.words=2;
  image.color=2; pixels[0]=0x80000001u; pixels[1]=1u;
  check(system.graphics.ImageTransparent(&image)==0,14); idle();
  image.x=479; image.y=239; image.width=1; image.words=1;
  check(system.graphics.Image(&image)==0,15); idle();
  system.io.PutString("IMAGE CONTRACT PASS\n"); system.Exit(0);
  for(;;) system.Yield();
}
