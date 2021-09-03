#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), DATA_BYTE, op_src->val);
	print_asm_template1();
}

make_instr_helper(r)

#include "cpu/exec/template-end.h"
