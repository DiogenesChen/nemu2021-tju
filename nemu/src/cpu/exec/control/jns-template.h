#include "cpu/exec/template-start.h"

#define instr jns

make_helper(concat(jns_i_, SUFFIX)) {
  int len = concat(decode_i_, SUFFIX)(eip + 1);
  print_asm("jns: 0x%x", (DATA_TYPE_S)op_src->imm + cpu.eip + len + 1);
  if (cpu.eflags.SF == 0)
    cpu.eip += (DATA_TYPE_S)op_src->imm;
  return len + 1;
}

#include "cpu/exec/template-end.h"
