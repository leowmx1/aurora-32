#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
#define TEXT_MAX 4096u
/* Only [0,length) / successfully read prefixes are consumed. No need to clear
 * 8192 words on every launch; stale RAM outside these prefixes is never read. */
static aurora_word_t text[TEXT_MAX] __attribute__((section(".noinit")));
static aurora_word_t scratch[TEXT_MAX] __attribute__((section(".noinit")));
static aurora_word_t line[160];
static aurora_fs_info info, current;
static aurora_fs_read_request request;
static aurora_word_t length, used;
static volatile aurora_word_t opened, dirty, skip_lf, overflow;

/* Bounded batches keep long help/error strings from masking IRQs for an entire
 * message, and avoid one syscall per filename character. */
static void put_text(const char *s) {
  while(*s) {
    aurora_word_t n=0u; while(n<64u && s[n]) ++n;
    if(system.io.Write((const aurora_word_t *)s,n)!=(int)n) system.Exit(1);
    s+=n;
  }
}
static void words(const aurora_word_t *s) {
  aurora_word_t n=0u; while(n<32u && s[n]) ++n;
  if(n && system.io.Write(s,n)!=(int)n) system.Exit(1);
}
static int equal(const aurora_word_t *s,const char *t) {
  while(*s && *s==*t) { ++s; ++t; } return *s==*t;
}
static void error(int code) { put_text("Error: "); system.io.PutInt(code); system.io.PutChar('\n'); }
static void prompt(void) { put_text(dirty?"txt* > ":"txt > "); }
static void show(void) {
  if(!opened) { put_text("Open a text file first.\n"); return; }
  for(aurora_word_t pos=0u;pos<length;) {
    aurora_word_t n=length-pos; if(n>64u) n=64u;
    if(system.io.Write(text+pos,n)<0) { error(AURORA_EIO); return; } pos+=n;
  }
  if(!length || text[length-1u]!='\n') system.io.PutChar('\n');
}
static void list(const aurora_word_t *path) {
  int id=system.fs.Open((const char *)path); if(id<0) { error(id); return; }
  for(aurora_word_t i=0u;;++i) {
    int s=system.fs.List((aurora_word_t)id,i,&info);
    if(s<=0) { if(s<0) error(s); break; }
    put_text(info.type==3u?"DIR  ":info.type==2u?"TXT  ":"EXE  ");
    words(info.name); if(info.type==3u) system.io.PutChar('/');
    else { put_text("  "); system.io.PutInt((int)info.words); }
    system.io.PutChar('\n');
  }
}
static void open_text(const aurora_word_t *path) {
  if(dirty) { put_text("Unsaved edits: save or discard first.\n"); return; }
  int id=system.fs.Open((const char *)path); if(id<0) { error(id); return; }
  int status=system.fs.Info((aurora_word_t)id,&info); if(status<0) { error(status); return; }
  if(info.type!=2u || info.words>TEXT_MAX) { put_text("Not a supported text file.\n"); return; }
  request.id=(aurora_word_t)id; request.offset=0u;
  while(request.offset<info.words) {
    request.count=info.words-request.offset; if(request.count>64u) request.count=64u;
    request.buffer=(aurora_word_t)(unsigned long)(scratch+request.offset);
    status=system.fs.Read(&request);
    if(status<=0) { error(status<0?status:AURORA_EIO); return; }
    request.offset+=(aurora_word_t)status;
  }
  /* Commit local view only after all reads succeed. */
  current.id=info.id; current.capacity=info.capacity;
  length=info.words; for(aurora_word_t i=0;i<length;++i) text[i]=scratch[i];
  opened=1u; dirty=0u; show();
}
static void help(void) {
  put_text("ls [PATH] | open PATH | show | help\n"
      "append TEXT    Append a line\nset N TEXT     Replace line N (from 1)\n"
      "delete N       Delete line N\nclear          Empty draft\n"
      "save           Save to EEPROM\ndiscard        Close without saving\n"
      "q              Exit (save/discard edits first)\n"
      "Paths start at root; no current directory.\n");
}
static void execute(void) {
  line[used]=0u;
  aurora_word_t *arg=line; while(*arg && *arg!=' ') ++arg;
  if(*arg) *arg++=0u; while(*arg==' ') ++arg;
  if(equal(line,"q")||equal(line,"quit")) {
    if(dirty) put_text("Unsaved edits: save or discard first.\n"); else system.Exit(0); return;
  }
  if(equal(line,"help")) { help(); return; }
  if(equal(line,"ls")) { list(arg); return; }
  if(equal(line,"open")) { if(!*arg) error(AURORA_EINVAL); else open_text(arg); return; }
  if(equal(line,"discard")) { opened=dirty=0u; length=0u; put_text("Draft discarded.\n"); return; }
  if(equal(line,"show")) { show(); return; }
  if(!opened) { put_text("Open a text file first.\n"); return; }
  if(equal(line,"save")) {
    int s=system.fs.Save(current.id,text,length);
    if(s!=(int)length) error(s); else { dirty=0u; put_text("Saved.\n"); } return;
  }
  if(equal(line,"clear")) { length=0u; dirty=1u; return; }
  aurora_word_t begin=length,end=length,add=0u,prefix=0u;
  if(equal(line,"append")) { if(length && text[length-1u]!='\n') prefix=1u; }
  else if(equal(line,"set")||equal(line,"delete")) {
    aurora_word_t number=0u;
    if(*arg<'0'||*arg>'9') { error(AURORA_EINVAL); return; }
    while(*arg>='0'&&*arg<='9') {
      if(number>TEXT_MAX) { error(AURORA_EINVAL); return; }
      number=number*10u+(*arg++-'0');
    }
    if(*arg && *arg!=' ') { error(AURORA_EINVAL); return; }
    while(*arg==' ') ++arg;
    if(!number) { error(AURORA_EINVAL); return; }
    begin=0u;
    for(aurora_word_t n=1;n<number && begin<length;++n) {
      while(begin<length && text[begin]!='\n') ++begin; if(begin<length) ++begin;
    }
    if(begin>=length) { error(AURORA_EINVAL); return; }
    end=begin; while(end<length && text[end]!='\n') ++end; if(end<length) ++end;
    if(equal(line,"delete")) {
      if(*arg) { error(AURORA_EINVAL); return; }
      for(aurora_word_t i=end;i<length;++i) text[begin+i-end]=text[i];
      length-=end-begin; dirty=1u; return;
    }
  } else { put_text("Unknown command. Type help.\n"); return; }
  while(arg[add]) ++add;
  aurora_word_t total=length-(end-begin)+add+1u+prefix;
  if(total>current.capacity || total>TEXT_MAX) { error(AURORA_ENOSPC); return; }
  aurora_word_t at=0u;
  for(aurora_word_t i=0;i<begin;++i) scratch[at++]=text[i];
  if(prefix) scratch[at++]='\n';
  for(aurora_word_t i=0;i<add;++i) scratch[at++]=arg[i]; scratch[at++]='\n';
  for(aurora_word_t i=end;i<length;++i) scratch[at++]=text[i];
  for(aurora_word_t i=0;i<at;++i) text[i]=scratch[i];
  length=at; dirty=1u;
}
static void key(aurora_word_t event) {
  aurora_word_t c=aurora_event_character(event);
  if(system.EventDrops()) { put_text("Input overflow; exiting without saving.\n"); system.Exit(1); }
  if(c=='\n' && skip_lf) { skip_lf=0u; return; }
  skip_lf=c=='\r';
  if(c=='\r'||c=='\n') {
    system.io.PutChar('\n');
    if(overflow) put_text("Command too long; discarded.\n"); else if(used) execute();
    used=overflow=0u; prompt(); return;
  }
  if(c==27u) { used=overflow=0u; system.io.PutChar('\n'); prompt(); return; }
  if(overflow) return;
  if(c==8u||c==127u) { if(used) { --used; system.io.PutChar('\b'); } return; }
  if(c<32u||c>126u) return;
  if(used>=159u) { overflow=1u; return; }
  line[used++]=c; system.io.PutChar((char)c);
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  system.DeferEvents();
  if(system.io.Subscribe(key)!=AURORA_OK) system.Exit(1);
  put_text("Aurora-32 Text Viewer / Editor\n"); help(); prompt();
  system.ResumeEvents();
  for(;;) system.WaitEvent();
}
