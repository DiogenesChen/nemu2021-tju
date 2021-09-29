#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  uint32_t val;
  uint32_t eval;
  bool check_eval;
  char expr[32];

} WP;

WP* new_wp();
void free_wp(WP *);
bool check_wp();
void delete_wp(int );
void info_wp();

#endif
