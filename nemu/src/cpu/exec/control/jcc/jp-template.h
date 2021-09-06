#include "cpu/exec/template-start.h"

#define instr jp

static void do_execute(){
	print_asm("jp %x",cpu.eip + 1 + DATA_BYTE + op_src->val);
	if(cpu.eflags.PF == 1) cpu.eip += op_src->val;
}

make_instr_helper(si)

#include "cpu/exec/template-end.h"
