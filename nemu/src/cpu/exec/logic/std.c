#include "cpu/exec/helper.h"

make_helper(std) {
  cpu.eflags.DF = 1;
  print_asm("std");
  return 1;
}