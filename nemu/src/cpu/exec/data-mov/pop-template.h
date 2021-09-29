#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
  current_sreg = R_SS;
  OPERAND_W(op_src, MEM_R(REG(R_ESP)));
  MEM_W(REG(R_ESP), 0);
  REG(R_ESP) += DATA_BYTE;
  print_asm_template1();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
make_instr_helper(rm)
#endif

#include "cpu/exec/template-end.h"
