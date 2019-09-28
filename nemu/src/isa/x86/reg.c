#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) { 
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display(void) {
  for (int idx = 0; idx < 8; idx ++)
	printf("%s:\t0x%08x\n", regsl[idx],cpu.gpr[idx]._32);
  for (int idx = 0; idx < 8; idx ++)
	printf("%s:\t0x%08x\n", regsw[idx],cpu.gpr[idx]._16);
  for (int idx = 0; idx < 8; idx ++)
	for (int idx_2 = 0; idx_2 < 2; idx_2 ++)
	  printf("%s:\t0x%08x\n", regsb[idx],cpu.gpr[idx]._8[idx_2]);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  int num;
  if (strcmp(s,"pc")==0) {
	num = cpu.pc;
	return num;
  }
  else {
	for (int j = 0; j < 8; j ++) {
	  if (strcmp(s,regsl[j]) == 0) {
		num = cpu.gpr[j]._32;
		return num;
	  }
    }	  
  }
  *success = false; return 0;
}




