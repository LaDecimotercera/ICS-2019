#include "fs.h"
#include <sys/types.h>
#include <unistd.h>

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fbsync_write(const void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENTS, FD_TTY, FD_DISPINFO, FD_FDSYNC, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
  {"/dev/tty", 0, 0, 0,invalid_read, serial_write},
  {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
  {"/dev/fbsync", 0, 0, 0, invalid_read, fbsync_write},
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write}, 
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))
void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[NR_FILES - 1].size = screen_height() * screen_width() * sizeof(uint32_t);
}

int fs_open(const char *pathname, int flags, int mode) {
  for(int i = 0; i < NR_FILES; i ++) {
    if(!strcmp(pathname, file_table[i].name)) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
	panic("should not reach here(triggered by fs_open)");
}

ssize_t fs_read(int fd, void *buf, size_t count) {
  assert(fd >= 0 && fd < NR_FILES);
  if (file_table[fd].read) {
    size_t len = file_table[fd].read(buf, file_table[fd].open_offset, count);
    file_table[fd].open_offset += len;
    return len;
  } else {
    count = file_table[fd].open_offset + count > file_table[fd].size ? \
          file_table[fd].size - file_table[fd].open_offset : count;        
    size_t len = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);
    file_table[fd].open_offset += len;
    return len;
  } 
}

ssize_t fs_write(int fd, const void *buf, size_t count) {
  assert(0 <= fd && fd < NR_FILES);
  if (file_table[fd].read) {
    size_t len = file_table[fd].write(buf, file_table[fd].open_offset, count);
    file_table[fd].open_offset += len;
    return len;
  } else {
    count = file_table[fd].open_offset + count > file_table[fd].size ? \
          file_table[fd].size - file_table[fd].open_offset : count;        
    size_t len = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);
    file_table[fd].open_offset += len;
    return len;
  }
  /*	size_t fs_size = file_table[fd].size;
  //printf("fs_write filename:%s\n", file_table[fd].name);
	switch(fd) {
		case FD_STDIN: break;
		case FD_TTY:
		case FD_STDOUT:
		case FD_STDERR:
			file_table[fd].write(buf, 0, count);
			break;
		case FD_FB:
			if(file_table[fd].open_offset >= fs_size)
				return 0;
			if(file_table[fd].open_offset + count > fs_size)
				count = fs_size - file_table[fd].open_offset;
			file_table[fd].write(buf, file_table[fd].open_offset, count);
			file_table[fd].open_offset += count;
			break;
		case FD_EVENTS:
		case FD_DISPINFO:
			break;
		default:
			// write to ramdisk
			if(file_table[fd].open_offset >= fs_size)
				return 0;
			if(file_table[fd].open_offset + count > fs_size)
				count = fs_size - file_table[fd].open_offset;

			//printf("write file: size = %d, len = %d, file open_offset = %d\n", fs_size, len, file_table[fd].open_offset);
			ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);
			file_table[fd].open_offset += count;
			break;
	}
	return count;*/
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  assert(0 <= fd && fd < NR_FILES);
  switch (whence) {
    case SEEK_SET: 
      file_table[fd].open_offset = offset; break;
    case SEEK_CUR: 
      file_table[fd].open_offset += offset; break;
    case SEEK_END: 
      file_table[fd].open_offset = file_table[fd].size + offset; break;
    default: panic("trigger whence error");  
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  file_table[fd].open_offset = 0;
  return 0;
}