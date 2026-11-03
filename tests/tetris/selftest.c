#define TETRIS_SELFTEST 1
#include "../../user-programs/programs/tetris/tetris.c"
static volatile int seen[7];
static void check(int ok,int n) {
  if(!ok) { system.io.PutString("TETRIS FAIL "); system.io.PutInt(n); system.Exit(1); }
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer(); initialize();
  /* Four rotations preserve cell identity, four distinct cells per shape. */
  for(int k=0;k<7;++k) for(int r=0;r<4;++r) for(int i=0;i<4;++i) {
    int p=cell(k,r,i); check(p>=0 && p<16,1);
    for(int j=0;j<i;++j) check(p!=cell(k,r,j),2);
    check(cell(k,4,i)==cell(k,0,i),3);
  }
  for(int turn=0;turn<20;++turn) {
    bag_used=7; for(int i=0;i<7;++i) seen[i]=0;
    for(int i=0;i<7;++i) { int k=take_piece(); check(k>=0 && k<7,4); ++seen[k]; }
    for(int i=0;i<7;++i) check(seen[i]==1,5);
  }
  initialize(); kind=1; px=3; py=18; rotation=0;
  check(fits(px,py,0) && !fits(px,py+1,0),6);
  check(!fits(-2,py,0) && !fits(8,py,0),7);
  lock_piece(); check(board[184]==2 && board[185]==2 && board[194]==2 && board[195]==2,8);
  initialize(); board[153]=3;
  for(int y=16;y<20;++y) for(int x=0;x<10;++x) board[y*10+x]=1;
  check(clear_lines()==4 && board[193]==3,9);
  for(int i=0;i<190;++i) check(board[i]==0,10);
  initialize(); kind=0; px=3; py=18;
  for(int x=0;x<10;++x) if(x<3 || x>6) board[190+x]=2;
  lock_piece(); check(lines==1 && score==100u,11);
  initialize(); kind=0; rotation=1; px=-2; py=0;
  check(fits(px,py,rotation),12); rotate_piece(); check(rotation==2 && fits(px,py,rotation),13);
  initialize(); for(int x=0;x<10;++x) { board[x]=1; board[10+x]=1; }
  spawn(); check(over,14);
  initialize(); key(' '); check(started && !paused,15);
  key('P'); int y=py; key('S'); check(paused && py==y,16);
  key('P'); check(!paused,17);
  kind=2; rotation=0; px=3; py=0;
  key(' '); check(rotation==1 && py==0 && score==0u,21);
  key('w'); check(score>0,18);
  initialize(); key(' '); key('W'); check(score>0,22);
  key('Q'); check(quit,19);
  lines=80u; check(interval()==100u,20);
  system.io.PutString("TETRIS LOGIC PASS\n"); system.Exit(0);
  for(;;) system.Yield();
}
