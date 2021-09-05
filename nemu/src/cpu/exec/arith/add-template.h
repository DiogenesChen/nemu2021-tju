#include "cpu/exec/template-start.h"

#define instr add

static void do_execute() {
	DATA_TYPE result = op_src->val + op_dest->val;
    update_eflags_pf_zf_sf(result);
    int len = (DATA_BYTE << 3) - 1;
    int s1 = op_dest->val>>len;
    int s2 = op_src->val>>len;
    cpu.eflags.OF = (s1 == s2 && s2 != cpu.eflags.SF);
    cpu.eflags.CF = (result < op_dest->val);
    OPERAND_W(op_dest, result);
    print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)


#include "cpu/exec/template-end.h"
