#include <stdio.h>
#include <assert.h>
#include "monitor/monitor.c"

int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
  FILE *fp = fopen("/home/liweikang/ics2019/nemu/tools/gen-expr/input","r");
  assert(fp != NULL);
  char str[65536];
  while (fgets(str,65536,fp)!=NULL)
    puts(str);
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
