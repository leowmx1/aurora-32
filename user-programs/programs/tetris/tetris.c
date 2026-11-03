#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
/* 10 x 20 cells, 4 pixels per cell; one-pixel gutters within each cell. */
#define W 10
#define H 20
#define BG 0x90c9u
#define WELL 0x8864u
#define INK 0xf798u
static volatile int board[200] __attribute__((section(".noinit")));
static int painted[200] __attribute__((section(".noinit")));
static int canvas[200] __attribute__((section(".noinit")));
static volatile int kind, rotation, px, py, next_kind;
static volatile int started, paused, over, quit, changed;
static volatile unsigned int score, lines, last_fall, random_state;
static int bag[7], bag_used;
static int shown_score, shown_lines, shown_pause, shown_next;
static volatile unsigned int random_divisor=65536u;
/* I O T S Z J L, cells in a 4-wide coordinate grid. */
static const int shapes[7][4]={{4,5,6,7},{1,2,5,6},{1,4,5,6},
  {1,2,4,5},{0,1,5,6},{0,4,5,6},{2,4,5,6}};
static const unsigned int colors[8]={WELL,0x9f39u,0xff09u,0xd9b8u,
  0xb32du,0xf16du,0xa63bu,0xf648u};
/* Digits then T E R I S L P N; 6x10 bitmaps, bit 0 top left. */
static const unsigned int font[18][2]={
 {0xf3cf3fff,0x0fffcf3c},{0x0c3cf30c,0x0fff30c3},
 {0xffc30fff,0x0fff0c3f},{0xffc30fff,0x0fffc30f},
 {0xffcf3cf3,0x0c30c30f},{0xff0c3fff,0x0fffc30f},
 {0xff0c3fff,0x0fffcf3f},{0x30c30fff,0x0c30c30c},
 {0xffcf3fff,0x0fffcf3f},{0xffcf3fff,0x0fffc30f},
 {0x0c30cfff,0x030c30c3},{0xcf0c3fff,0x0fff0c33},
 {0xcfcf33cf,0x0cf3cf33},{0x0c30cfff,0x0fff30c3},
 {0xff0c3fff,0x0fffc30f},{0xc30c30c3,0x0fff0c30},
 {0xffcf3fff,0x00c30c3f},{0xfffffcf3,0x0cf3ffff}};
static aurora_graphics_bitmap glyph_packet;

static void fail(const char *message) { system.io.PutString(message); system.Exit(1); }
static unsigned int random_below(unsigned int n) {
  random_state=random_state*1664525u+1013904223u;
  return (random_state/random_divisor)%n;
}
static int take_piece(void) {
  if(bag_used==7) {
    for(int i=0;i<7;++i) bag[i]=i;
    for(int i=6;i>0;--i) {
      int j=(int)random_below((unsigned int)i+1u), t=bag[i];
      bag[i]=bag[j]; bag[j]=t;
    }
    bag_used=0;
  }
  return bag[bag_used++];
}
static int cell(int piece,int turn,int i) {
  int p=shapes[piece][i], x=p%4, y=p/4;
  if(piece==1) return p;
  int edge=piece==0?3:2;
  for(int r=0;r<turn;++r) { int old=x; x=edge-y; y=old; }
  return y*4+x;
}
static int fits(int x,int y,int turn) {
  for(int i=0;i<4;++i) {
    int p=cell(kind,turn,i), xx=x+p%4, yy=y+p/4;
    if(xx<0 || xx>=W || yy<0 || yy>=H) return 0;
    if(board[yy*W+xx]) return 0;
  }
  return 1;
}
static unsigned int interval(void) {
  unsigned int level=lines/10u;
  return level>=8u?100u:500u-level*50u;
}
static void spawn(void) {
  kind=next_kind; next_kind=take_piece(); rotation=0; px=3; py=0;
  if(!fits(px,py,rotation)) over=1;
  last_fall=system.TimeMs(); changed=1;
}
static int clear_lines(void) {
  int write=H-1, count=0;
  for(int read=H-1;read>=0;--read) {
    int full=1;
    for(int x=0;x<W;++x) if(!board[read*W+x]) full=0;
    if(full) { ++count; continue; }
    if(write!=read) for(int x=0;x<W;++x) board[write*W+x]=board[read*W+x];
    --write;
  }
  while(write>=0) { for(int x=0;x<W;++x) board[write*W+x]=0; --write; }
  return count;
}
static void lock_piece(void) {
  for(int i=0;i<4;++i) {
    int p=cell(kind,rotation,i);
    board[(py+p/4)*W+px+p%4]=kind+1;
  }
  int n=clear_lines();
  unsigned int points=n==4?800u:n==3?500u:n==2?300u:n==1?100u:0u;
  score+=points*(lines/10u+1u); lines+=(unsigned int)n;
  spawn();
}
static void down(void) {
  if(fits(px,py+1,rotation)) { ++py; changed=1; }
  else lock_piece();
}
static void rotate_piece(void) {
  int r=(rotation+1)%4;
  /* Small bounded horizontal wall kicks, not full SRS. */
  for(int i=0;i<5;++i) {
    int dx=i==0?0:i==1?-1:i==2?1:i==3?-2:2;
    if(fits(px+dx,py,r)) { px+=dx; rotation=r; changed=1; return; }
  }
}
static void key(aurora_word_t event) {
  int c=(int)aurora_event_character(event);
  if(c>='a' && c<='z') c-='a'-'A';
  if(c=='Q') { quit=1; return; }
  if(over) return;
  if(!started) {
    if(c==' ') { started=1; last_fall=system.TimeMs(); changed=1; }
    return;
  }
  if(c=='P') { paused=!paused; last_fall=system.TimeMs(); changed=1; return; }
  if(paused) return;
  if(c=='A' && fits(px-1,py,rotation)) { --px; changed=1; }
  if(c=='D' && fits(px+1,py,rotation)) { ++px; changed=1; }
  if(c==' ') rotate_piece();
  if(c=='S') { if(fits(px,py+1,rotation)) ++score; down(); last_fall=system.TimeMs(); }
  if(c=='W') {
    while(fits(px,py+1,rotation)) { ++py; score+=2u; }
    lock_piece();
  }
}
static void initialize(void) {
  for(int i=0;i<200;++i) { board[i]=0; painted[i]=0; canvas[i]=0; }
  score=lines=0; started=paused=over=quit=0; changed=1;
  random_state=system.TimeMs()+0x6d2b79f5u; bag_used=7;
  shown_score=shown_lines=shown_pause=shown_next=-1;
  next_kind=take_piece(); spawn();
}
static void wait_graphics(void) {
  unsigned int start=system.TimeMs();
  for(;;) {
    int busy=system.graphics.Busy();
    if(busy<0) fail("Graphics status failed.\n");
    if(!busy) return;
    if(system.TimeMs()-start>=10000u) fail("Graphics timeout.\n");
    system.Yield();
  }
}
/* Packed positions/sizes keep helper calls inside the register ABI. */
static void fill(unsigned int pos,unsigned int size,unsigned int color) {
  wait_graphics();
  if(system.graphics.FillRect(pos%256u,pos/256u,size%256u,size/256u,color)!=AURORA_OK)
    fail("Graphics fill failed.\n");
}
__attribute__((noinline,optnone)) static void glyph(int x,int y,int index) {
  wait_graphics();
  glyph_packet.x=x; glyph_packet.y=y;
  glyph_packet.width=6; glyph_packet.height=10;
  glyph_packet.color=INK; glyph_packet.background=BG;
  glyph_packet.data_low=font[index][0]; glyph_packet.data_high=font[index][1];
  if(system.graphics.Bitmap(&glyph_packet)!=AURORA_OK) fail("Graphics bitmap failed.\n");
}
static void number(int y,unsigned int n,int old) {
  unsigned int d=100000u; n%=1000000u;
  for(int i=0;i<6;++i) {
    if(old<0 || n/d%10u!=(unsigned int)old/d%10u) glyph(3+i*7,y,(int)(n/d%10u));
    d/=10u;
  }
}
static void screen_begin(void) {
  fill(0u,160u+100u*256u,BG);
  fill(57u+7u*256u,42u+82u*256u,0xb9d1u);
  fill(58u+8u*256u,40u+80u*256u,WELL);
  glyph(3,3,10); glyph(10,3,11); glyph(17,3,10);
  glyph(24,3,12); glyph(31,3,13); glyph(38,3,14);
  glyph(3,23,14); glyph(3,54,15); glyph(112,8,17); glyph(112,55,16);
}
static void draw(void) {
  for(int i=0;i<200;++i) canvas[i]=board[i];
  if(!over) for(int i=0;i<4;++i) {
    int p=cell(kind,rotation,i); canvas[(py+p/4)*W+px+p%4]=kind+1;
  }
  for(int i=0;i<200;++i) if(canvas[i]!=painted[i]) {
    fill((unsigned int)(58+(i%W)*4)+(unsigned int)(8+(i/W)*4)*256u,
         3u+3u*256u,colors[canvas[i]]);
    painted[i]=canvas[i];
  }
  number(36,score,shown_score); shown_score=(int)(score%1000000u);
  number(67,lines,shown_lines); shown_lines=(int)(lines%1000000u);
  int state=paused || !started;
  if(state!=shown_pause) { glyph(122,55,state); shown_pause=state; }
  if(next_kind!=shown_next) {
    fill(112u+25u*256u,16u+16u*256u,BG);
    for(int i=0;i<4;++i) {
      int p=cell(next_kind,0,i);
      fill((unsigned int)(112+p%4*4)+(unsigned int)(25+p/4*4)*256u,
           3u+3u*256u,colors[next_kind+1]);
    }
    shown_next=next_kind;
  }
  wait_graphics(); changed=0;
}
#ifndef TETRIS_SELFTEST
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  system.DeferEvents();
  initialize();
  if(system.io.Subscribe(key)!=AURORA_OK) fail("Keyboard subscription failed.\n");
  system.io.PutString("Screen Tetris - 10 x 20, 4-pixel cells\n"
    "SPACE start / rotate   A/D move\nW hard drop   S soft drop   P pause   Q quit\n"
    "S=score  L=lines  N=next  P=waiting/paused\n");
  screen_begin(); draw(); system.ResumeEvents();
  for(;;) {
    system.DeferEvents();
    if(system.EventDrops()) fail("Input queue overflow. Exiting safely.\n");
    if(quit || over) {
      if(changed && !quit) draw();
      system.io.PutString(over?"Game over. Score: ":"Tetris quit. Score: ");
      system.io.PutInt((int)score); system.io.PutString("  Lines: ");
      system.io.PutInt((int)lines); system.io.PutChar('\n'); system.Exit(0);
    }
    unsigned int now=system.TimeMs(), period=interval();
    if(started && !paused && now-last_fall>=period) { last_fall=now; down(); }
    if(changed) draw();
    /* Short bounded processing; no catch-up loop after a slow draw. Sleep is
     * interruptible by input, and callbacks cannot mutate the render snapshot. */
    int waiting=!started || paused;
    unsigned int elapsed=system.TimeMs()-last_fall;
    unsigned int delay=elapsed<period?period-elapsed:1u;
    system.ResumeEvents();
    if(waiting) system.WaitEvent(); else system.Sleep(delay);
  }
}
#endif
