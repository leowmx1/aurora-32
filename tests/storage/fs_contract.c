#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
static aurora_fs_info info;
static aurora_fs_read_request request;
static aurora_word_t buffer[64];
static aurora_word_t text[4]={'O','K','!','\n'};
static void check(int condition,int stage) {
  if(!condition) { system.io.PutString("FS FAIL "); system.io.PutInt(stage); system.Exit(stage); }
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  int dir=system.fs.Open("/docs"), id=system.fs.Open("docs/notes.txt");
  check(dir>0 && id>0,1);
  check(system.fs.Open("/")==0,2);
  check(system.fs.Open("docs/../calculator")==AURORA_ENOENT,3);
  check(system.fs.List((aurora_word_t)dir,0u,&info)==1 && info.type==2u,4);
  check(system.fs.List((aurora_word_t)dir,100u,&info)==0,5);
  check(system.fs.List(1u,0u,&info)==AURORA_EINVAL,6);
  check(system.fs.Info((aurora_word_t)id,&info)==0,7);
  request.id=(aurora_word_t)id; request.offset=info.words; request.count=64u;
  request.buffer=(aurora_word_t)(unsigned long)buffer;
  check(system.fs.Read(&request)==0,8);
  request.offset=info.words+1u; check(system.fs.Read(&request)==AURORA_EINVAL,9);
  request.offset=0u; request.count=65u; check(system.fs.Read(&request)==AURORA_EINVAL,10);
  request.count=2u; request.buffer=0x813fffffu;
  check(system.fs.Read(&request)==AURORA_EFAULT,11);
  request.buffer=0x80000000u; check(system.fs.Read(&request)==AURORA_EFAULT,12);
  request.buffer=0x82000000u; check(system.fs.Read(&request)==AURORA_EFAULT,13);
  check(aurora_syscall(AURORA_SYSCALL_FS_READ,0xffffffffu,0,0)==AURORA_EFAULT,14);
  check(aurora_syscall(AURORA_SYSCALL_FS_INFO,(aurora_word_t)id,0x01000000u,0)==AURORA_EFAULT,15);
  check(system.fs.Save(1u,text,4u)==AURORA_EPERM,16);
  check(system.fs.Save((aurora_word_t)dir,text,4u)==AURORA_EPERM,17);
  check(system.fs.Save((aurora_word_t)id,text,4097u)==AURORA_ENOSPC,18);
  check(aurora_syscall(AURORA_SYSCALL_FS_SAVE,(aurora_word_t)id,0xffffffffu,4u)==AURORA_EFAULT,19);
  text[0]=1u; check(system.fs.Save((aurora_word_t)id,text,4u)==AURORA_EINVAL,20); text[0]='O';
  check(system.fs.Save((aurora_word_t)id,text,4u)==4,21);
  request.offset=0u; request.count=64u; request.buffer=(aurora_word_t)(unsigned long)buffer;
  check(system.fs.Read(&request)==4 && buffer[0]=='O' && buffer[3]=='\n',22);
  check(system.fs.Info((aurora_word_t)id,&info)==0 && info.words==4u,23);
  check(system.fs.Save((aurora_word_t)id,text,0u)==0,24);
  check(system.fs.Read(&request)==0,25);
  system.io.PutString("FS CONTRACT PASS\n"); system.Exit(0);
}
