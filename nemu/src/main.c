#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "monitor/expr.h"

int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
  FILE *fp = fopen("/home/liweikang/ics2019/nemu/tools/gen-expr/input","r");
  assert(fp != NULL);
  char str[65536];
  while (fgets(str,sizeof(str),fp)) {
    int res=atoi(strtok(str," ")); //printf("%d",res);
//	sscanf(res, "%d", &strtok(str," "));
//    char buf[65536]="";
    bool success = true;
//  printf(strtok(NULL," "));
//	printf("%d",expr(strtok(NULL," "),&success));
	char *e = strtok(NULL,"\n"); 
	if(res!=expr(e,&success)) {
		printf("Test fault\n");
		break;
	}
	memset(str,'\0',sizeof(str));
  }
	
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
