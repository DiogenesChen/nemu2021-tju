#include "cpu/exec/template-start.h"

#define instr je

static void do_execute() {
	print_asm("je %x",cpu.eip + DATA_BYTE + op_src->val + 1);
	if (cpu.eflags.ZF == 1)cpu.eip += op_src->val;
}

make_instr_helper(i)


#include "cpu/exec/template-end.h"
