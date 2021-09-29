#include "cpu/exec/template-start.h"

#define instr ja

static void do_execute() {
  DATA_TYPE_S displacement = op_src->val;
  print_asm("ja: 0x%x", cpu.eip + 1 + DATA_BYTE + displacement);
  if (!cpu.eflags.ZF && !cpu.eflags.CF)
    cpu.eip += displacement;
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
