#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "cmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cmp-template.h"
#undef DATA_BYTE

make_helper_v(cmp_si2rm)
