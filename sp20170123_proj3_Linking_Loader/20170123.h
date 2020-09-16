#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#define MAX_HASH 20
#define MAX_MNE 10
#define MAX_hiString 100
#define MAX_CMD 10
#define MB_SIZE 1048576
#define MAX_SYMHASH 26

typedef enum{
	Help, Dir, Quit, History, Dump, Edit, Fill, Reset, Opcode, Opcodelist, Type, Assemble, Symbol, Error, Progaddr, Loader, Run, Bp
}Comd;

typedef struct hisNode *hisPtr;
typedef struct hisNode{
	int num;
	char data[MAX_CMD];
	int args;
	int argv[3];
	char string[MAX_hiString];
	hisPtr link;
}hisNode;

typedef struct opnode* opPtr;
typedef struct opnode{
	int opcode;
	char mnemonic[MAX_MNE];
	int format;
	opPtr next;
}opnode;

typedef struct state* statePtr;
typedef struct state{
	char buffer[255];
	char symbol[20];
	char opcode[20];
	char operand[20];
	char operand2[20];
	int loc;
	char object[20];
	statePtr next;
}state;

typedef struct symnode* symPtr;
typedef struct symnode
{
	char symbol[10];
	int address;
	symPtr next;
}symnode;

typedef struct exsymnode* exsymPtr;
typedef struct exsymnode
{
	char ctrsection[10];
	char symname[10];
	int address;
	int label;
	int length;
	exsymPtr next;
}exsymnode;

typedef struct refernode* referPtr;
typedef struct refernode
{
	char name[10];
	int address;
}refernode;

opPtr hashtable[MAX_HASH];
symPtr* symTable;
exsymPtr* exsymTable;
referPtr refertable[10];

int PROGADDR=0x00;
int PROGLEN = 0x00;
int csaddr;
int bplist[100];
int bpnum = 0;
int runaddr = 0x00;
int reglist[10] = { 0 };
int bpflag = 0;
int finishflag = 0;
int loadflag = 0;
void push_node(hisPtr* top, char* command, int args, ...);

Comd getcom(char* command, hisPtr* last);

void print_history(hisPtr first);

void print_help();

void print_prompt();

void print_dir();

void print_dump(unsigned char* memptr, int start, int end);

int edit_memory(unsigned char* mem, int* edit_address, int* edit_value);

int fill_memory(unsigned char* mem, int* fill_start, int* fill_end, int* fill_value);

void reset_memory(unsigned char* mem);

int get_hashkey(char* mnemonic);

void make_opcodetable(FILE *fp);

void print_input_error();

int print_opcode(char* op_str);

opPtr find_opcode(char* op_str);

void print_opcodetable();

int type_file(char *filename);

int get_symhashkey(char* symbol);

int insert_symbol(char* symbol, int address);

int print_symbol();

int find_symbol(char *symbol);

int change_loc(int* LOC, statePtr node, int*start_add);

void get_nix(char *opcode, char *opcode2, int*nn, int* ii, int* xx, int* op_index);

int get_regnum(char reg);

void print_asm_error(int linenum);

int pass1(FILE *asmf, statePtr state_start, statePtr state_end, int* LOC, int* start_add);

int pass2(char* lst_string, char* obj_string, statePtr state_start, statePtr state_end, int *LOC, int *start_add, int proglen);

int find_refaddr(int file_num, char* refername);

char *strip(char *string);

void set_progaddr();