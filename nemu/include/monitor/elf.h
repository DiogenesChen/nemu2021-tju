#ifndef __ELF_H__
#define __ELF_H__

#include "common.h"

typedef struct {
    swaddr_t prev_ebp;
    swaddr_t ret_addr;
    int args[4];
    char str[32];
} PartOfStackFrame;

int getVariable(char*, bool*);

swaddr_t getFrame(swaddr_t, bool*);

swaddr_t read_ebp (swaddr_t);

#endif