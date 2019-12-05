#include "common.h"
#include "syscall.h"
#include <sys/types.h>
#include <unistd.h>
#include "proc.h"
#include "fs.h"

extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t count);
extern ssize_t fs_write(int fd, const void *buf, size_t count);
extern int fs_close(int fd);
extern off_t fs_lseek(int fd, off_t offset, int whence);
extern void naive_uload(PCB *pcb, const char *filename);

/*static inline size_t sys_write(int fd, void *buf, size_t count){
  if (fd == 1 || fd == 2){
    char ch[count];
    memcpy(ch, buf, count);
    for (int i = 0; i < count; i ++) {
      _putc(ch[i]);
    } 
    return count;
  }
  return -1;
}*/

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield: _yield(); c->GPRx = 0; break;
    case SYS_exit: naive_uload(NULL, "/bin/init"); break;
    case SYS_write: c->GPRx = fs_write(a[1], (void *)a[2], a[3]); break; 
    case SYS_brk: c->GPRx = 0; break;
    case SYS_open: c->GPRx = fs_open((const char *)a[1],a[2],a[3]); break;
    case SYS_read: c->GPRx = fs_read(a[1], (void *)a[2], a[3]); break;
    case SYS_close: c->GPRx = fs_close(a[1]); break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1],a[2],a[3]); break;
    case SYS_execve: naive_uload(NULL, (const char*)a[1]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
