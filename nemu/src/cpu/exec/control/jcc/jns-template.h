#include "cpu/exec/template-start.h"

#define instr jns

static void do_execute(){
	print_asm("jns %x",cpu.eip + 1 + DATA_BYTE + op_src->val);
	if(cpu.eflags.SF == 0) cpu.eip += op_src->val;
}

make_instr_helper(si)

#include "cpu/exec/template-end.h"
