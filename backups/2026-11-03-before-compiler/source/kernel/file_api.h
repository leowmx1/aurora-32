#ifndef AURORA_KERNEL_FILE_API_H
#define AURORA_KERNEL_FILE_API_H
/* Bound full spans before any read/write. Never accept USTACK or kernel memory. */
__attribute__((noinline,optnone)) static int fs_user_span(aurora_word_t a,aurora_word_t n,int write) {
  if(!n) return 1;
  if(a>=AURORA_URAM_BEGIN && a<AURORA_URAM_END && n<=AURORA_URAM_END-a) return 1;
  return !write && a>=AURORA_UROM_BEGIN && a<AURORA_UROM_END && n<=AURORA_UROM_END-a;
}
static aurora_status_t kernel_fs_open(aurora_word_t address) {
  aurora_word_t path[128]; kernel_file f;
  for(aurora_word_t i=0;i<128u;++i) {
    if(address+i<address || !fs_user_span(address+i,1u,0)) return AURORA_EFAULT;
    path[i]=aurora.Read(address+i);
    if(!path[i]) {
      if(!fs_open(path,&f)) return AURORA_ENOENT;
      user_file_refs[f.id]=1u; return (aurora_status_t)f.id;
    }
  } return AURORA_EINVAL;
}
static aurora_status_t kernel_fs_info(aurora_word_t id,aurora_word_t out) {
  if(!fs_user_span(out,37u,1)) return AURORA_EFAULT;
  kernel_file f; if(!fs_get(id,&f)) return AURORA_ENOENT;
  user_file_refs[id]=1u;
  aurora.Write(out,id); aurora.Write(out+1u,f.type); aurora.Write(out+2u,f.words);
  aurora.Write(out+3u,f.capacity); aurora.Write(out+4u,f.parent);
  for(aurora_word_t j=0;j<32u;++j) aurora.Write(out+5u+j,id?storage_read(fs_at(id)+4u+j):(j?0u:'/'));
  return AURORA_OK;
}
static aurora_status_t kernel_fs_list(aurora_word_t parent,aurora_word_t ordinal,aurora_word_t out) {
  kernel_file f; if(!fs_get(parent,&f)) return AURORA_ENOENT;
  if(f.type!=3u) return AURORA_EINVAL;
  if(!fs_user_span(out,37u,1)) return AURORA_EFAULT;
  for(aurora_word_t id=1;id<=fs_count;++id) if(fs_get(id,&f)&&f.parent==parent) {
    if(!ordinal) { aurora_status_t s=kernel_fs_info(id,out); return s<0?s:1; } --ordinal;
  } return 0;
}
static aurora_status_t kernel_fs_read(aurora_word_t request) {
  if(!fs_user_span(request,4u,0)) return AURORA_EFAULT;
  aurora_word_t id=aurora.Read(request), off=aurora.Read(request+1u);
  aurora_word_t out=aurora.Read(request+2u), count=aurora.Read(request+3u);
  kernel_file f; if(!fs_get(id,&f)) return AURORA_ENOENT;
  if(f.type==3u || count>64u || off>f.words) return AURORA_EINVAL;
  if(!fs_user_span(out,count,1)) return AURORA_EFAULT;
  user_file_refs[id]=1u;
  if(count>f.words-off) count=f.words-off;
  for(aurora_word_t j=0;j<count;++j) aurora.Write(out+j,fs_read(&f,off+j));
  return count;
}
static aurora_status_t kernel_fs_save(aurora_word_t id,aurora_word_t data,aurora_word_t length) {
  kernel_file f; if(!fs_get(id,&f)) return AURORA_ENOENT;
  if(fs_version!=2u || f.type!=2u) return AURORA_EPERM;
  if(length>f.capacity || length>4096u) return AURORA_ENOSPC;
  if(!fs_user_span(data,length,0)) return AURORA_EFAULT;
  user_file_refs[id]=1u;
  aurora_word_t sum=0u;
  for(aurora_word_t j=0;j<length;++j) {
    aurora_word_t c=aurora.Read(data+j);
    if(c!=9u && c!=10u && c!=13u && (c<32u || c>126u)) return AURORA_EINVAL;
    sum+=c;
  }
  for(aurora_word_t j=0;j<length;++j) if(!fs_store(f.offset+j,aurora.Read(data+j))) return AURORA_EIO;
  if(!fs_store(fs_at(id)+3u,sum)||!fs_store(fs_at(id)+2u,length)) return AURORA_EIO;
  return length;
}
#endif
