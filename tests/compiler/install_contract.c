#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
static aurora_word_t program[]={0x000e15u,0x82000000u,0x000e0042u,0x000e0048u,
  0x000115u,23u,0x000015u,8u,0x000037u,2u,0x000030u,0x8000000au};
static aurora_fs_info info;
static aurora_fs_read_request request;
static aurora_word_t readback[64], path[128];
static void check(int ok,int stage) {
  if(!ok) { system.io.PutString("INSTALL FAIL "); system.io.PutInt(stage); system.Exit(stage); }
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  check(system.fs.Install("invalid",program,0u)==AURORA_EINVAL,1);
  check(system.fs.Install("invalid",program,131073u)==AURORA_EINVAL,2);
  check(system.fs.Install("invalid",(const aurora_word_t *)0x01000000u,1u)==AURORA_EFAULT,3);
  check(system.fs.Install("invalid",(const aurora_word_t *)0x82000000u,1u)==AURORA_EFAULT,4);
  check(system.fs.Install("invalid",(const aurora_word_t *)0x813fffffu,2u)==AURORA_EFAULT,5);
  check(system.fs.Install((const char *)0xffffffffu,program,12u)==AURORA_EFAULT,6);
  for(aurora_word_t i=0;i<128u;++i) path[i]='a';
  check(system.fs.Install((const char *)path,program,12u)==AURORA_EINVAL,7);
  check(system.fs.Install("../invalid",program,12u)==AURORA_ENOENT,8);
  check(system.fs.Install("/",program,12u)==AURORA_EINVAL,9);
  check(system.fs.Install("calculator",program,12u)==AURORA_EEXIST,10);
  check(system.fs.Install("docs/notes.txt",program,12u)==AURORA_EEXIST,11);
  check(system.fs.Install("docs",program,12u)==AURORA_EEXIST,12);
  int id=system.fs.Install("docs/installed",program,12u); check(id>0,13);
  check(system.fs.Info((aurora_word_t)id,&info)==0 && info.type==1u && info.words==34u && info.capacity==34u,14);
  check(system.fs.Save((aurora_word_t)id,program,12u)==AURORA_EPERM,15);
  request.id=(aurora_word_t)id; request.offset=0u; request.count=64u;
  request.buffer=(aurora_word_t)(unsigned long)readback;
  check(system.fs.Read(&request)==34,16);
  check(readback[0]==0x41455845u && readback[4]==0x80000000u && readback[16]==0x80000000u && readback[17]==22u,17);
  for(aurora_word_t i=0;i<12u;++i) check(readback[22u+i]==program[i],18);
  check(system.fs.Install("docs/installed",program,12u)==AURORA_EEXIST,19);
  system.io.PutString("INSTALL CONTRACT PASS\n"); system.Exit(0);
  for(;;) system.Yield();
}
