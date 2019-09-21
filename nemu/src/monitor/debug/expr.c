#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ = 1, TK_UNEQ = 0, TK_TEN = 10, TK_SIXTEEN = 16, 

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
  {"==", TK_EQ},						// equal
  {"!=", TK_UNEQ},					    // unequal
  {"[0-9]+", TK_TEN},		    		// decimal number
  {"\\(", '('},                         // left_parenthesis
  {"\\)", ')'},							// right_parenthesis
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

static Token tokens[32] __attribute__((used)) = {};
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
			case '+':{
				tokens[nr_token].type = rules[i].token_type;
			    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    	    nr_token ++;
		    }break;  
			case '-':{   
				tokens[nr_token].type = rules[i].token_type;
			    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    	    nr_token ++;
			}break;
			case '*':{   
				tokens[nr_token].type = rules[i].token_type;
			    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    	    nr_token ++;
			}break;
			case '/':{   
				tokens[nr_token].type = rules[i].token_type;
			    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    	    nr_token ++;
			}break;
			case TK_EQ:{   
				tokens[nr_token].type = rules[i].token_type;
			    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    	    nr_token ++;
			}break;
			case TK_UNEQ:{  
				tokens[nr_token].type = rules[i].token_type;
			    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    	    nr_token ++;
			}break;
			case TK_TEN:{  
				tokens[nr_token].type = rules[i].token_type;
			    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    	    nr_token ++;
			}break;
			case TK_SIXTEEN:{  
				tokens[nr_token].type = rules[i].token_type;
			    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    	    nr_token ++;
			}break;
			case '(':{  
				tokens[nr_token].type = rules[i].token_type;
			    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    	    nr_token ++;
			}break;
			case ')':{   
				tokens[nr_token].type = rules[i].token_type;
			    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    	    nr_token ++;
			}break;
			default: assert(0);
         }
        break;
      }
    } 

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
     } 
  } 

  return true;
}

bool check_parentheses(int p, int q) {
  int match = 0; 
  if (tokens[p].type!='('||tokens[p].type!=')')
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

uint32_t eval(int p, int q) {
  int op, val1, val2;  
  if (p > q)
	return false;
  else if (p == q) {
	int res = 0;
	if (tokens[p].type == TK_TEN) {
		sscanf(tokens[p].str,"%d",&res);
		return res;
	}
	else return false;
  }
  else if (check_parentheses(p,q) == true) {			
	  printf("Yes!\n");
	  return eval(p + 1, q - 1);
  }
  else {
	op = find_dominant_op(p,q);	
	val1 = eval(p, op - 1);
	val2 = eval(op + 1, q);

	switch (tokens[op].type) {
		case '+': return val1 + val2;
		case '-': return val1 - val2;
		case '*': return val1 * val2;
		case '/': return val1 / val2;
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
  return eval(p,q);
  return 0;
}
