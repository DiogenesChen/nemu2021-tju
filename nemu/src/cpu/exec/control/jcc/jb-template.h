#include "cpu/exec/template-start.h"

#define instr jb

static void do_execute(){
	print_asm("jb %x",cpu.eip + 1 + DATA_BYTE + op_src->val);
	if(cpu.eflags.CF == 1) cpu.eip += op_src->val;
}

make_instr_helper(si)

#include "cpu/exec/template-end.h"
