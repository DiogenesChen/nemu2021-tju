#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_i_, SUFFIX)){
    int len = concat(decode_i_, SUFFIX) (eip + 1);
	cpu.esp -= DATA_BYTE;      
	swaddr_write(cpu.esp, 4, cpu.eip + (len + 1));
    cpu.eip += op_src->val;
	print_asm("call %x", cpu.eip + 1 + len + op_src->val);
	return len + 1;
}

#include "cpu/exec/template-end.h"

