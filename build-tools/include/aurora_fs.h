#ifndef AURORA_FS_H
#define AURORA_FS_H
/* Read-only browsing plus bounded replacement of existing text files.
 * IDs remain valid until shell management/remount; no per-process open handles. */
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_fs_open(const char *path) {
  return aurora_syscall(AURORA_SYSCALL_FS_OPEN,(aurora_word_t)(unsigned long)path,0,0);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_fs_list(aurora_word_t directory,aurora_word_t ordinal,aurora_fs_info *out) {
  return aurora_syscall(AURORA_SYSCALL_FS_LIST,directory,ordinal,(aurora_word_t)(unsigned long)out);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_fs_info(aurora_word_t id,aurora_fs_info *out) {
  return aurora_syscall(AURORA_SYSCALL_FS_INFO,id,(aurora_word_t)(unsigned long)out,0);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_fs_read(const aurora_fs_read_request *request) {
  return aurora_syscall(AURORA_SYSCALL_FS_READ,(aurora_word_t)(unsigned long)request,0,0);
}
static __inline__ __attribute__((always_inline)) aurora_status_t
__aurora_fs_save(aurora_word_t id,const aurora_word_t *text,aurora_word_t count) {
  return aurora_syscall(AURORA_SYSCALL_FS_SAVE,id,(aurora_word_t)(unsigned long)text,count);
}
typedef struct aurora_system_fs_api {
  aurora_status_t (*Open)(const char *);
  aurora_status_t (*List)(aurora_word_t,aurora_word_t,aurora_fs_info *);
  aurora_status_t (*Info)(aurora_word_t,aurora_fs_info *);
  aurora_status_t (*Read)(const aurora_fs_read_request *);
  aurora_status_t (*Save)(aurora_word_t,const aurora_word_t *,aurora_word_t);
} aurora_system_fs_api;
#endif
