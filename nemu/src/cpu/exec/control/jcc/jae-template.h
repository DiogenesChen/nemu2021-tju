#include "cpu/exec/template-start.h"

#define instr jae

static void do_execute(){
	print_asm("jae %x",cpu.eip + 1 + DATA_BYTE + op_src->val);
	if(cpu.eflags.CF == 1) cpu.eip += op_src->val;
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
