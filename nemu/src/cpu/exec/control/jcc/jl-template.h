#include "cpu/exec/template-start.h"

#define instr jl

static void do_execute(){
	print_asm("jl %x",cpu.eip + 1 + DATA_BYTE + op_src->val);
	if(cpu.eflags.SF != cpu.eflags.OF) cpu.eip += op_src->val;
}

make_instr_helper(si)

#include "cpu/exec/template-end.h"
