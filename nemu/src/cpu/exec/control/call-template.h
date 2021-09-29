#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_i_, SUFFIX)) {
	int len = concat(decode_i_, SUFFIX)(eip + 1);
	reg_l(R_ESP) -= DATA_BYTE;
	MEM_W(reg_l(R_ESP), cpu.eip + len + 1);
	cpu.eip += (DATA_TYPE_S)op_src->val;
	print_asm("call: 0x%x", cpu.eip + len + 1);
	return len + 1;
}

make_helper(concat(call_rm_, SUFFIX)) {
	int len = concat(decode_rm_, SUFFIX)(eip + 1);
	reg_l(R_ESP) -= DATA_BYTE;
	MEM_W(reg_l(R_ESP), cpu.eip + len + 1);
	cpu.eip = (DATA_TYPE_S)op_src->val - len - 1;
	print_asm("call: %s", op_src->str);
	return len + 1;
}

#include "cpu/exec/template-end.h"