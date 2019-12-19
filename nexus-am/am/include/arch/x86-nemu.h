#ifndef __ARCH_H__
#define __ARCH_H__

struct _Context {
  //uintptr_t esi, ebx, eax, eip, edx, eflags, ecx, cs, esp, edi, ebp;
  struct _AddressSpace *as;
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int irq;
  uintptr_t eip, cs;
  union {
	 struct {
			 uint32_t CF: 1;
			 unsigned _0: 5;
			 uint32_t ZF: 1;
			 uint32_t SF: 1;
			 unsigned _1: 1;
			 uint32_t IF: 1;
       uint32_t DF: 1;
			 uint32_t OF: 1;
			 unsigned _2: 20;
	};
	uint32_t val; 
  }	eflags;
};

#define GPR1 eax
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax

#endif
