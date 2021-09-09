#ifndef __ELF_H__
#define __ELF_H__

#include "common.h"

typedef struct {
    swaddr_t prev_ebp;
    swaddr_t ret_addr;
    uint32_t args[4];
} PartOfStackFrame;

void getFrame();

int getVariable(char*, bool*); 


#endif