#include "common.h"
#include "nemu.h"
#include "monitor/elf.h"
#include <stdlib.h>
#include <elf.h>

char *exec_file = NULL;

static char *strtab = NULL;
static Elf32_Sym *symtab = NULL;
static int nr_symtab_entry;

 void read_ebp (swaddr_t addr ,PartOfStackFrame *ebp){
	ebp -> prev_ebp = swaddr_read (addr , 4);
	ebp -> ret_addr = swaddr_read (addr+4 , 4);
	int i = 0;
	for (;i < 4;i ++) ebp -> args [i] = swaddr_read (addr+8+4*i, 4);
}

void getFrame(){
	int i, j = 0;
	PartOfStackFrame now_ebp;
	char tmp[32];
	int tmplen;
	swaddr_t addr = reg_l (R_EBP);
	now_ebp.ret_addr = cpu.eip;
	while (addr > 0){
		printf ("#%d  0x%08x in ",j++,now_ebp.ret_addr);
		for(i = 0; i < nr_symtab_entry; i ++){
			if (symtab[i].st_value <= now_ebp.ret_addr && symtab[i].st_value +  symtab[i].st_size >= now_ebp.ret_addr && (symtab[i].st_info&0xf) == STT_FUNC){
				tmplen = symtab[i+1].st_name - symtab[i].st_name - 1;
				strncpy (tmp,strtab+symtab[i].st_name,tmplen);
				tmp [tmplen] = '\0';
				break;
			}
		}
		printf("<%s>\t",tmp);
		read_ebp (addr, &now_ebp);
		if (strcmp (tmp,"main") == 0)printf ("no args\n");
		else printf ("args :  %d , %d , %d , %d \n", now_ebp.args[0],now_ebp.args[1],now_ebp.args[2],now_ebp.args[3]);
		addr = now_ebp.prev_ebp;
	}
}

int getVariable(char* name, bool* success){
	*success = false;
  	int i = 0;
  	for (; i < nr_symtab_entry; i++) {
    	if ((symtab[i].st_info & 0xf) == STT_OBJECT) {
			char str[32];
			strcpy(str, strtab + symtab[i].st_name);
			if (strcmp(str, name) == 0) {
				*success = true;
				return symtab[i].st_value;
			}
    	}
  	}
  return 0;
}

void load_elf_tables(int argc, char *argv[]) {
	int ret;
	Assert(argc == 2, "run NEMU with format 'nemu [program]'");
	exec_file = argv[1];

	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	uint8_t buf[sizeof(Elf32_Ehdr)];
	ret = fread(buf, sizeof(Elf32_Ehdr), 1, fp);
	assert(ret == 1);

	/* The first several bytes contain the ELF header. */
	Elf32_Ehdr *elf = (void *)buf;
	char magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};

	/* Check ELF header */
	assert(memcmp(elf->e_ident, magic, 4) == 0);		// magic number
	assert(elf->e_ident[EI_CLASS] == ELFCLASS32);		// 32-bit architecture
	assert(elf->e_ident[EI_DATA] == ELFDATA2LSB);		// littel-endian
	assert(elf->e_ident[EI_VERSION] == EV_CURRENT);		// current version
	assert(elf->e_ident[EI_OSABI] == ELFOSABI_SYSV || 	// UNIX System V ABI
			elf->e_ident[EI_OSABI] == ELFOSABI_LINUX); 	// UNIX - GNU
	assert(elf->e_ident[EI_ABIVERSION] == 0);			// should be 0
	assert(elf->e_type == ET_EXEC);						// executable file
	assert(elf->e_machine == EM_386);					// Intel 80386 architecture
	assert(elf->e_version == EV_CURRENT);				// current version


	/* Load symbol table and string table for future use */

	/* Load section header table */
	uint32_t sh_size = elf->e_shentsize * elf->e_shnum;
	Elf32_Shdr *sh = malloc(sh_size);
	fseek(fp, elf->e_shoff, SEEK_SET);
	ret = fread(sh, sh_size, 1, fp);
	assert(ret == 1);

	/* Load section header string table */
	char *shstrtab = malloc(sh[elf->e_shstrndx].sh_size);
	fseek(fp, sh[elf->e_shstrndx].sh_offset, SEEK_SET);
	ret = fread(shstrtab, sh[elf->e_shstrndx].sh_size, 1, fp);
	assert(ret == 1);

	int i;
	for(i = 0; i < elf->e_shnum; i ++) {
		if(sh[i].sh_type == SHT_SYMTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".symtab") == 0) {
			/* Load symbol table from exec_file */
			symtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(symtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
			nr_symtab_entry = sh[i].sh_size / sizeof(symtab[0]);
		}
		else if(sh[i].sh_type == SHT_STRTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".strtab") == 0) {
			/* Load string table from exec_file */
			strtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(strtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
		}
	}

	free(sh);
	free(shstrtab);

	assert(strtab != NULL && symtab != NULL);

	fclose(fp);
}