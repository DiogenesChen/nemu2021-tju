#include "cpu/exec/template-start.h"

#define instr jno

static void do_execute(){
	print_asm("jno %x",cpu.eip + 1 + DATA_BYTE + op_src->val);
	if(cpu.eflags.OF == 0) cpu.eip += op_src->val;
}

make_instr_helper(si)

#include "cpu/exec/template-end.h"
