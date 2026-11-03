#ifndef AURORA_KERNEL_FILESYSTEM_H
#define AURORA_KERNEL_FILESYSTEM_H
#include "storage.h"
/* ASFS2: IDs are slot+1, root=0. Parent links form an acyclic tree. */
enum { FS_MAGIC=0x41534653u, FS_NAME_WORDS=32u, FS_MAX_FILES=64u };
static volatile aurora_word_t fs_words, fs_count, fs_directory, fs_entry_words, fs_data, fs_version;
typedef struct kernel_file {
  aurora_word_t offset, words, checksum, id, type, parent, capacity;
} kernel_file;
static aurora_word_t fs_at(aurora_word_t id) { return fs_directory+(id-1u)*fs_entry_words; }
static int fs_get(aurora_word_t id, kernel_file *f) {
  if (!fs_words || id > fs_count) return 0;
  f->id=id; f->offset=f->words=f->checksum=f->parent=f->capacity=0u; f->type=3u;
  if (!id) return 1;
  aurora_word_t at=fs_at(id);
  f->type=storage_read(at); if (!f->type) return 0;
  f->offset=storage_read(at+1u); f->words=storage_read(at+2u); f->checksum=storage_read(at+3u);
  f->parent=fs_version==2u?storage_read(at+36u):0u;
  f->capacity=fs_version==2u?storage_read(at+37u):f->words;
  return 1;
}
__attribute__((noinline,optnone)) static int fs_mount(void) {
  fs_words=fs_count=0u;
  aurora_word_t version=storage_read(1), words=storage_read(3), count=storage_read(4);
  aurora_word_t dir=storage_read(5), stride=storage_read(6), data=storage_read(7);
  if (storage_read(0)!=FS_MAGIC || (version!=1u && version!=2u) || storage_read(2)!=16u ||
      storage_read(8)!=(version==1u?1u:3u) || words<16u || words>STORAGE_WORDS || count>64u ||
      stride<40u || stride>256u || dir<16u || data>words || !storage_range(dir,count*stride,data)) return 0;
  for (aurora_word_t i=9u;i<16u;++i) if(storage_read(i)) return 0;
  for (aurora_word_t i=0u;i<count;++i) {
    aurora_word_t at=dir+i*stride, type=storage_read(at);
    if (!type && version==2u) continue;
    aurora_word_t off=storage_read(at+1u), size=storage_read(at+2u), sum=storage_read(at+3u);
    aurora_word_t parent=version==2u?storage_read(at+36u):0u;
    aurora_word_t cap=version==2u?storage_read(at+37u):size;
    if(type<1u || type>3u || (version==1u && type!=1u) || parent>count || parent==i+1u) return 0;
    if(parent && storage_read(dir+(parent-1u)*stride)!=3u) return 0;
    aurora_word_t ancestor=parent, hops=0u;
    while(ancestor) {
      if(ancestor>count || ancestor==i+1u || ++hops>count || storage_read(dir+(ancestor-1u)*stride)!=3u) return 0;
      ancestor=storage_read(dir+(ancestor-1u)*stride+36u);
    }
    if(type==3u) { if(off || size || sum || cap) return 0; }
    else if(off<data || size>cap || !storage_range(off,cap,words) ||
            (type==1u && (size<16u || cap!=size)) || (type==2u && (!cap || cap>4096u))) return 0;
    aurora_word_t len=0u;
    while(len<32u) {
      aurora_word_t c=storage_read(at+4u+len); if(!c) break;
      if(!((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='-'||c=='_'||c=='.')) return 0;
      ++len;
    }
    if(!len || len==32u || (storage_read(at+4u)=='.' &&
        (len==1u || (len==2u && storage_read(at+5u)=='.')))) return 0;
    for(aurora_word_t j=len;j<32u;++j) if(storage_read(at+4u+j)) return 0;
    for(aurora_word_t j=version==2u?38u:36u;j<stride;++j) if(storage_read(at+j)) return 0;
    for(aurora_word_t j=0u;j<i;++j) {
      aurora_word_t other=dir+j*stride, ot=storage_read(other); if(!ot) continue;
      aurora_word_t begin=storage_read(other+1u), n=storage_read(other+(version==2u?37u:2u));
      if(type!=3u && ot!=3u && off<begin+n && begin<off+cap) return 0;
      if(parent!=(version==2u?storage_read(other+36u):0u)) continue;
      aurora_word_t k=0u; while(k<32u && storage_read(other+4u+k)==storage_read(at+4u+k)) ++k;
      if(k==32u) return 0;
    }
  }
  fs_version=version; fs_words=words; fs_count=count; fs_directory=dir; fs_entry_words=stride; fs_data=data;
  return 1;
}
static aurora_word_t fs_read(const kernel_file *f,aurora_word_t offset) { return storage_read(f->offset+offset); }
static int fs_open(const aurora_word_t *path,kernel_file *f) {
  aurora_word_t pos=0u,parent=0u;
  if(path[pos]=='/') ++pos;
  if(!path[pos]) return fs_get(0u,f);
  while(pos<128u) {
    aurora_word_t start=pos;
    while(pos<128u && path[pos] && path[pos]!='/') ++pos;
    aurora_word_t len=pos-start, found=0u;
    if(!len || len>31u) return 0;
    for(aurora_word_t id=1u;id<=fs_count;++id) {
      if(!fs_get(id,f) || f->parent!=parent) continue;
      aurora_word_t at=fs_at(id), j=0u;
      while(j<len && storage_read(at+4u+j)==path[start+j]) ++j;
      if(j==len && !storage_read(at+4u+len)) { found=1u; break; }
    }
    if(!found) return 0;
    if(!path[pos]) return 1;
    if(f->type!=3u) return 0;
    parent=f->id; ++pos;
    if(!path[pos]) return 1;
  }
  return 0;
}
static aurora_word_t fs_program_count(void) {
  aurora_word_t n=0u; for(aurora_word_t i=1;i<=fs_count;++i) if(storage_read(fs_at(i))==1u) ++n; return n;
}
static void fs_name(aurora_word_t id) {
  if(!id) { console_putchar('/'); return; }
  for(aurora_word_t j=0;j<32u;++j) { aurora_word_t c=storage_read(fs_at(id)+4u+j); if(!c) break; console_putchar(c); }
}
static void fs_list(aurora_word_t parent) {
  aurora_word_t width=4u; kernel_file f;
  for(aurora_word_t id=1;id<=fs_count;++id) if(fs_get(id,&f) && f.parent==parent) {
    aurora_word_t n=0u; while(n<32u && storage_read(fs_at(id)+4u+n)) ++n; if(n>width) width=n;
  }
  kernel_boot_put_string("NAME"); for(aurora_word_t j=4;j<width;++j) console_putchar(' ');
  kernel_boot_put_string("  TYPE  SIZE (words)\n");
  for(aurora_word_t id=1;id<=fs_count;++id) if(fs_get(id,&f) && f.parent==parent) {
    aurora_word_t j=0u; while(j<32u) { aurora_word_t c=storage_read(fs_at(id)+4u+j); if(!c) break; console_putchar(c); ++j; }
    for(;j<width;++j) console_putchar(' ');
    kernel_boot_put_string(f.type==1u?"  EXE   ":f.type==2u?"  TXT   ":"  DIR   ");
    aurora_word_t digits=1u; for(aurora_word_t n=f.words;n>=10u;n/=10u) ++digits;
    for(;digits<12u;++digits) console_putchar(' ');
    kernel_console_put_uint(f.words); console_putchar('\n');
  }
}
static void fs_tree(void) {
  aurora_word_t parent=0u,next=1u,depth=0u; kernel_file f;
  kernel_boot_put_string("/\n");
  for(;;) {
    aurora_word_t id=next; for(;id<=fs_count;++id) if(fs_get(id,&f)&&f.parent==parent) break;
    if(id<=fs_count) {
      for(aurora_word_t j=0;j<=depth;++j) kernel_boot_put_string("  ");
      fs_name(id); if(f.type==3u) console_putchar('/'); console_putchar('\n');
      if(f.type==3u) { parent=id; next=1u; ++depth; } else next=id+1u;
    } else { if(!parent) break; next=parent+1u; fs_get(parent,&f); parent=f.parent; --depth; }
  }
}
static int fs_checksum(const kernel_file *f) {
  aurora_word_t sum=0u; for(aurora_word_t i=0;i<f->words;++i) sum+=fs_read(f,i); return sum==f->checksum;
}
/* Read-back checked writes, not power-loss atomic. A failure invalidates mount. */
static int fs_store(aurora_word_t off,aurora_word_t value) {
  aurora.Write(STORAGE_BASE+off,value);
  if(storage_read(off)==value) return 1;
  fs_words=fs_count=0u; return 0;
}
static int fs_valid_name(const aurora_word_t *name) {
  aurora_word_t n=0u;
  while(n<32u && name[n]) { aurora_word_t c=name[n++];
    if(!((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='-'||c=='_'||c=='.')) return 0;
  }
  return n && n<32u && !(name[0]=='.' && (n==1u || (n==2u && name[1]=='.')));
}
static int fs_sibling(aurora_word_t parent,const aurora_word_t *name) {
  kernel_file f; for(aurora_word_t id=1;id<=fs_count;++id) if(fs_get(id,&f)&&f.parent==parent) {
    aurora_word_t j=0; while(j<32u && name[j]==storage_read(fs_at(id)+4u+j)) { if(!name[j]) return 1; ++j; }
  } return 0;
}
/* First-fit among at most 64 extents. Never copy/compact payloads. */
static aurora_word_t fs_allocate(aurora_word_t cap) {
  aurora_word_t candidate=fs_data;
  for(aurora_word_t pass=0u;pass<=fs_count;++pass) {
    if(!storage_range(candidate,cap,STORAGE_WORDS)) return 0u;
    aurora_word_t next=candidate; kernel_file f;
    for(aurora_word_t id=1;id<=fs_count;++id) if(fs_get(id,&f)&&f.type!=3u) {
      if(candidate<f.offset+f.capacity && f.offset<candidate+cap && next<f.offset+f.capacity) next=f.offset+f.capacity;
    }
    if(next==candidate) return candidate;
    candidate=next;
  } return 0u;
}
static aurora_status_t fs_create(const aurora_word_t *path,aurora_word_t type) {
  if(!fs_words || fs_version!=2u) return AURORA_EPERM;
  aurora_word_t split=0u,n=0u,parent_path[128]; kernel_file p;
  while(n<127u && path[n]) { parent_path[n]=path[n]; if(path[n]=='/') split=n+1u; ++n; }
  if(n==127u || !fs_valid_name(path+split)) return AURORA_EINVAL;
  parent_path[split]=0u;
  if(!fs_open(parent_path,&p) || p.type!=3u) return AURORA_ENOENT;
  if(fs_sibling(p.id,path+split)) return AURORA_EEXIST;
  aurora_word_t id=1u; for(;id<=fs_count;++id) if(!storage_read(fs_at(id))) break;
  if(id>64u || !storage_range(fs_directory,id*fs_entry_words,fs_data)) return AURORA_ENOSPC;
  aurora_word_t cap=type==2u?2048u:0u, at=fs_at(id), off=cap?fs_allocate(cap):0u;
  if(cap && !off) return AURORA_ENOSPC;
  if(off+cap>fs_words) { if(!fs_store(3u,off+cap)) return AURORA_EIO; fs_words=off+cap; }
  for(aurora_word_t j=0;j<fs_entry_words;++j) if(!fs_store(at+j,0u)) return AURORA_EIO;
  if(!fs_store(at+1u,cap?off:0u)||!fs_store(at+36u,p.id)||!fs_store(at+37u,cap)) return AURORA_EIO;
  for(aurora_word_t j=0;path[split+j];++j) if(!fs_store(at+4u+j,path[split+j])) return AURORA_EIO;
  if(!fs_store(at,type)) return AURORA_EIO;
  if(id>fs_count) { if(!fs_store(4u,id)) return AURORA_EIO; fs_count=id; }
  return AURORA_OK;
}
static aurora_status_t fs_remove(const kernel_file *f) {
  if(!fs_words || fs_version!=2u || !f->id) return AURORA_EPERM;
  /* Do not recycle a live user's ID for a different file behind its back. */
  if(process_table[PROCESS_USER].valid && user_file_refs[f->id]) return AURORA_EBUSY;
  kernel_file child; for(aurora_word_t id=1;id<=fs_count;++id)
    if(fs_get(id,&child)&&child.parent==f->id) return AURORA_EBUSY;
  return fs_store(fs_at(f->id),0u)?AURORA_OK:AURORA_EIO;
}
static aurora_status_t fs_rename(const kernel_file *f,const aurora_word_t *name) {
  if(!fs_words || fs_version!=2u || !f->id) return AURORA_EPERM;
  if(!fs_valid_name(name)) return AURORA_EINVAL;
  if(fs_sibling(f->parent,name)) return AURORA_EEXIST;
  aurora_word_t end=0u;
  for(aurora_word_t j=0;j<32u;++j) { if(!end && !name[j]) end=1u;
    aurora_word_t value=end?0u:name[j], at=fs_at(f->id)+4u+j;
    if(storage_read(at)!=value && !fs_store(at,value)) return AURORA_EIO;
  } return AURORA_OK;
}
static aurora_status_t fs_move(const kernel_file *f,const aurora_word_t *path) {
  if(!fs_words || fs_version!=2u || !f->id) return AURORA_EPERM;
  aurora_word_t parent_path[128], name[32], split=0u,n=0u; kernel_file dest;
  if(fs_open(path,&dest)) {
    if(dest.type!=3u) return AURORA_EEXIST;
    for(aurora_word_t j=0;j<32u;++j) name[j]=storage_read(fs_at(f->id)+4u+j);
  } else {
    while(n<127u && path[n]) { parent_path[n]=path[n]; if(path[n]=='/') split=n+1u; ++n; }
    if(n==127u || !fs_valid_name(path+split)) return AURORA_EINVAL;
    parent_path[split]=0u;
    if(!fs_open(parent_path,&dest)||dest.type!=3u) return AURORA_ENOENT;
    for(aurora_word_t j=0;j<32u;++j) name[j]=j<n-split?path[split+j]:0u;
  }
  aurora_word_t ancestor=dest.id; kernel_file p;
  for(aurora_word_t hop=0;ancestor && hop<64u;++hop) {
    if(ancestor==f->id) return AURORA_EINVAL;
    if(!fs_get(ancestor,&p)) return AURORA_EINVAL; ancestor=p.parent;
  }
  if(ancestor) return AURORA_EINVAL;
  if(fs_sibling(dest.id,name)) return AURORA_EEXIST;
  /* Only the parent and name change. File ID, offset and capacity stay fixed. */
  for(aurora_word_t j=0;j<32u;++j) {
    aurora_word_t at=fs_at(f->id)+4u+j;
    if(storage_read(at)!=name[j] && !fs_store(at,name[j])) return AURORA_EIO;
  }
  return (f->parent==dest.id || fs_store(fs_at(f->id)+36u,dest.id))?AURORA_OK:AURORA_EIO;
}
static aurora_word_t fs_allocated(void) {
  aurora_word_t n=fs_data; kernel_file f;
  for(aurora_word_t id=1;id<=fs_count;++id) if(fs_get(id,&f)) n+=f.capacity;
  return n;
}
#endif
