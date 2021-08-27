#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256,PLUS, MINUS,MULT,DIV,EQ,NOTEQ, AND, OR, NOT, LB, RB,REGSNAME,NUM,HEXNUM,NEG,POINTER

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
	int priority;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE, 0},				// spaces
	{"\\+", PLUS,4},					// plus	
	{"\\-", MINUS,4},						//minus
	{"\\*",MULT,5},					//multiply
	{"\\/",DIV,5},					//divid
	{"==", EQ,3},					//equal
	{"!=",NOTEQ,3},					//not equal
	{"&&",AND,2},					//logical and
	{"\\|\\|",OR, 1},					//logical or
	{"!",NOT,6},					//logical not
	{"\\(",LB,7},					//left parenthesis
	{"\\)",RB,7},					//right parenthesis
	{"0[xX][0-9,a-f,A-F]+", HEXNUM,0},		//hexadecimal
	{"[0-9]+", NUM, 0},				//decimal
	{"$[a-z]{2,3}", REGSNAME,0}			//registers name
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int priority;
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE: break;
					case HEXNUM :
					case NUM :
					case REGSNAME :{
					strncpy(tokens[nr_token].str, substr_start - substr_len, substr_len);
					tokens[nr_token].str[substr_len] = '\0';
					default:
			//solve negative
					if(rules[i].token_type == MINUS){
					if(nr_token == 0)
						tokens[nr_token ++].type = NEG;
					else if(PLUS <= tokens[nr_token - 1].type && tokens[nr_token - 1].type <= LB)
						tokens[nr_token ++].type = NEG;
					else{
						tokens[nr_token ++].type = MINUS;
						tokens[nr_token ++].priority = 4;
						}
					}
					//solve pointer
					else if(rules[i].token_type == MULT){
					if(nr_token == 0)
						tokens[nr_token ++].type = POINTER;
					else if(PLUS <= tokens[nr_token - 1].type && tokens[nr_token - 1].type <= LB)
						tokens[nr_token ++].type = POINTER;
					else{
						tokens[nr_token++].type = MULT;
						tokens[nr_token++].type = 5;
						}
					}
					//normal
					else{ 
					tokens[nr_token].priority = rules[i].priority;
					tokens[nr_token ++].type = rules[i].token_type;
					}
				// panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	 
	}
	return true;
}

bool check_parentheses(int l, int r, bool *success){
	*success = true;
	if(l > r) return *success = false;
	int cnt = 0; 
	bool flag = 1;
	int i;
	for(i = l; i <= r; i ++){
		if(tokens[i].type == LB)
			cnt ++;
		if(tokens[i].type == RB)
			cnt --;
		if(cnt < 0) return *success = false;
		if(cnt == 0 && i != r){
			flag = 0;
		}
		}
		if(cnt != 0) return *success = false;
		return flag;	
}
uint32_t  make_dop(int l, int r){
	int i;
	int j;
	int dop = l;
	int min_pri = 10;
	for(i = l; i <= l; i ++){
	 if(tokens[i].type == NUM || tokens[i].type == HEXNUM || tokens[i].type == REGSNAME)
		continue;
	int cnt = 0;
	bool key = true;
	for(j = i - 1; j >= l; j --){
	if(tokens[j].type == '(' && !cnt){
		key = false;
		break;
		}
	if(tokens[j].type == '(') cnt --;
	if(tokens[j].type == ')') cnt ++;
	}
	if(!key) continue;
	if(tokens[i].priority <= min_pri){
		min_pri = tokens[i].priority;
		dop = i;
	}
	}
	return dop;
}
uint32_t eval(int l, int r,bool *success){
	*success = true;
	if(l > r){
	*success = false;
	return 0;
	}
	if(l == r){
		uint32_t temp;
	if(tokens[l].type == HEXNUM){
		sscanf(tokens[l].str,"%x",&temp);
		return temp;
		}
	else if(tokens[l].type == NUM){
		sscanf(tokens[l].str,"%d",&temp);
		printf("%s", tokens[l].str);
		return temp;
		}	
	
	else if(tokens[l].type == REGSNAME){
		if(strcmp(tokens[l].str + 1, "eax") == 0) return cpu.eax;
		if(strcmp(tokens[l].str + 1, "ecx") == 0) return cpu.ecx;
		if(strcmp(tokens[l].str + 1, "edx") == 0) return cpu.edx;
		if(strcmp(tokens[l].str + 1, "ebx") == 0) return cpu.ebx;
		if(strcmp(tokens[l].str + 1, "esp") == 0) return cpu.esp;
		if(strcmp(tokens[l].str + 1, "ebp") == 0) return cpu.ebp;
		if(strcmp(tokens[l].str + 1, "esi") == 0) return cpu.esi;
		if(strcmp(tokens[l].str + 1, "edi") == 0) return cpu.edi;
		if(strcmp(tokens[l].str + 1, "eip") == 0) return cpu.eip;
		 *success = false;
		return 0;
		}
		 *success = false;
		return 0;
		}
	else if(check_parentheses(l, r,success) == 1){
		return eval(l + 1, r - 1, success);
		}
	
	else {
	uint32_t dop;
	dop = make_dop(l,r);
	if(dop == l || tokens[dop].type == MINUS || tokens[dop].type == POINTER || tokens[dop].type == '!'){
	int val;
	val = eval(l + 1, r,success);
	switch(dop){
	case MINUS: return -val;
	case POINTER: return swaddr_read(val,4);
	case '!': return !val;
	default: Assert(1, "wrong expression");
		}
	}
	uint32_t val1, val2;
	val1 = eval(l , dop - 1,success);
	val2 = eval(dop + 1,r,success);
	switch(tokens[dop].type){
	case PLUS: return val1 + val2;
	case MINUS: return val1 - val2;
	case MULT: return val1 * val2;
	case DIV: return val1 / val2;
	case OR: return val1 || val2;
	case AND: return val1 && val2;
	case EQ: return val1 == val2;
	case NOT: return val1 != val2;
	default: Assert(1, "Wrong expression!");
	}

	}
	return 0;
}	
uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	*success = true;
	//panic("please implement me");
	int i;
	for(i = 0; i < nr_token; i ++){
		printf("%d\n",tokens[i].type);
	}
	return eval(0,nr_token -1, success);
}


