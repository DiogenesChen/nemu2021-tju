#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NUMBER, REGISTER, HNUMBER, NEQ, AND, OR,

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
    {"\\b[0-9]{1,31}\\b",NUMBER,0},      // number
    {"\\|\\|",OR,1},                // or
    {"&&",AND,2},                   // and
    {"==", EQ, 3},                  // equal
    {"!=", NEQ, 3},                 // not equal
	{"\\+", '+', 4},			    // plus
    {"-", '-', 4},                  // sub
    {"\\*", '*', 5},                // multiples
    {"/", '/', 5},                  // divide
    {"!",'!',6},                    // not
    {"\\(", '(', 7},
    {"\\)", ')', 7},                // braces
    {"\\b0[xX][0-9a-fA-F]{1,31}\\b",HNUMBER,0},        // hex number
    {"\\$[a-zA-Z]{2,3}",REGISTER,0},                  // register
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
	int type;
    int priority;
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
                    case REGISTER: {
                        tokens[nr_token].type = rules[i].token_type;
                        tokens[nr_token].priority = rules[i].priority;
                        strncpy(tokens[nr_token].str, substr_start + 1, substr_len - 1);
                        tokens[nr_token].str[substr_len-1] = '\0';
                        nr_token ++;
                        
                        break;
                    } // REGISTERS recorded as "[register name]\0", lost of first'$'
                    default:{
                        tokens[nr_token].type = rules[i].token_type;
                        tokens[nr_token].priority = rules[i].priority;
                        strncpy(tokens[nr_token].str, substr_start, substr_len);
                        tokens[nr_token].str[substr_len] = '\0';
                        nr_token ++;

                        break;
                    } // Normal types
                }
                position += substr_len;
                break;
            }
        }
		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
    }
    
	return true; 
}

uint32_t eval(uint32_t lp, uint32_t rp){
    if(lp > rp) { Assert (lp > rp, "Wrong expression!\n"); return 0;}
    else if (lp == rp){
        uint32_t num = 0;
        if(tokens[lp].type == NUMBER)
            sscanf(tokens[lp].str, "%d", &num);
        else if (tokens[lp].type == HNUMBER)
            sscanf(tokens[lp].str, "%x", &num);
        else if (tokens[lp].type == REGISTER){
            
        }
    }
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
    
    
	return 0;
}


