#include "nemu.h"
#include "monitor/elf.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
    NOTYPE = 256, EQ, NUMBER, REGISTER, HNUMBER, NEQ, AND, OR, POINTER, MINUS, VAR

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

    {" +",    NOTYPE, 0},                // spaces
    {"\\b[0-9]{1,31}\\b",NUMBER,0}, // number
    {"\\|\\|",OR,1},                // or
    {"&&",AND,2},                   // and
    {"==", EQ, 3},                  // equal
    {"!=", NEQ, 3},                 // not equal
    {"\\+", '+', 4},                // plus
    {"-", '-', 4},                  // sub
    {"\\*", '*', 5},                // multiples
    {"/", '/', 5},                  // divide
    {"!",'!',6},                    // not
    {"\\(", '(', 7},
    {"\\)", ')', 7},                // braces
    {"\\b0[xX][0-9a-fA-F]{1,31}\\b",HNUMBER,0},        // hex number
    {"\\$[a-zA-Z]{2,3}",REGISTER,0},                  // register
    {"[a-zA-Z][A-Za-z0-9_]*", VAR, 0},               //varible
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

                //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);

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

bool check_parentness(uint32_t lp, uint32_t rp){
    if(tokens[lp].type == '(' && tokens[rp].type == ')'){
        int lbn = 0, rbn = 0;
        int i = lp + 1;
        for(; i < rp; i ++){
            if(tokens[i].type == '(') lbn ++;
            if(tokens[i].type == ')') rbn ++;
            if(rbn > lbn) return false;
        }
        
        return true;
    }
    
    else return false;
}

uint32_t make_dop(int lp, int rp){
    int i, j;
    int dop = lp;
    int min_priority = 10;
    for(i = lp; i <= rp; i ++){
        if (tokens[i].type == NUMBER || tokens[i].type == HNUMBER 
                || tokens[i].type == REGISTER || tokens[i].type == VAR)
                    continue;
        int cnt = 0;
        bool key = true;
        for (j = i - 1; j >= lp ;j --){
            if (tokens[j].type == '(' && !cnt){key = false;break;}
            if (tokens[j].type == '(')cnt --;
            if (tokens[j].type == ')')cnt ++;
        }
            if (!key)continue;
            if (tokens[i].priority <= min_priority){min_priority = tokens[i].priority; dop = i;}
    }
    
    return dop;
}

uint32_t eval(int lp, int rp){
    if(lp > rp) { Assert (lp > rp, "Wrong expression!\n"); return 0;}
    
    else if (lp == rp){
        uint32_t num = 0;
        if(tokens[lp].type == NUMBER)
            sscanf(tokens[lp].str, "%d", &num);
        else if (tokens[lp].type == HNUMBER)
            sscanf(tokens[lp].str, "%x", &num);
        else if (tokens[lp].type == REGISTER){
            if(strlen(tokens[lp].str) == 3){
                int i;
                for(i = R_EAX; i <= R_EDI; i++){
                    if(strcmp(tokens[lp].str, regsl[i]) == 0) {num = reg_l(i);  break;}
                }
                if( i > R_EDI && strcmp(tokens[lp].str, "eip") == 0) num = cpu.eip;
                    else Assert(1, "No such register, you may check for your spellings");
            }
            else if(strlen(tokens[lp].str) == 2){
                int i;
                if(tokens[lp].str[1] == 'x' || tokens[lp].str[1] == 'i' || tokens[lp].str[1] == 'p'){
                    for(i = R_AX; i <= R_DI; i ++){
                        if(strcmp(tokens[lp].str, regsw[i]) == 0) {num = reg_w(i);  break;}
                        else Assert(1, "No such register, you may check for your spellings");
                    }
                }
                else if(tokens[lp].str[1] == 'h' || tokens[lp].str[1] == 'l'){
                    for(i = R_AL; i <= R_BH; i ++){
                        if(strcmp(tokens[lp].str, regsb[i]) == 0) {num = reg_b(i);  break;}
                        else Assert(1, "No such register, you may check for your spellings");
                    }
                }
            }
        }
        else if (tokens[lp].type == VAR) {
            bool success = false;
            num = getVariable(tokens[lp].str, &success);
            if(!success) Assert(1, "wrong varibale");
        }

        else assert(1);
        
        return num;
    }
    
    else if(check_parentness(lp, rp) == 1){
        return eval(lp + 1, rp - 1);
    }
    else{
        uint32_t dop;
        dop = make_dop(lp, rp);
        if(dop == lp || tokens[dop].type == MINUS || tokens[dop].type == POINTER
                || tokens[dop].type == '!'){
            int val;
            val = eval(lp + 1, rp);
            switch (tokens[dop].type) {
                case MINUS: return -val;
                case POINTER: return swaddr_read(val, 4);
                case '!': return !val;
                default: Assert(1, "Wrong expression!");
            }
        }
        uint32_t val1, val2;
        val1 = eval(lp, dop - 1); val2 = eval(dop + 1, rp);
        
        switch (tokens[dop].type) {
            case '+': return val1 + val2;
            case '-': return val1 - val2;
            case '*': return val1 * val2;
            case '/': return val1 / val2;
            case OR: return val1 || val2;
            case AND: return val1 && val2;
            case EQ: return val1 == val2;
            case NEQ: return val1 != val2;
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
    int i = 0;
    for (i = 0;i < nr_token; i ++) {
        if (tokens[i].type == '*' && 
                (i == 0 || 
                    (tokens[i - 1].type != NUMBER && tokens[i - 1].type != HNUMBER && tokens[i - 1].type != REGISTER && tokens[i - 1].type != VAR  && tokens[i - 1].type !=')'))) {
            tokens[i].type = POINTER;
                tokens[i].priority = 6;
            }
        if (tokens[i].type == '-' && 
                (i == 0 || 
                    (tokens[i - 1].type != NUMBER && tokens[i - 1].type != HNUMBER && tokens[i - 1].type != REGISTER && tokens[i - 1].type != VAR  && tokens[i - 1].type !=')'))) {
                tokens[i].type = MINUS;
                tokens[i].priority = 6;
            }
    }
    *success = true;
    
    return eval(0, nr_token - 1);
}



