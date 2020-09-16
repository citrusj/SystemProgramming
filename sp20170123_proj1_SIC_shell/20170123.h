#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#define MAX_HASH 20
#define MAX_MNE 10
#define MAX_CMD 10
#define MB_SIZE 1048576

typedef enum{
	Help, Dir, Quit, History, Dump, Edit, Fill, Reset, Opcode, Opcodelist, Error
}Comd;

typedef struct hisNode *hisPtr;
typedef struct hisNode{
	int num;
	char data[MAX_CMD];
	int args;
	int argv[3];
	char mnemonic[MAX_MNE];
	hisPtr link;
}hisNode;

typedef struct opnode* opPtr;
typedef struct opnode{
	int opcode;
	char mnemonic[MAX_MNE];
	float value;
	opPtr next;
}opnode;

void push_node(hisPtr* top, char* command, int args, ...);

Comd getcom(char* command, hisPtr* last);

void print_history(hisPtr first);

void print_help();

void print_prompt(void);

void print_dir();

void print_dump(char* memptr, int start, int end);

int edit_memory(char* mem, int* edit_address, int* edit_value);

int fill_memory(char* mem, int* fill_start, int* fill_end, int* fill_value);

void reset_memory(char* mem);

int get_hashkey(char* mnemonic);

void make_opcodetable(FILE *fp, opPtr* hashtable);

void print_input_error();

int print_opcode(opPtr* hashtable, char* op_str);

void print_opcodetable(opPtr* hashtable);


