#include "proc.h"
#include <elf.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define min(x, y)         ((x) < (y) ? (x) : (y))
#define PTE_ADDR(pte)     ((uint32_t)(pte) & ~0xfff)
#define OFF(va)           ((uint32_t)(va) & 0xfff)

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t count);
extern off_t fs_lseek(int fd, off_t offset, int whence);
extern int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  //In PA 3.2
  /*Elf_Ehdr elf;
  Elf_Phdr ph;
  ramdisk_read(&elf, 0, sizeof(Elf_Ehdr));
  for (size_t i = 0; i < elf.e_phnum; i ++) {
    ramdisk_read(&ph, elf.e_phoff + i * sizeof(Elf_Phdr), sizeof(Elf_Phdr));
    if (ph.p_type == PT_LOAD) {
      ramdisk_read((void *)ph.p_vaddr, ph.p_offset, ph.p_filesz);
      memset((void *)(ph.p_vaddr + ph.p_filesz), 0, ph.p_memsz - ph.p_filesz); 
    }
  }
  return elf.e_entry;*/

  //In PA 3.3
  Elf_Ehdr elf_ehdr;
  Elf_Phdr elf_phdr;
  
  int fd = fs_open(filename, 0, 0);
  fs_read(fd, &elf_ehdr, sizeof(Elf_Ehdr));
  for (size_t i = 0; i < elf_ehdr.e_phnum; i ++) {
    fs_lseek(fd, elf_ehdr.e_phoff + i * sizeof(Elf_Phdr), SEEK_SET);
    fs_read(fd, (void *)&elf_phdr, sizeof(Elf_Phdr));
    if (elf_phdr.p_type == PT_LOAD) {
      fs_lseek(fd, elf_phdr.p_offset, SEEK_SET);
      //undef HAS_VME
      //fs_read(fd, (void *)elf_phdr.p_vaddr, elf_phdr.p_filesz); 
      
      //In PA 4.2
      int32_t left_size = elf_phdr.p_filesz;
      void *st = (void *)elf_phdr.p_vaddr;
      while (left_size > 0) {
        void *page_base = new_page(1);  
        _map(&pcb->as, st, page_base, 0);
        uint32_t write_size = min(left_size, PGSIZE);
        fs_read(fd, page_base, write_size);
        left_size -= write_size;
        st += write_size;
      }
      //memset((void *)(elf_phdr.p_vaddr + elf_phdr.p_filesz), 0, elf_phdr.p_memsz - elf_phdr.p_filesz);
      left_size = elf_phdr.p_memsz - elf_phdr.p_filesz;
      while (left_size > 0) {
        void *page_base = new_page(1);  
        _map(&pcb->as, st, page_base, 0);
        memset(page_base, 0, PGSIZE);
        left_size -= PGSIZE;
        st += PGSIZE;
      }
    }
  }
  fs_close(fd);
  
  return elf_ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  _protect(&pcb->as);//In PA 4.2
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
