#include "cpu/exec/helper.h"

make_helper(ret){
	cpu.eip = swaddr_read(cpu.esp, 4) - 1;
	cpu.esp += 4;

	print_asm("ret");

	return 1;
}
