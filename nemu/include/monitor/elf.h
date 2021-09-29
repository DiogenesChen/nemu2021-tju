#ifndef __ELF_H__
#define __ELF_H__

#include "common.h"

void getFrame(swaddr_t, char*);
uint32_t getVariable(char*, bool*);
void getTable();

#endif
