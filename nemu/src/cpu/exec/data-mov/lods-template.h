#include "cpu/exec/template-start.h"

#define instr lods

make_helper(concat(lods_, SUFFIX)) {
  REG(R_EAX) = swaddr_read(reg_l(R_ESI), DATA_BYTE);
  if (cpu.eflags.DF == 0)
    reg_l(R_ESI) += DATA_BYTE;
  else
    reg_l(R_ESI) -= DATA_BYTE;

  print_asm("loads%s", str(SUFFIX));
  return 1;
}

#include "cpu/exec/template-end.h"
