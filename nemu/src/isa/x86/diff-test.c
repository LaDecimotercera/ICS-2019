#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if(ref_r->eax != pc.eax || ref_r->ecx != pc.ecx || ref_r->edx != pc.edx ||
		 ref_r->ebx != pc.ebx || ref_r->esp != pc.esp || ref_r->ebp != pc.ebp ||
		 ref_r->esi != pc.esi || ref_r->edi != pc.edi || ref_r->eip != pc.eip)
	//if(cpu.eip != 0x100005)
    return false;
  return true;
}

void isa_difftest_attach(void) {
}
