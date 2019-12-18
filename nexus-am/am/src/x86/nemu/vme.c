#include <am.h>
#include <x86.h>
#include <nemu.h>
#include <klib.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void* (*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static _Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE},
  {.start = (void*)MMIO_BASE,  .end = (void*)(MMIO_BASE + MMIO_SIZE)}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;

  return 0;
}

int _protect(_AddressSpace *as) {
  PDE *updir = (PDE*)(pgalloc_usr(1));
  as->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  return 0;
}

void _unprotect(_AddressSpace *as) {
}

static _AddressSpace *cur_as = NULL;
void __am_get_cur_as(_Context *c) {
  c->as = cur_as;
}

void __am_switch(_Context *c) {
  if (vme_enable) {
    set_cr3(c->as->ptr);
    cur_as = c->as;
  }
}

int _map(_AddressSpace *as, void *va, void *pa, int prot) {
  /*PDE *updir = (PDE *)(as->ptr);
  PDE pde = updir[PDX(va)];
  if (!(pde & PTE_P)) {
    PTE *new_page = (PTE *)pgalloc_usr(1);
    pde = PTE_ADDR(new_page) | PTE_P;
    updir[PDX(va)] = pde;
  }
  PTE pte = ((PTE *)PTE_ADDR(pde))[PTX(va)];
  if (!(pte & PTE_P))
  {
    pte = PTE_ADDR(pa) | PTE_P;
    ((PTE *)PTE_ADDR(pde))[PTX(va)] = pte; 
  }

  return 0;*/
  // 获取页目录表基址
  PDE *updir = (PDE *) (as->ptr);
  intptr_t vaddr = (intptr_t) va;
  // 获取va对应页目录项
  PDE pde = updir[PDX(vaddr)];

  // 判断页目录项pde对应物理页是否可用
  if ((pde & PTE_P) == 0) {// 不可用
	// 申请新的物理页
    PTE *new = (PTE *)(pgalloc_usr(1));
	// 把该物理页赋给该页目录项
    pde = ((PDE)new & 0xfffff000) | PTE_P;
	// 更新页目录项
    updir[PDX(vaddr)] = pde;
  }

  // 获取页表基址
  PTE *upt = (PTE *)(((pde >> 12) & 0xfffff) << 12);
  // 获取页表项
  PTE pte = upt[PTX(vaddr)];
  // 判断页表项pte对应物理页是否可用
  if ((pte & PTE_P) == 0) {// 不可用
    // 使用物理页pa更新页表项
    upt[PTX(vaddr)] = ((PTE)pa & 0xfffff000) | PTE_P;
  }

  return 0;
}

_Context *_ucontext(_AddressSpace *as, _Area ustack, _Area kstack, void *entry, void *args) {
  /*typedef struct {
    int argc;
    char** argv;
    char** envp;
  } StackFrame;

  StackFrame *stackframe = (StackFrame*) (ustack.end - sizeof(StackFrame));
  _Context *tmp = (_Context*)(ustack.end - sizeof(StackFrame) - sizeof(_Context));

  stackframe->argc = 0;
  stackframe->argv = NULL;
  stackframe->envp = NULL;

	tmp->cs = 8;
	tmp->eip = (uintptr_t)(entry);
	tmp->esp = tmp->ebp = (uintptr_t)(ustack.end);
  tmp->as = as;
  
  return tmp;*/
  //return NULL;
  _Context *context = ustack.end - sizeof(_Context) - 0x20;
  memset(context, 0x00, sizeof(_Context) + 0x20);
  context->cs = 8;
  context->eip = (uint32_t)entry;
  //context->eflags->val = 0x02;
  context->as = as;
  //context->eflags->IF = 1;

  return context;
}
