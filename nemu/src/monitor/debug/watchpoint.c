#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
        wp_pool[i].val = 0;
        wp_pool[i].args = NULL;
		if(NR_WP-1-i) wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = wp_pool;
	free_ = wp_pool;
}

WP* new_wp(){
    WP *n, *p;
    n = free_;
    p = head;
    free_ = free_ -> next;
    n -> next = NULL;
    if(p == NULL) { p = n; head = n; }
    else{
        while (p -> next) p = p ->next;
        p -> next = n;
    }
    return n;
}

void free_wp(WP* wp){
    WP *h, *p;
    p = free_;
    if(p == NULL) { p = wp; free_ = wp;}
    else{
        while (p -> next) p = p -> next;
        p -> next = wp;
        wp -> next = NULL;
    }
    h = head;
    if(head == NULL) assert(0);
    else{
        while (h -> next && h -> next -> NO != wp -> NO) h = h -> next;
        if(h -> next == NULL && h -> NO == wp -> NO) printf("GHOST!!!");
        else if (h -> next -> NO == wo -> NO) f -> next = f -> next -> next;
    }
    wp -> next = NULL;
    wp -> val = 0;
    wp -> args = NULL;
}

bool checkWP(){
    WP* wp;
    wp = head;
    bool suc, key;
    key = true;
    while(wp != NULL){
        int val = expr(wp -> args, suc);
        if(!suc) assert(1);
        if(wp -> val != val){
            key = false;
            printf ("Hit breakpoint %d at 0x%08x\n",f->b,cpu.eip);
            f = f->next;
            continue;
            printf ("Watchpoint %d: %s\n",f -> NO,f -> args);
            printf ("Old value = %d\n",f -> val);
            printf ("New value = %d\n",val);
            f->val = val;
        }
        f = f->next;
    }
    
    return key;
}

void delete_wp(int num){
    WP *p;
    p = &wp_pool[num];
    free_wp (p);
}

void info_wp(){
    WP *p;
    p = head;
    while (p != NULL){
        printf ("Watchpoint %d: %s = %d\n",p -> NO, p -> args, p -> val);
        p = p -> next;
    }
}


/* TODO: Implement the functionality of watchpoint */



