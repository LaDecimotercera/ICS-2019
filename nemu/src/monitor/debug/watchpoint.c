#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
int point_cnt = 0; 

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  } 
  wp_pool[NR_WP - 1].next = NULL;
  head = NULL;
  free_ = wp_pool;
}

WP* new_wp() {
  if (!point_cnt) 
	  init_wp_pool();
  WP *p = free_;
  if (p) {
	free_ = free_->next;
	p->next = head;
	head = p;
	point_cnt += 1;
	return p;
  }
  else assert(0);
}

void free_wp(int NO) {
  WP *p = head;
  WP *q = p;
  if (head == NULL) {
	//printf("No watchpoint found!\n"); return;
  }
  if (p->NO == NO) {
	head = head->next;
	p->next = free_;
	free_ = p;
	printf("Deleted watchpoint %d\n", p->NO);
  }
  else {
	while (p->NO!=NO && p!=NULL) {
		q = p; 
		p = p->next;
	}
	if (p->NO == NO) {
		q->next = p->next;
		p->next = free_;
		free_ = p;
	    point_cnt -= 1;
		printf("Deleted watchpoint %d\n", p->NO);
	}
	//else if (p == NULL) printf("No watchpoint found!\n");
  }
}
	
/* TODO: Implement the functionality of watchpoint */

void set_watchpoint(char *e) {
  WP *p; p = new_wp();
  printf("Set watchpoint %d\n", p->NO);
  bool success = true;
  strcpy(p->tokens, e);
//printf("Expr:%s\n", p->tokens);
  p->Old_value = expr(p->tokens, &success);
//if (success) printf("Old value: %x\n", p->Old_value);
//else printf("Failed\n");
}

bool check_watchpoint() {
  WP *q = head;
  bool success_ = true;
  if (!q) {
	//printf("No watchpoint found!\n");
	return false;
  }
  while (q) { 
    q->New_value = expr(q->tokens, &success_);
    if (success_ && q->New_value!=q->Old_value) {
      printf("Hit watchpoint %d\n", q->NO);
      q->Old_value = q->New_value;
	  return true; 
    }
    else if (!success_) {
	  printf("New value eval failure in watchpoint %d\n", q->NO);
	}
	q = q->next;
  }
  return false;
}

void show_watchpoint() {
  WP *p = head;
  //if (!p) printf("No watchpoint found!\n");  
  while(p) {
	printf("%d %x\n", p->NO, p->Old_value);
	p=p->next;
  }
}
