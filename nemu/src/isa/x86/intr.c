#include "rtl/rtl.h"

#define IRQ_TIMER 32          // for x86

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push((rtlreg_t *)&cpu.eflags.val);
  rtl_push((rtlreg_t *)&cpu.cs);
  rtl_push((rtlreg_t *)&ret_addr);
  cpu.eflags.IF = 0;
  uint32_t tmp_15_0 = vaddr_read(cpu.idtr.base + 8 * NO, 4);
  uint32_t tmp_31_16 = vaddr_read(cpu.idtr.base + 8 * NO + 4, 4);
  rtl_j((tmp_15_0 & 0xffffu) | (tmp_31_16 & 0xffff0000u));  
}

bool isa_query_intr(void) {
  if (cpu.INTR & cpu.eflags.IF) {
    cpu.INTR = false;
    raise_intr(IRQ_TIMER, cpu.pc);
    return true;
  }
  return false;
}