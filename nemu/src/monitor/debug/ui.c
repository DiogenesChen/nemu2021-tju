#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "monitor/elf.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
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

static int cmd_si(char *args) {
    if(args){
        int steps;
        char* charpointer = args;
        while (charpointer[0]) {
            if(charpointer[0] - '0' > 9 || charpointer[0] - '0' < 0)
                break;
            charpointer++;
        }
        if(!charpointer[0] && atoi(args) > 0){
            steps = atoi(args);
            cpu_exec(steps);
        }
        else
            printf("si : %s : illegal argument, non integer or too big\n", args);
    }
    else
        cpu_exec(1);
    return 0;
}

static int cmd_info(char *args) {
    if(args) {
        if( args[0] == 'r' ) {
            int i;
            for(i = 0; i <= R_EDI; i++) {
                printf( "$%s\t0x%08x\t%d\n", regsl[i], reg_l(i), reg_l(i));
            }
            printf( "$eip\t0x%08x\t%d\n", cpu.eip, cpu.eip );
        }
            //或者一个一个打出来也可以
        else if( args[0] == 'w' ) info_wp();
    }
    else printf("Invalid Command\n");
    
    return 0;
}

static int cmd_x(char *args){
    if (args == NULL) {
            printf("Argument lost, you may mean\n\tx [accessingNum] [adress]\n");
            return 0;
    }
    int num;
    swaddr_t star_adress;
    sscanf(args, "%d%x", &num, &star_adress);
    int i;
    for(i = 0; i < num; i++){
        if(!(i % 4))
            printf("\n0x%08x : ", star_adress);
        printf("0x%08x ", swaddr_read(star_adress, 4));
        star_adress+=4;
    }
    printf("\n");
    return 0;
}

static int cmd_p(char* args){
    if (args == NULL) {
        printf("Argument lost, you may mean\n\tp [expression]\n");
        return 0;
    }
    bool success;
    int val;
    val = expr(args, &success);
    if(success)
        printf("Expression value = %d, 0x%x in hex\n", val, val);
    else
        Assert(1, "Unexpected expression");
    return 0;
}

static int cmd_w(char* args){
    if (args == NULL) {
        printf("Argument lost, you may mean\n\tw [expression]\n");
        return 0;
	}
    WP *wp;
    bool suc;
    wp = new_wp();
    wp -> val = expr (args,&suc);
    if (!suc) { Assert (1,"Wrong expression\n"); delete_wp(wp -> NO); return 0; }
    printf ("Watchpoint %d: %s\n",wp -> NO, args);
    strcpy (wp -> args, args);
    printf ("Value : %d\n",wp -> val);
    return 0;
}

static int cmd_d(char* args){
	if (args == NULL) {
        printf("Argument lost, you may mean\n\td [watchpointNum]\n");
        return 0;
	}
    int num;
    sscanf(args, "%d", &num);
    delete_wp(num);
    
    return 0;
}

static int cmd_bt(char *args) {
	getFrame();
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
    { "si", "Continue the excution for peticular steps(-num), default as 1", cmd_si },
    { "info", "Print the value of registers, watchpoints", cmd_info },
    { "x", "Print the address of memory", cmd_x},
    { "p", "Calculate given expression", cmd_p},
    { "w", "Set watch point", cmd_w},
    { "d", "Delete watchpoints", cmd_d},
    { "bt", "Print the stack information", cmd_bt},

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}

