#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_i_, SUFFIX)){
    int len = concat(decode_i_, SUFFIX) (eip + 1);
	reg_l(R_ESP) -= DATA_BYTE;      
	swaddr_write(reg_l(R_ESP), 4, cpu.eip + (len + 1));
	print_asm("call 0x%x", cpu.eip + 1 + len + op_src->val);
	cpu.eip += op_src->val;
	return len + 1;
}

make_helper (concat(call_rm_, SUFFIX)){
	int len = decode_rm_l(eip + 1);
	swaddr_t ret_addr = cpu.eip + len + 1;
	swaddr_write(cpu.esp - 4, 4, ret_addr);
	cpu.esp -= 4 ;
	cpu.eip = op_src->val - len - 1;
	print_asm("call *%s",op_src->str);

	return len + 1;	
}

#include "cpu/exec/template-end.h"

