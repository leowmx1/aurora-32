#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
/* Integer-pixel grid: 11x11 cells, 10x10 interiors, 1-pixel gutters.
 * The 110x220 well nearly fills the screen height without changing rules. */
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
/* Digits then T E R I S L P N; 12x16 HUD / 18x24 title cells, bit0 top-left. */
static const aurora_word_t font[18][6] = {
  {0xf0000000u,0xc30c1980u,0x3643c430u,0x0c30c33cu,0x00f01983u,0x00000000u}, /* 0 */
  {0x60000000u,0x006c0780u,0x06006006u,0x60060060u,0x03fc0600u,0x00000000u}, /* 1 */
  {0xf8000000u,0x01801880u,0x18018018u,0x300600c0u,0x03fc0180u,0x00000000u}, /* 2 */
  {0xf8000000u,0x01801800u,0x0f008018u,0x00300180u,0x00fc1803u,0x00000000u}, /* 3 */
  {0xc0000000u,0x01a01e01u,0x1981981bu,0xfe18418cu,0x01801807u,0x00000000u}, /* 4 */
  {0xf8000000u,0x80080081u,0x1800f800u,0x00300300u,0x007c1801u,0x00000000u}, /* 5 */
  {0xe0000000u,0xc0180301u,0x31c1fc00u,0x0c30c30cu,0x00f01983u,0x00000000u}, /* 6 */
  {0xfc000000u,0x01803003u,0x0c00c018u,0x30060060u,0x00180300u,0x00000000u}, /* 7 */
  {0xf0000000u,0xc30c1980u,0x0f019810u,0x0c30c198u,0x00f838c3u,0x00000000u}, /* 8 */
  {0xf0000000u,0xc10c1880u,0x38c30c30u,0x80100378u,0x00780c01u,0x00000000u}, /* 9 */
  {0xfc000000u,0x00600603u,0x06006006u,0x60060060u,0x00600600u,0x00000000u}, /* T */
  {0xf8000000u,0x80080081u,0x1f800800u,0x08008008u,0x01f80080u,0x00000000u}, /* E */
  {0xfc000000u,0xc10c18c0u,0x0fc18c10u,0x8c18c0ccu,0x030c30c1u,0x00000000u}, /* R */
  {0xfc000000u,0x00600601u,0x06006006u,0x60060060u,0x01fc0600u,0x00000000u}, /* I */
  {0xf0000000u,0xc00c1180u,0x0f003800u,0x00300180u,0x00f81843u,0x00000000u}, /* S */
  {0x18000000u,0x80180180u,0x01801801u,0x18018018u,0x03f80180u,0x00000000u}, /* L */
  {0xfc000000u,0xc30c18c0u,0x18c30c30u,0x0c00c0fcu,0x000c00c0u,0x00000000u}, /* P */
  {0x0c000000u,0xc31c31c3u,0x36c32c33u,0x8c34c34cu,0x030c38c3u,0x00000000u}, /* N */
};
static const aurora_word_t title_font[18][14] = {
  {0x00000000u,0xe0000000u,0x801fe003u,0x030700e3u,0x70381c0cu,0xe181f0e0u,0xf819e606u,0x8183e061u,0x1c0c0607u,0xe3c03070u,0xe001fe00u,0x00000003u,0x00000000u,0x00000000u}, /* 0 */
  {0x00000000u,0xc0000000u,0xc007c001u,0x0077001fu,0x070001c4u,0x70001c00u,0x0001c000u,0x001c0007u,0x01c00070u,0x1c000700u,0xfc07ff00u,0x0000001fu,0x00000000u,0x00000000u}, /* 1 */
  {0x00000000u,0xf0000000u,0xc01fe001u,0x03820071u,0x38000e00u,0x8000e000u,0x00070001u,0x001c000eu,0x00700038u,0x01c000e0u,0xfc07ff00u,0x0000001fu,0x00000000u,0x00000000u}, /* 2 */
  {0x00000000u,0xf0000000u,0x801ff001u,0x03800070u,0x18000e00u,0xfc007000u,0x0007f000u,0x00c0003cu,0x0c000700u,0xf0003000u,0xfc01ff00u,0x00000001u,0x00000000u,0x00000000u}, /* 3 */
  {0x00000000u,0x80000000u,0x001e0007u,0x01f8007cu,0x1cc00760u,0xc6007380u,0x38070c01u,0xc070601cu,0x3fff0fffu,0x70001c00u,0x0001c000u,0x00000007u,0x00000000u,0x00000000u}, /* 4 */
  {0x00000000u,0xfc000000u,0xc03ff00fu,0x00070001u,0x0070001cu,0xff0001c0u,0x0007fc00u,0x00c0003cu,0x0c000300u,0x70003800u,0xfc00ff00u,0x00000001u,0x00000000u,0x00000000u}, /* 5 */
  {0x00000000u,0x80000000u,0x003f800fu,0x000e0007u,0x0030001cu,0xfb0000c0u,0x700ffc01u,0x01c0c070u,0x180c0603u,0xe1807030u,0xe001fe00u,0x00000003u,0x00000000u,0x00000000u}, /* 6 */
  {0x00000000u,0xfe000000u,0x007ff81fu,0x030001c0u,0x18000e00u,0xc0007000u,0x00038000u,0x001c0006u,0x00e00030u,0x07000180u,0x38001c00u,0x00000000u,0x00000000u,0x00000000u}, /* 7 */
  {0x00000000u,0xf0000000u,0xc03fe003u,0x030300e1u,0x30700c0cu,0xfc007380u,0xe003f000u,0x00e1c03cu,0x1c0c0703u,0xe1c07030u,0xf003fe00u,0x00000003u,0x00000000u,0x00000000u}, /* 8 */
  {0x00000000u,0xf0000000u,0xc01fe003u,0x030300e1u,0x70380c0eu,0x0381c0e0u,0xf01e1c07u,0x00df007fu,0x0c000300u,0x78001800u,0xfc00ff00u,0x00000000u,0x00000000u,0x00000000u}, /* 9 */
  {0x00000000u,0xfe000000u,0x007ff81fu,0x0030000cu,0x030000c0u,0x30000c00u,0x0000c000u,0x000c0003u,0x00c00030u,0x0c000300u,0xc0003000u,0x00000000u,0x00000000u,0x00000000u}, /* T */
  {0x00000000u,0xfc000000u,0xc03ff00fu,0x00070001u,0x0070001cu,0xff0001c0u,0x700ffc03u,0x0001c000u,0x001c0007u,0x01c00070u,0xfc03ff00u,0x0000000fu,0x00000000u,0x00000000u}, /* E */
  {0x00000000u,0xfc000000u,0xc01ff003u,0x038300f0u,0x30300c0cu,0xc300e0c0u,0xf003fc01u,0x0038c007u,0x060c01c3u,0xc0c03830u,0x0c070300u,0x0000001cu,0x00000000u,0x00000000u}, /* R */
  {0x00000000u,0xfc000000u,0x003ff00fu,0x0030000cu,0x030000c0u,0x30000c00u,0x0000c000u,0x000c0003u,0x00c00030u,0x0c000300u,0xfc03ff00u,0x0000000fu,0x00000000u,0x00000000u}, /* I */
  {0x00000000u,0xe0000000u,0xc03fe007u,0x00030001u,0x0030000cu,0x3e0003c0u,0x0003f000u,0x00e0001eu,0x1c000700u,0xe0407000u,0xfc01ff80u,0x00000003u,0x00000000u,0x00000000u}, /* S */
  {0x00000000u,0x18000000u,0x80006000u,0x00060001u,0x00600018u,0x06000180u,0x60001800u,0x00018000u,0x00180006u,0x01800060u,0xf807fe00u,0x0000001fu,0x00000000u,0x00000000u}, /* L */
  {0x00000000u,0xfc000000u,0xc01ff003u,0x070300e0u,0x70301c0cu,0x0301c0c0u,0xf00e0c03u,0x001fc01fu,0x000c0003u,0x00c00030u,0x0c000300u,0x00000000u,0x00000000u,0x00000000u}, /* P */
  {0x00000000u,0x1e000000u,0xe0607818u,0x060f8183u,0x61b8186eu,0x338186e0u,0x3818ce06u,0x8198e066u,0x1b0e06e3u,0xe0e07c38u,0x0e078381u,0x0000001cu,0x00000000u,0x00000000u}, /* N */
};
static aurora_graphics_image glyph_packet;

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
  /* Keep packed arguments intact: avoid needless unpack/repack and the
   * current backend's half-word shift legalization on word-addressed RAM. */
  if(aurora_syscall(AURORA_SYSCALL_GRAPHICS_RECT16,pos,size,color)!=AURORA_OK)
    fail("Graphics fill failed.\n");
}
__attribute__((noinline,optnone)) static void glyph(int x,int y,int index) {
  wait_graphics();
  glyph_packet.x=49+x*12/5; glyph_packet.y=2+y*12/5;
  glyph_packet.width=12; glyph_packet.height=16;
  glyph_packet.color=INK; glyph_packet.background=BG;
  glyph_packet.data=font[index]; glyph_packet.words=6u;
  if(y==3) {
    glyph_packet.width=18; glyph_packet.height=24;
    glyph_packet.data=title_font[index]; glyph_packet.words=14u;
  }
  if(system.graphics.Image(&glyph_packet)!=AURORA_OK) fail("Graphics bitmap failed.\n");
}
static void number(int y,unsigned int n,int old) {
  unsigned int d=100000u; n%=1000000u;
  for(int i=0;i<6;++i) {
    if(old<0 || n/d%10u!=(unsigned int)old/d%10u) glyph(3+i*7,y,(int)(n/d%10u));
    d/=10u;
  }
}
static void screen_begin(void) {
  fill(0u,480u+240u*65536u,BG);
  fill(183u+8u*65536u,114u+224u*65536u,0xb9d1u);
  fill(185u+10u*65536u,110u+220u*65536u,WELL);
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
    fill((unsigned int)(185+(i%W)*11)+(unsigned int)(10+(i/W)*11)*65536u,
         10u+10u*65536u,colors[canvas[i]]);
    painted[i]=canvas[i];
  }
  number(36,score,shown_score); shown_score=(int)(score%1000000u);
  number(67,lines,shown_lines); shown_lines=(int)(lines%1000000u);
  int state=paused || !started;
  if(state!=shown_pause) { glyph(122,55,state); shown_pause=state; }
  if(next_kind!=shown_next) {
    fill(320u+60u*65536u,44u+44u*65536u,BG);
    for(int i=0;i<4;++i) {
      int p=cell(next_kind,0,i);
      fill((unsigned int)(320+p%4*11)+(unsigned int)(60+p/4*11)*65536u,
           10u+10u*65536u,colors[next_kind+1]);
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
  system.io.PutString("Screen Tetris - 10 x 20, 11-pixel cells\n"
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
