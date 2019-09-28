#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  int Old_value, New_value;
  char tokens[65536];
  /* TODO: Add more members if necessary */


} WP;

  WP* new_wp();
  void free_wp(int NO);
  void set_watchpoint(char *expr);
  void show_watchpoint();
  bool check_watchpoint();
#endif
