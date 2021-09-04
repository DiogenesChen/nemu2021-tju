#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_, SUFFIX)){
	cpu.eip = MEM_R(reg_l(R_ESP)) - 1;
	//cpu.esp += DATA_BYTE;
	print_asm("ret");
	return 1;
}

#include "cpu/exec/template-end.h"
