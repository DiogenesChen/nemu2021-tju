#include "cpu/exec/template-start.h"

make_helper(ret){
	cpu.eip = MEM_R(reg_l(R_ESP));
	reg_l(R_ESP) += DATA_BYTE;
	print_asm("ret");
    return 1;
}

#include "cpu/exec/template-end.h"
