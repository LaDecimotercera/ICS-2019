#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
  uintptr_t new_brk = brk + increment;
  if (current->max_brk == 0) {
    current->max_brk = new_brk;
  } else {
    if (new_brk > current->max_brk) {
      uintptr_t pg_start = PGROUNDUP(current->max_brk);
      uintptr_t pg_end = PGROUNDUP(new_brk);
      for (; pg_start <= pg_end; pg_start += PGSIZE) {
        _map(&current->as, (void *)pg_start, new_page(1), 0);
      }
      current->max_brk = new_brk;
    }
  }

  return 0; 
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
