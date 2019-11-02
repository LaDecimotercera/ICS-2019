#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  rtl_and(&t1, &id_dest->val, &id_src->val);

  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&t1, id_dest->width);  
  //TODO();

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
	
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&t1, id_dest->width);
  //TODO();

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);

  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&t1, id_dest->width);
  // TODO();
 
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);

  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&t1, id_dest->width);  
  //TODO();

  print_asm_template2(or);
}

make_EHelper(sar) {
  rtl_sar(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  rtl_update_ZFSF(&t1, id_dest->width);
  //TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  rtl_update_ZFSF(&t1, id_dest->width);
  //TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  rtl_update_ZFSF(&t1, id_dest->width);
  //TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  rtl_not(&t1, &id_dest->val);
  operand_write(id_dest, &t1);
  //TODO();

  print_asm_template1(not);
}

make_EHelper(rol) {
  rtl_li(&t0, id_src->val);
  while(t0){
    rtl_shri(&t1, &id_dest->val, id_dest->width * 8 -1);
    //rtl_li(&s0, 2*id_dest->val);
    rtl_addi(&t1, &t1, 2*id_dest->val);
    operand_write(id_dest, &t1);
    t0--;  
  }
  if (id_src->val == 1){
    rtl_shri(&t1, &id_dest->val, id_dest->width * 8 -1);
    rtl_get_CF(&t0);
    if (t0 != t1){
      rtl_li(&s0, 1);
      rtl_set_OF(&s0);
    }
    else
    {
      rtl_li(&s0, 0);
      rtl_set_OF(&s0);
    }
  }
}
