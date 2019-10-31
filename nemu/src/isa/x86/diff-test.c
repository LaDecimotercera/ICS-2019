#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc){
  if(ref_r->eax!=cpu.eax || ref_r->ecx!=cpu.ecx || ref_r->edx!=cpu.edx || 
  ref_r->ebx!=cpu.ebx || ref_r->esp!=cpu.esp || ref_r->ebp!=cpu.ebp || 
  ref_r->esi!=cpu.esi || ref_r->edi!=cpu.edi || ref_r->pc!=cpu.pc) {
    printf("qemus eax:0x%08x, cpu eax:0x%08x @pc:0x%08x\n", ref_r->eax, cpu.eax, pc);
    printf("qemus ecx:0x%08x, cpu ecx:0x%08x @pc:0x%08x\n", ref_r->ecx, cpu.ecx, pc);
    printf("qemus edx:0x%08x, cpu edx:0x%08x @pc:0x%08x\n", ref_r->edx, cpu.edx, pc);
    printf("qemus ebx:0x%08x, cpu ebx:0x%08x @pc:0x%08x\n", ref_r->ebx, cpu.ebx, pc);
    printf("qemus esp:0x%08x, cpu esp:0x%08x @pc:0x%08x\n", ref_r->esp, cpu.esp, pc);
    printf("qemus ebp:0x%08x, cpu ebp:0x%08x @pc:0x%08x\n", ref_r->ebp, cpu.ebp, pc);
    printf("qemus esi:0x%08x, cpu esi:0x%08x @pc:0x%08x\n", ref_r->esi, cpu.esi, pc);
    printf("qemus edi:0x%08x, cpu edi:0x%08x @pc:0x%08x\n", ref_r->edi, cpu.edi, pc);
    printf("qemus pc:0x%08x, cpu pc:0x%08x @pc:0x%08x\n", ref_r->pc, cpu.pc, pc);
    return false;
  }
  return true;
}

void isa_difftest_attach(void) {
}
