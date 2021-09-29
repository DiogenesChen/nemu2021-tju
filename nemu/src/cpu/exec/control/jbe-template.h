#include "cpu/exec/template-start.h"

#define instr jbe

static void do_execute() {
	DATA_TYPE_S displacement = op_src->val;
	if (cpu.eflags.CF == 1 || cpu.eflags.ZF == 1) cpu.eip += displacement;
  print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
