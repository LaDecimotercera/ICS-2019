#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);
  //TODO();
  print_asm_template1(push);
}

make_EHelper(pop) {
  // TODO();
  rtl_pop(&id_dest->val);
  operand_write(id_dest, &id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&cpu.esp, &cpu.ebp);
  rtl_pop(&cpu.ebp); //UNDONE
  //TODO();

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {//Look into rtl.h pls.
    rtl_lr(&s0, R_AX, 2);
    rtl_msb(&s1, &s0, 2);
    if (s1) s1 = 0xffff;
    else s1 = 0;
    rtl_sr(R_DX, &s1, 2); 
    //TODO();
  } 
  else {
    rtl_lr(&s0, R_EAX, 4);
    rtl_msb(&s1, &s0, 4);
    if (s1) s1 = 0xffffffff;
    else s1 = 0;
    rtl_sr(R_EDX, &s1, 4);
    //TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    rtl_lr(&s0, R_AL, 1);
    rtl_sext(&s0, &s0, 1);
    rtl_sr(R_AX, &s0, 2);
    //TODO();
  }
  else {
    rtl_lr(&s0, R_AX, 2);
    rtl_sext(&s0, &s0, 2);
    rtl_sr(R_EAX, &s0, 4);
    //TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(movsb) { //still confused
  int incdec;
  rtlreg_t flag;
  rtl_get_DF(&flag);
  if (flag == 0) 
    incdec = 1;
  else incdec = -1;

  rtl_lr(&s0, R_ESI, 4);
  rtl_lm(&s1, &s0, 1);  
  rtl_lr(&t0, R_EDI, 4);
  rtl_sm(&t0, &s1, 1); //[destination-index] := [source-index];
//used to be rtl_sm(&t1, &s1, 4)
  rtl_addi(&s0, &s0, incdec);
  rtl_sr(R_EDI, &s0, 4);

  rtl_addi(&t0, &t0,incdec);
  rtl_sr(R_ESI, &t0, 4);

  print_asm_template2(movsb);
  /*int incdec = 1;
  switch (decinfo.opcode & 0xff) {
    case 0xa4:
      incdec = cpu.eflags.DF ? -1 : 1;
      rtl_lr(&s0, R_ESI, 4);
      rtl_lm(&s1, &s0, 1);
      s0 += incdec;
      rtl_sr(R_ESI, &s0, 4);
      rtl_lr(&s0, R_EDI, 4);
      rtl_sm(&s0, &s1, 1);
      s0 += incdec;
      rtl_sr(R_EDI, &s0, 4);
      print_asm("movsb")
      break;
    case 0xa5:
      if (decinfo.isa.is_operand_size_16) {
        incdec = cpu.eflags.DF ? -2 : 2;
        rtl_lr(&s0, R_ESI, 4);
        rtl_lm(&s1, &s0, 2);
        s0 += incdec;
        rtl_sr(R_ESI, &s0, 4);
        rtl_lr(&s0, R_EDI, 4);
        rtl_sm(&s0, &s1, 2);
        s0 += incdec;
        rtl_sr(R_EDI, &s0, 4);
        print_asm("movsw")
      } else {
        incdec = cpu.eflags.DF ? -4 : 4;
        rtl_lr(&s0, R_ESI, 4);
        rtl_lm(&s1, &s0, 4);
        s0 += incdec;
        rtl_sr(R_ESI, &s0, 4);
        rtl_lr(&s0, R_EDI, 4);
        rtl_sm(&s0, &s1, 4);
        s0 += incdec;
        rtl_sr(R_EDI, &s0, 4);
        print_asm("movsl")
      }
      break;
    default:
      panic("movs");*/

}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
