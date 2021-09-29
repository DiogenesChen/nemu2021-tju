#include "cpu/exec/template-start.h"

#define instr jge

static void do_execute() {
  DATA_TYPE_S displacement = op_src->val;
  print_asm("jge: %x", cpu.eip + 1 + DATA_BYTE + displacement);
  if (cpu.eflags.ZF == 1 || cpu.eflags.SF == cpu.eflags.OF)
    cpu.eip += displacement;
}
make_instr_helper(i)

#include "cpu/exec/template-end.h"
