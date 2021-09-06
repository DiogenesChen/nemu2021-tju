#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "jng-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "jng-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "jng-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(jng_si)
