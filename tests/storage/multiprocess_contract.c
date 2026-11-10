#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
static volatile aurora_word_t identity, calls, keys;
static volatile aurora_word_t guard[8];
static aurora_fs_info info;
static aurora_fs_read_request read_request;
static aurora_word_t text[64];
static aurora_graphics_bitmap bitmap={10u,10u,2u,2u,3u,4u,9u,0u};
static const aurora_word_t tune[4]={64u,80u,20u,0u};
static void check(int ok,int code) {
  if(!ok) { system.io.PutString("MULTIPROCESS FAIL "); system.io.PutInt(code); system.Exit(code); }
}
static void key(aurora_word_t event) {
  aurora_word_t c=aurora_event_character(event);
  ++keys;
  if(c=='q') system.Exit(0);
  if(c=='g') {
    aurora_status_t status=system.graphics.Bitmap(&bitmap);
    system.io.PutString("GPU="); system.io.PutInt(status); system.io.PutChar('\n');
  } else if(c=='m') {
    aurora_status_t status=system.midi.Sequence(tune,1u,0u);
    system.io.PutString("MIDI="); system.io.PutInt(status); system.io.PutChar('\n');
  } else {
    system.io.PutChar('['); system.io.PutChar(c); system.io.PutString("]\n");
  }
}
/* Keep distinct live frames over tick, sleep and callbacks in all four banks. */
__attribute__((noinline,optnone)) static void hold(aurora_word_t seed,aurora_word_t depth) {
  volatile aurora_word_t saved[4];
  for(aurora_word_t i=0u;i<4u;++i) saved[i]=seed+i+depth;
  if(depth) hold(seed+17u,depth-1u);
  else check(system.Sleep(10u)==0,31);
  for(aurora_word_t i=0u;i<4u;++i) check(saved[i]==seed+i+depth,32);
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  identity=system.TimeMs()+12345u;
  for(aurora_word_t i=0u;i<8u;++i) guard[i]=identity+i;
  aurora_status_t status=system.io.Subscribe(key);
  check(status==0 || status==AURORA_EBUSY,1);
  int bg=status==AURORA_EBUSY;
  aurora_status_t file;
  do { file=system.fs.Open("docs/welcome.txt"); if(file==AURORA_EBUSY) system.Yield(); } while(file==AURORA_EBUSY);
  check(file>0,2);
  do { status=system.fs.Info(file,&info); if(status==AURORA_EBUSY) system.Yield(); } while(status==AURORA_EBUSY);
  check(status==0 && info.id==(aurora_word_t)file && info.type==2u,3);
  read_request.id=file; read_request.offset=0u;
  read_request.buffer=(aurora_word_t)(unsigned long)text; read_request.count=7u;
  do { status=system.fs.Read(&read_request); if(status==AURORA_EBUSY) system.Yield(); } while(status==AURORA_EBUSY);
  check(status==7 && text[0]=='W' && text[6]=='e',4);
  check(system.io.Write(text,7u)==7,5);
  system.io.PutString(bg?" BG READY\n":" FG READY\n");
  aurora_word_t start=system.TimeMs();
  for(;;) {
    hold(identity,2u);
    ++calls;
    for(aurora_word_t i=0u;i<8u;++i) check(guard[i]==identity+i,33);
    if(bg && system.TimeMs()-start>=1000000u) system.Exit(17);
  }
}
