#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
    int val;
    char args[32];
	struct watchpoint *next;

	/* TODO: Add more members if necessary */


} WP;

WP* new_wp();

void free_wp(WP* wp);

void delete_wp(int);

void info_wp();

bool check_wp();

#endif

