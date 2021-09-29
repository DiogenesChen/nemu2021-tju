#include "cpu/exec/template-start.h"

#define instr js

make_helper(concat(js_i_, SUFFIX)) {
  int len = concat(decode_i_, SUFFIX)(eip + 1);
  print_asm("js 0x%x", (DATA_TYPE_S)op_src->imm + cpu.eip + len + 1);
  if (cpu.eflags.SF == 1)
    cpu.eip += (DATA_TYPE_S)op_src->imm;
  return len + 1;
}

#include "cpu/exec/template-end.h"
