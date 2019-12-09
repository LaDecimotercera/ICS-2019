#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

extern void naive_uload(PCB *pcb, const char *filename);
extern void context_kload(PCB *pcb, void *entry);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  /*switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  naive_uload(NULL, "/bin/init");*/
  context_kload(&pcb[0], (void *)hello_fun);
  switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
  //return NULL;
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  current = &pcb[0];

  // then return the new context
  return current->cp;
}
