#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
int fg_pcb = 0;

extern void naive_uload(PCB *pcb, const char *filename);
extern void context_kload(PCB *pcb, void *entry);
extern void context_uload(PCB *pcb, const char *filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World fsrom Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  //switch_boot_pcb();

  Log("Initializing processes...");
  //context_uload(&pcb[0], "/bin/pal");
  
  // load program here
  //naive_uload(NULL, "/bin/dummy");
  context_uload(&pcb[0], "/bin/hello");
  //switch_boot_pcb();
  context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[2], "/bin/pal");
  context_uload(&pcb[3], "/bin/pal");
  switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
  //return NULL;
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  current = (current == &pcb[0] ? &pcb[fg_pcb] : &pcb[0]);
  //current = &pcb[0];
  // then return the new context
  return current->cp;
}
