#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include "monitor/elf.h"
#include <regex.h>

enum {
  NOTYPE = 256, EQ, NEQ, AND, OR, MINUS, POINTER, DEX, HEX, VARIABLE, REGISTER

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type, prior;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"\\b0[xX][0-9a-fA-F]+\\b", HEX, 0},
  {"\\b[0-9]+\\b", DEX, 0},
  {"\\$[a-zA-Z]+", REGISTER, 0},
  {" +",	NOTYPE, 0},				// spaces
  {"\\+", '+', 4},					// plus
  {"-", '-', 4},                //jian
  {"\\*", '*', 5},             //cheng
  {"/", '/', 5},                 //chu
  {"==", EQ, 3},						// equal
  {"!=", NEQ, 3},          //budneg
  {"!", '!', 6},
  {"&&", AND, 2},       ///AND
  {"\\|\\|", OR, 1},      //or
  {"\\(", '(', 7},
  {"\\)", ')', 7},
  {"[a-zA-Z][A-Za-z0-9_]*", VARIABLE, 0},

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
  char str[32];
  int prior;
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

        char *ls = e + position + 1;
        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if (substr_len > 32) { // in case str overflow
          puts("Too Long expr!!!");
          assert(0);
        }
        if (rules[i].token_type == NOTYPE) {
          break;
        }
        if (rules[i].token_type == REGISTER) {
          strncpy(tokens[nr_token].str, ls, substr_len - 1);
          tokens[nr_token].str[substr_len-1] = '\0';
        } else if (rules[i].token_type == VARIABLE) {
          strncpy(tokens[nr_token].str, e + position - substr_len, substr_len);
          tokens[nr_token].str[substr_len] = '\0';
        } else {
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          tokens[nr_token].str[substr_len] = '\0';
        }
        tokens[nr_token].type = rules[i].token_type;
        tokens[nr_token].prior = rules[i].prior;
        nr_token++;

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

bool check_parentheses(int l, int r) {
  int i, flag = 0;
  if (tokens[l].type !=  '(' || tokens[r].type != ')')
    return false;
  for (i = l; i <= r; ++ i) {
    if (tokens[i].type == '(')
      flag++;
    else if (tokens[i].type == ')')
      flag--;
    if (flag == 0 && i < r)
      return false;
  }
  if (flag != 0)
    return false;
  return true;
}



int dominant_operator(int l, int r) {
  int i, pos = l;
  int ls = 10, flag = 0;
  for (i = l; i <= r; ++ i) {
    if (tokens[i].type >= DEX && tokens[i].type <= REGISTER)
      continue;
    if (tokens[i].type == '(') {
      flag ++; i ++;
      while (i <= r) {
        if (tokens[i].type == '(')
          flag ++;
        else if (tokens[i].type == ')')
          flag--;
        if (flag == 0)
          break;
        i ++;
      }
      if (flag) {
        return -1;
      }
    }
    if (tokens[i].prior <= ls) {
      ls = tokens[i].prior;
      pos = i;
    }
  }
  return pos;
}

uint32_t eval(int l, int r, bool *succuess) {
  //Log("%d %d", l, r);
  if (l > r) {
    *succuess = false;
    return 0;
  } else if (l == r) {
    uint32_t num = 0;
    // printf("str: %d\n", tokens[l].type);
    if (tokens[l].type == DEX)
      sscanf(tokens[l].str, "%d", &num);
    if (tokens[l].type == HEX)
      sscanf(tokens[l].str, "%x", &num);
    // printf("num %d\n", num);
    if (tokens[l].type == REGISTER) {
      int i;
      uint32_t len = strlen(tokens[l].str);
      if (len == 3) {
        for (i = R_EAX; i <= R_EDI; ++ i)
          if (!strcmp(tokens[l].str, regsl[i]))
            break;
        if (i > R_EDI)
          if (!strcmp(tokens[l].str, "eip"))
            num = cpu.eip;
          else *succuess = false;
        else num = reg_l(i);
        //printf("NUM: %d\n", num);
      }
      if (len == 2) {
        if (tokens[l].str[1] == 'x' || tokens[l].str[1] == 'p' || tokens[l].str[1] == 'i') {
          for (i = R_AX; i <= R_DI; ++ i) {
            if (!strcmp(tokens[l].str, regsw[i]))
              break;
          }
          num = reg_w(i);
        }
        else if (tokens[l].str[1] == 'l' || tokens[l].str[1] == 'h') {
          for (i = R_AL; i <= R_BH; ++ i) {
            if (!strcmp(tokens[l].str, regsb[i]))
              break;
          }
          num = reg_b(i);
        }
        else assert(1);
      }
    }
    if (tokens[l].type == VARIABLE) {
      //Log("%d", (int)(*succuess));
      return getVariable(tokens[l].str, succuess);
    }
    return num;
  }
  else if (check_parentheses(l, r) == true) {
    return eval(l + 1, r - 1, succuess);
  }
  else {
    int op = dominant_operator(l, r);
    //Log("op: %d", op);
    if (op == -1) {
      *succuess = false;
      return -1;
    }
    if (l == op || tokens[op].type == POINTER || tokens[op].type == MINUS || tokens[op].type == '!') {
      uint32_t ls = eval(l + 1, r, succuess);
      //Log("ls: %d", ls);
      switch (tokens[op].type) {
        case POINTER:
          return swaddr_read(ls, 4);
        case MINUS:
          return -ls;
        case '!':
          return !ls;
        default:
          *succuess = false;
          return -1;
          //Assert(1, "ERROER");
      }
    }
    uint32_t val1 = eval(l, op - 1, succuess), val2 = eval(op + 1, r, succuess);
    //Log("val1 %d, val2 %d, l %d, r %d", val1, val2, l, r);
    switch (tokens[op].type) {
      case '+':
        return val1 + val2;
      case '-':
        return val1 - val2;
      case '*':
        return val1 * val2;
      case '/':
        return val1 / val2;
      case EQ:
        return val1 == val2;
      case NEQ:
        return val1 != val2;
      case AND:
        return val1 && val2;
      case OR:
        return val1 || val2;
      default:
        break;
    }
  }
  //*succuess = false;
  return -1;
}

uint32_t expr(char *e, bool *success) {
  if(!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //Log("make_token success");
  int i;
  for (i = 0; i < nr_token; i++) {
    if (i == 0 || ((tokens[i - 1].type < DEX || tokens[i - 1].type > REGISTER) && tokens[i - 1].type != ')')) {
      if (tokens[i].type == '*') {
        tokens[i].type = POINTER;
        tokens[i].prior = 6;
      }
      if (tokens[i].type == '-') {
        tokens[i].type = MINUS;
        tokens[i].prior = 6;
      }
    }
    // printf("%s %d\n", tokens[i].str, tokens[i].type);
  }

  /* TODO: Insert codes to evaluate the expression. */
  *success = true;
  return eval(0, nr_token - 1, success);
}

