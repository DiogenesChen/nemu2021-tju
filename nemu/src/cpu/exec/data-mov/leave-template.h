#include "cpu/exec/template-start.h"

#define instr leave

static void do_execute() {
    swaddr_t i;
    for (i = REG(R_ESP);i < REG (R_EBP); i+=DATA_BYTE) MEM_W (i,0);        //To empty the stack
    REG(R_ESP) = REG (R_EBP);
	REG(R_EBP) = MEM_R (REG (R_ESP));
	REG(R_ESP) +=DATA_BYTE;
	print_asm("leave");
}

make_instr_helper(r)

#include "cpu/exec/template-end.h"
