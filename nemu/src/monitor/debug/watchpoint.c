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
		if(NR_WP-1-i) wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

WP* new_wp(){
    WP *n, *p;
    n = free_;
    p = head;
    free_ = free_ -> next;
    n -> next = NULL;
    if(p == NULL) { head = n; p = head;}
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
    }
    h = head;
    if( head == NULL ) assert(0);
    if ( head -> NO == wp -> NO ) head = head -> next;
    else{
        while (h -> next != NULL && h -> next -> NO != wp -> NO) h = h -> next;
        if(h -> next == NULL && h -> NO == wp -> NO) printf("GHOST!!!");
        else if (h -> next -> NO == wp -> NO) h -> next = h -> next -> next;
    }
    wp -> next = NULL;
}

bool check_wp(){
    WP* wp;
    wp = head;
    bool suc, key;
    key = true;
    while(wp != NULL){
        int val = expr(wp -> args, &suc);
        if(!suc) assert(1);
        if(wp -> val != val){
            key = false;
            printf ("Hint breakpoint %d at address 0x%08x\n", wp -> NO, cpu.eip);
            printf ("Watchpoint %d: %s\n",wp -> NO,wp -> args);
            printf ("Old value = %d\n",wp -> val);
            printf ("New value = %d\n",val);
            wp -> val = val;
        }
        wp = wp ->next;
    }
    
    return key;
}

void delete_wp(int num){
    WP *p = head;
	while(p -> next && p -> NO != num){
		p = p -> next;
	}
	if( p -> NO == num )
    	free_wp (p);
	else
		printf("unexpected number, delete failed\n");
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



