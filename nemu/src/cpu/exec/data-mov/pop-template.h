#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    OPERAND_W(op_src, MEM_R(cpu.esp));
	reg_l(R_ESP) += DATA_BYTE;
	print_asm_template1();
}

make_instr_helper(r)

#include "cpu/exec/template-end.h"
