#include "cpu/exec/template-start.h"

#define instr jnle

static void do_execute(){
	print_asm("jnle %x",cpu.eip + 1 + DATA_BYTE + op_src->val);
	if(cpu.eflags.ZF == 0 && cpu.eflags.SF == cpu.eflags.OF) cpu.eip += op_src->val;
}

make_instr_helper(si)

#include "cpu/exec/template-end.h"
