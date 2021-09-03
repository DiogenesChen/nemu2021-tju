#include "cpu/exec/template-start.h"

#define instr je

static void do_execute(){
	DATA_TYPE_S val = op_src->val;
	print_asm("je %x",cpu.eip + 1 + DATA_BYTE + op_src->val);
	if(cpu.eflags.ZF == 1)
	cpu.eip += val;
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
