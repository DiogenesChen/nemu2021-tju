#include "cpu/exec/template-start.h"

#define instr lods

static void do_execute(){
	reg_l(R_EAX) = MEM_R(reg_l(R_ESI));
	cpu.esi += (cpu.eflags.DF ? -DATA_BYTE : DATA_BYTE);
	print_asm_template1();
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
