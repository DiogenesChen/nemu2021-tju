#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "nemu.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP * new_wp() {
  WP * f, * p;
  f = free_;
  free_ = free_ -> next;
  f -> next = NULL;
  p = head;
  if (p == NULL)
    p = head = f;
  else {
    while (p -> next != NULL)
      p = p -> next;
    p -> next = f;
  }
  f -> check_eval = false;
  return f;
}

void free_wp(WP * wp) {
  WP * p;
  p = head;
  if (head == NULL)
    Assert(0, "ERROR");
  else if (p -> NO == wp -> NO) {
    head = head -> next;
    p -> next = free_;
    p -> val = 0;
    p -> expr[0] = '\0';
    free_ = p;
    return;
  } else {
    WP * ls = head;
    p = p -> next;
    while (p != NULL) {
      if (p -> NO == wp -> NO) {
        ls -> next = p -> next;
        p -> val = 0;
        p -> next = free_;
        p -> expr[0] = '\0';
        free_ = p;
        return;
      } else
        p = p -> next, ls = ls -> next;
    }
  }
}

void delete_wp(int num) {
  WP * f;
  f = &wp_pool[num];
  free_wp(f);
}

void info_wp() {
  WP * f;
  f = head;
  if (f == NULL) {
    puts("No watchpoints currently set.");
    return;
  }
  while (f != NULL) {
    printf("%d: %s = %d\n", f -> NO, f -> expr, f -> val);
    f = f -> next;
  }
}

bool check_wp() {
  WP * f;
  f = head;
  bool suc, flag = 1;
  while (f != NULL) {
    uint32_t ls = expr(f -> expr, &suc);
    if (!suc)
      Assert(1, "REEOR\n");
    if (f -> check_eval) {
      if (ls == f -> eval) {
        printf("Hint watchpoint %d at 0x%x:\n", f -> NO, cpu.eip);
        printf("value: %d (0x%x)\n", ls, ls);
        flag = 0;
      }
    } else if (ls != f -> val) {
      printf("Hint watchpoint %d at 0x%x:\n", f -> NO, cpu.eip);
      printf("old value: %d (0x%x)\n", f -> val, f -> val);
      printf("new value: %d (0x%x)\n", ls, ls);
      f -> val = ls;
      flag = 0;
    }
    f = f -> next;
  }
  return flag;
}
