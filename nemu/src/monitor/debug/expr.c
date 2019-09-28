#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_UNEQ = 0, TK_EQ = 1, TK_DEC = 10, TK_HEX = 16, TK_POINTER = 19, TK_REG = 128, 

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},				    // spaces
  {"\\+", '+'},						    // plus
  {"\\-", '-'},							// minus
  {"\\*", '*'},						    // multiply
  {"\\/", '/'},						    // divide
  {"\\(", '('},                         // left_parenthesis
  {"\\)", ')'},                         // right_parenthesis
  {"==", TK_EQ},						// equal
  {"!=", TK_UNEQ},					    // unequal
  {"[0-9]+", TK_DEC},		    		// decimal numbers
  {"0x[0-9a-f]+", TK_HEX},              // hexadecimal numbers
  {"\\$[a-ehilpx]{2,3}", TK_REG},       // registers
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
     }
  } 
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
       if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

         /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
		if (rules[i].token_type==TK_NOTYPE)
			continue;	
        switch (rules[i].token_type) {
			case '+':
			case '-':
			case '*':
			case '/':
			case TK_EQ:
			case TK_UNEQ:
			case TK_DEC:
			case TK_HEX:
			case TK_REG:
			case '(':
			case ')':{
				tokens[nr_token].type = rules[i].token_type;
			    memset(tokens[nr_token].str,'\0',sizeof(tokens[nr_token].str));
			    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    	    nr_token ++;} break;
			default: assert(0);
         }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("No match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
     } 
  } 

  return true;
}

bool check_parentheses(int p, int q) {
  int match = 0; 
  if (tokens[p].type!='('||tokens[q].type!=')')
	return false;
  for (;p < q; p ++) {
	if (tokens[p].type == '(')
		match ++;
	else if (tokens[p].type == ')')
		match --;
	if (match < 0)
		assert(0);
	else if (match == 0)
		return false;
	}
  if (match == 1)
	return true;
  else
	return false;	
}

uint32_t find_dominant_op(int p, int q) {	
  int match = 0;
  int op = p;
  for (int i = p; i<q ; i ++) {
	if (tokens[i].type=='(')
		match ++;
	else if (tokens[i].type == ')')
		match --;
	else if (match == 0) {
		if (tokens[i].type == '+'||tokens[i].type == '-')
			op = i;
		else if (tokens[i].type == '*'||tokens[i].type == '/') {
			if(tokens[op].type != '+'&&tokens[op].type != '-')
				op=i;
		}
	}
  }
  return op;
}

extern uint32_t isa_reg_str2val(const char *s, bool *success);

uint32_t eval(int p, int q) {
  int op, val1, val2;  
  if (p > q)
	return false;
  else if (p == q) {
	int res = 0;
	if (tokens[p].type == TK_DEC) 
		sscanf(tokens[p].str,"%d",&res);
//		return res;
	else if (tokens[p].type == TK_HEX)
		sscanf(tokens[p].str,"%x",&res);
//      return res;
	else if (tokens[p].type == TK_REG) {
		bool success = true;
		for (int i = 0; i < 4; i ++)
			tokens[p].str[i] = tokens[p].str[i+1];  
		res = isa_reg_str2val(tokens[p].str, &success);
	}	
    return res;
  }
  else if (check_parentheses(p,q) == true) {			
	  return eval(p + 1, q - 1);
  }
  else {
	op = find_dominant_op(p,q);
	val1 = eval(p, op - 1);
    val2 = eval(op + 1, q); 

	switch (tokens[op].type) { 
		case '+': return val1 + val2;
		case '-': {if (op == p)
					  return -val2;
				   else return val1 - val2;}
		case '*': {if (op == p)
					  return vaddr_read(val2,4);
				   else return val1 * val2;}
		case '/': return val1 / val2;
		case TK_EQ: return val1 == val2;
		case TK_UNEQ: return val1 != val2;
		default: assert(0);
	}
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  } 

  /* TODO: Insert codes to evaluate the expression. */
  int p = 0;
  int q = nr_token - 1;
  for (int i = 0; i < nr_token; i ++) {
	 if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == '+' || tokens[i - 1].type == '-'\
				 || tokens[i - 1].type == '*' || tokens[i - 1].type == '/'))
		tokens[i].type = TK_POINTER ;
  }
  return eval(p,q);
}
