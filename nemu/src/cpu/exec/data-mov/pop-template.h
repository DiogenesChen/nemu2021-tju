#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    reg_l(op_src->reg) = MEM_R(cpu.esp);
	reg_l(R_ESP) += DATA_BYTE;
	print_asm_template1();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
make_instr_helper(rm)
#endif

#include "cpu/exec/template-end.h"
