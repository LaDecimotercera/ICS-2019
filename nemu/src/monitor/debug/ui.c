#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

extern void isa_reg_display(void);

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

   if (line_read) {
    free(line_read);
    line_read = NULL;
   }

  line_read = readline("(nemu) ");

     if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si_N(char *args) {//single-step execution of N operations
  int steps = 1;
  char *arg = strtok(NULL, " ");
  if (arg) steps = atoi(arg);
  if (steps<=0) cpu_exec(-1);
  else {
	// for (int step = 0; step < steps; step ++)
	  cpu_exec(steps);
  }
  return 0;
}

static int cmd_info (char *args) {
  char *arg = strtok(NULL, " ");
  if (strcmp(arg,"r")==0) { isa_reg_display();} 
//  else if (arg == 'w'）{ 
//  }	  
  else { printf("Unknown command '%s'\n", args);}//mighty problem
  return 0;
} 

static int cmd_p_EXPR(char *args) {
	char *arg = strtok(NULL," ");
	bool success = true;
	int res = expr(arg,&success);
	if (success)
		printf("%#x\n",res);
	else
		printf("Fail to eval\n");   
	return 0;
}

static int cmd_x_N_EXPR(char *args) {// PA1.1: simplified version
  vaddr_t start_addr; int len;
  char *arg = strtok(NULL, " ");
  sscanf(arg,"%d",&len);
  char *arg_ = strtok(NULL, " ");
  sscanf(arg_,"%x",&start_addr);
  printf("%#x:\t",start_addr);
  for (int i = 0; i <  len; i ++) {
	printf("%#x\t",vaddr_read(start_addr,4));
	start_addr += 4;
   }
  printf("\n");
  return 0;
}	

static int cmd_help(char *args);   

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Single-step execution of N commands", cmd_si_N },
  { "info", "Print program status by option: r(registers); w(watchpoints)", cmd_info },
  { "x", "Scan memory", cmd_x_N_EXPR },
  { "p", "Find the value of the expression EXPR", cmd_p_EXPR },

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))


static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
      }
   } 
  else {
     for (i = 0; i < NR_CMD; i ++) {
       if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  } 
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if ( is_batch_mode) {
    cmd_c(NULL);
    return;
  } 

  for (char *str; (str = rl_gets() ) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    } 

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      } 
     } 

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  } 
}
