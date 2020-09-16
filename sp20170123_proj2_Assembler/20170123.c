#include "20170123.h"

/* function: push_node() */
/* To push new node to History Linked List */
/* return: nothing */
void push_node(hisPtr* top, char* command, int args, ...)
{
	//insert node to history linked list
	va_list argp;
	va_start(argp, args);

	hisPtr tmp;
	tmp = malloc(sizeof(hisNode));
	sprintf(tmp->data, "%s", command);
	tmp->num = (*top)->num+1;
	tmp->args = args;
	
	//make node for dump, edit, fill
	for(int i=0;i<args && i<3 ;i++)
	{
		tmp->argv[i] = va_arg(argp,int);
	}
	//make node for opcode mnemonic
	if(args == 4) sprintf(tmp->string, "%s", va_arg(argp,char *));
	
	va_end(argp);

	tmp->link = NULL;
	//link
	(*top)->link = tmp;
	*top = tmp;
}

/*funtion: getcom()*/
/*To return enum value of command to main, push history node for some commands*/
/*return: enum value in Comd*/
Comd getcom(char *command, hisPtr *last)
{
	//mapping command to function
	if(strcmp(command, "help")==0 || strcmp(command, "h") == 0){
		push_node(last, command, 0);
		return Help;
	}
	else if(strcmp(command, "dir")==0 || strcmp(command, "d") ==0){
		push_node(last, command, 0);
		return Dir;
	}
	else if(strcmp(command, "quit") == 0 || strcmp(command, "q") == 0) {
		push_node(last, command, 0);
		return Quit;
	}
	else if(strcmp(command, "history") == 0 || strcmp(command, "hi") == 0) {
 		push_node(last, command, 0);
		return History;
	}
	else if(strcmp(command, "dump") == 0 || strcmp(command, "du") == 0) {
		return Dump;
	}
	else if(strcmp(command, "edit") == 0 || strcmp(command, "e") == 0){
		return Edit;
	}
	else if(strcmp(command, "fill") == 0 || strcmp(command, "f") == 0){
		return Fill;
	}
	else if(strcmp(command, "reset") == 0){
		push_node(last, command,0);
		return Reset;
	}
	else if(strcmp(command, "opcode") == 0){
		return Opcode;
	}
	else if(strcmp(command, "opcodelist") == 0){
		push_node(last, command,0);
		return Opcodelist;
	} 
	else if(strcmp(command, "type") == 0){
		return Type;
	}
	else if(strcmp(command, "assemble") == 0){
		return Assemble;
	}
	else if(strcmp(command, "symbol") == 0){
		push_node(last, command, 0);
		return Symbol;
	}
	//when there is no command
	else{
		while(getchar()!='\n');
		printf("Error: command not exist\n");
		return Error;
	}
}

/*function: print_history()*/
/*To print History by traversing linked list consists of hisNode and hisPtr*/
/*return: nothing*/
void print_history(hisPtr first)
{	
	//print history
	hisPtr tmp = first->link;
	while(tmp)
	{
		//print number and command
		printf("%-5d %s ",tmp->num, tmp->data);
		//case for opcode mnemonic history
		if(tmp->args==4){
			printf("%s",tmp->string);
		}
		//case for dump, edit, fill history
		else{
			for(int i=0;i<tmp->args;i++)
			{
				printf("%X",tmp->argv[i]);
				if(i<tmp->args-1) printf(", ");
			}
		}
		printf("\n");
		tmp = tmp->link;
	}
	return;
}
/*function:print_help()*/
/*To print text about shell commands*/
/*return: nothing*/
void print_help()
{
	//print all commands
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\n");
	return;
}	

/*function: print_prompt()*/
/*To pring prompt message*/
/*return: nothing*/
void print_prompt(void){

	fprintf(stdout, "sicsim> ");
	return;
}

/*function: print_dir()*/
/*To print files in current path. Check whether folder or executable file*/
/*return: nothing*/
void print_dir()
{
	DIR *dir;
	char printbuf[100];
	struct dirent *ent;
	struct stat stat_buf;
	int print_cnt=0;
	dir = opendir("./");
	if (dir!= NULL){
		while((ent = readdir(dir))!=NULL){
			if((strcmp(ent->d_name,"..")==0) ||( strcmp(ent->d_name,".")==0)) continue;
			

			if(lstat(ent->d_name, &stat_buf)>=0){
				if(S_ISDIR(stat_buf.st_mode)==1){
					sprintf(printbuf,"%s/",ent->d_name);
				}
				else if (S_IXUSR & stat_buf.st_mode){
					sprintf(printbuf,"%s*",ent->d_name);
				}
				else sprintf(printbuf, "%s ",ent->d_name);
			}
			printf("%-15s",printbuf);
			print_cnt++;
			if(print_cnt==4){
				printf("\n"); print_cnt=0;
			}
			
		}	
		closedir(dir);
		}
	printf("\n");
	return;
}

/*function: print_dump()*/
/*To print memory address, memory value and corresponding ascii code in the range of start~end address*/
/*return: nothing*/
void print_dump(char* memptr, int start, int end)
{
	char* tmpptr = memptr;
	size_t offset = start;
	size_t address = (start/16)*16;
	
	//buf that copies  one line memory
	char* buf;
	buf = malloc(sizeof(char)*16);
	
	int i,j;
	for(i = (start/16);i<=(end/16);i++)
	{
		memcpy(buf, tmpptr+16*i, 0x16);
		printf("%05X  ",(unsigned int)address);
		//print blank for [address:start]
		for(j = address;j<offset;j++)	printf("   ");
		//print value in line
		for(;(offset<address+0x10) && (offset<=end);offset++)
			 printf("%02X ", (unsigned char)buf[offset%0x10]);
		//print blank for [end:endof line]
		for(j=offset;j<address+0x10;j++) printf("   ");
		printf("; ");
		//print exacn ascii code
		for(j=0;j<16;j++)
		{
			if(buf[j]>=0x20 && buf[j]<=0x7E) printf("%c",buf[j]);
			else printf(".");
		}
		//increase address
		address += 0X10;
		printf("\n");
	}
	free(buf);
	
}

/*function: edit_memory()*/
/*To edit value of the address of memory*/
/*if there is a input error return -1, else return 0*/
int edit_memory(char* mem, int *edit_address, int *edit_value)
{
	
	char x, y;
	x = getchar();
	
	//error: no argvs
	if(x == '\n' || x == '\0'){
		return -1;
	}
	scanf("%x",edit_address);
	while(1)
	{
		y = getchar();
		//error: no value
		if(y == '\n' || y=='\0'){
			return -1;
		}
		//valid input
		else if(y==','){
			scanf("%x",edit_value);
			break;
		}
	}

	//address or value is not valid
	if(*edit_address<0x00 || *edit_address>0xfffff) return -1;
	if(*edit_value<0x00 || *edit_value>0xFF) return -1;
	
	//edit memory
	while(getchar()!='\n');
	*(mem+(*edit_address)) = (*edit_value);
	return 0;
}

/*function:fill_memory()*/
/*To fill the memory value in a range of start~end*/
/*return: if there is an input error, return -1, else return 0*/
int fill_memory(char* mem, int *fill_start, int* fill_end, int* fill_value)
{
	int i;
	char x, y, z, c;
	x = getchar();
	//no enough argvs
	if(x=='\n' || x=='\0') {
		return -1;
	}
	scanf("%x", fill_start);
	while(1)
	{
		y = getchar();
		//no enough argvs
		if(y=='\n' || y=='0'){
			return -1;
		}
		else if(y==','){
			scanf("%x",fill_end);
			while(1){
				z = getchar();
				//no enough argvs
				if(z == '\n' || y=='\0'){
					return -1;
				}
				//valid input
				else if(z==','){
					scanf("%x",fill_value);break;
				}
			}
			break;
		}
	}
	while((c=getchar())!= '\n');
	
	//input error handling
	if(*fill_start<0x00 || *fill_start>0xfffff) return -1;
	if(*fill_end<0x00 || *fill_end>0xfffff) return -1;
	if(*fill_value<0x00 || *fill_value>0xFF) return -1;
	if(*fill_start > *fill_end) return -1;	
	
	//valid input->fill
	for(i=(*fill_start);i<=(*fill_end);i++)
	{
		*(mem+i) = (*fill_value);
	}
	return 0;
}

/*function: reset_memory()*/
/*To reset all range of memory value to 0x00*/
/*return: nothing*/
void reset_memory(char *mem)
{
	//reset all memory to 0x00
	size_t k;
	for(k=0;k<MB_SIZE;k++)
	{
		*(mem+k) = 0x00;
	}
}	

/*function: get_hashkey()*/
/*To get the key value using string of mnemonic. Kinda hash function*/
/*return: hash key of input mnemonic string*/
int get_hashkey(char* mnemonic)
{
	//hash function
	int key;
	key = (mnemonic[0]+mnemonic[1]*2)%20;
	return key;
}

/*function: make_opcodetable()*/
/*make opcodetable in the structure of hashtable by reading text file*/
/*return: nothing*/
void make_opcodetable(FILE *fp)
{	
	//make opcodetable in hash table	
	opPtr node;
	char c;
	int i, up, down, key;
	while(1)
	{
		i=0;up=0;down=1;
		//create node
		node = (opPtr)malloc(sizeof(opnode));
		fscanf(fp, "%x",&node->opcode);
		for (c = fgetc(fp); (c == ' ' || c == '\t'); c = fgetc(fp));
		if (c == EOF) break;
		for(;(c!=' ');c=fgetc(fp)){
			if (c == '\t') break;
			node->mnemonic[i++] = c;
		}
		node->mnemonic[i] =  '\0';
		fscanf(fp, "%d",&up);
		if((c=fgetc(fp))=='/'){
			fscanf(fp, "%d",&down);
			c=fgetc(fp);
		}
		
		node->format = up;
		key = get_hashkey(node->mnemonic);
		
		//insert node to hashtable
		node->next = hashtable[key];
		hashtable[key] = node;
	}
}

/* function: print_input_error()*/
/*To print error message in other function*/
/*return: nothing*/
void print_input_error()
{
	//print error msg when user type wrong input
	printf("Error: input format is not valid\n");
	return;
}

/*function: print_opcode()*/
/*To print corresponding opcode of input mnemonic*/
/*return: if there is an input error, return -1, else return 0*/
int print_opcode(char* op_str)
{	
	//error: no mnemonic
	if(getchar() == '\n') return -1;
	
	//print mnemonic with opcode
	scanf("%s",op_str);
	opPtr find = find_opcode(op_str);
	if(find == NULL) return -1;	
	int opcode = find->opcode;
	printf("opcode is %x\n", opcode);
	
	return 0;
}
//function find_opcode()*/
/*To find opcode and return opPtr*/
/*return found opPtr*/
opPtr find_opcode(char* op_str)
{
	opPtr op_node;
	for(op_node = hashtable[get_hashkey(op_str)];op_node!=NULL;op_node = op_node->next)
	{
		if (strcmp(op_node->mnemonic, op_str) == 0) {
			return op_node;
			break;
		}
	}
	if (op_node == NULL) {
		printf("Error: invalid mnemonic %s\n", op_str);
		return (opPtr)NULL;
	}
	return op_node;
}
/*function: print_opcodetable()*/
/*To print hashtable of opcode-mnemonic*/
/*return: nothing*/
void print_opcodetable()
{
	//print hashtable with opcode	
	opPtr op_node;
	int i;
	for(i=0;i<MAX_HASH;i++)
	{
		printf("%d :",i);
		op_node = hashtable[i];
		if(op_node == NULL){
			printf("\n");
			continue;
		}
		printf(" [%s,%X] ", op_node->mnemonic, op_node->opcode);
		op_node = op_node->next;
		while(op_node)
		{
			printf("->");
			printf(" [%s,%X] ", op_node->mnemonic, op_node->opcode);
			op_node = op_node->next;
		}
		printf("\n");
	}
}
/*function type_file()*/
/*To cat file through shell*/
/*return: if there is no file return -1*/
int type_file(char *filename)
{	
	char c;
	scanf("%s",filename);
	FILE *typef = fopen(filename,"r");
	if(typef==NULL){
		printf("Error: File not exist\n");
		return -1;	
	}
	while(1){
		c=fgetc(typef);
		if (c == EOF) break;
		printf("%c",c);
	}
	fclose(typef);
	return 0;
}
/*function get_symhashkey()*/
/*To get hashkey of symbol table*/
/*return: if symbol is not alphabet, retunr -1*/
int get_symhashkey(char* symbol)
{
	int key = 25 - (symbol[0] - 65);
	if (key < 0 || key>25) return -1;
	return key;
}
/*function insert_symbol*/
/*To insert symbol node to symboltable(hashtable)*/
/*return: if input symbol is already exist, return -1*/
int insert_symbol(char* symbol, int address)
{
	int key = get_symhashkey(symbol);
	symPtr tmp;
	symPtr node = (symPtr)malloc(sizeof(symnode));
	strcpy(node->symbol, symbol); node->address = address;
	if (symTable[key] == NULL) {
		node->next = NULL;
		symTable[key] = node;
		return 0;
	}
	if (strcmp(symTable[key]->symbol, symbol) < 0) {
		node->next = symTable[key];
		symTable[key] = node;
		return 0;
	}
	for (tmp = symTable[key]; tmp != NULL; tmp = tmp->next)
	{
		if (strcmp(tmp->symbol, symbol) == 0) {
			return -1;
		}
		if (tmp->next == NULL || strcmp(tmp->next->symbol, symbol) < 0) {
			break;
		}
	}

	node->next = tmp->next;
	tmp->next = node;
	return 0;
}
/*function print_symbol()*/
/*To print symbol table*/
/*return: if there is no symbol, return -1*/
int print_symbol()
{
	if(symTable == NULL){
		printf("Error: symbol table is Empty. Please assemble any file\n");
		return -1;
	}
	int i = 0;
	symPtr tmp;
	for (i = 0; i < MAX_SYMHASH; i++)
	{
		for (tmp = symTable[i]; tmp != NULL; tmp = tmp->next)
		{
			printf("\t%s\t%04X\n", tmp->symbol, tmp->address);
		}
	}
	return 0;
}
/*function find_symbol()*/
/*To find symbol in symboltable and get opcode of it */
/*return: if there is no symbol in table, return -1, if there is a symbol return corresponding opcode*/
int find_symbol(char *symbol)
{
	int key = get_symhashkey(symbol);
	int address;
	if (key < 0) return -1;
	symPtr tmp;
	for (tmp = symTable[key]; tmp != NULL; tmp = tmp->next)
	{
		if (strcmp(tmp->symbol, symbol) == 0) {
			address = tmp->address; break;
		}
	}
	if (tmp == NULL) return -1;
	return address;
}
/*functon change_loc()*/
/*To change location in asm file*/
/*return: if the format is invalid, return -1 , else return 0*/
int change_loc(int* LOC, statePtr node, int*start_add)
{
	//increase LOC in regards of opcode
	if (node->buffer[0] == '.') return 0;
	opPtr find = (opPtr)malloc(sizeof(opnode));
	int i;

	if (node->opcode[0] == '+') {
		*LOC += 4;
	}
	else if (strcmp(node->opcode, "WORD") == 0) {
		//Error Handling for Non Decimal operand
		for (i = 0; node->operand[i] != '\0'; i++) {
			if (node->operand[i] < 48 || node->operand[i]>57) return -1;
		}
		*LOC += 3;
	}
	else if (strcmp(node->opcode, "RESW") == 0) {
		//Error Handling for Non Decimal operand
		for (i = 0; node->operand[i] != '\0'; i++) {
			if (node->operand[i] < 48 || node->operand[i]>57) return -1;
		}
		*LOC += (3 * atoi(node->operand));
	}
	else if (strcmp(node->opcode, "RESB") == 0) {
		//Error Handling for Non Decimal operand
		for (i = 0; node->operand[i] != '\0'; i++) {
			if (node->operand[i] < 48 || node->operand[i]>57) return -1;
		}
		*LOC += atoi(node->operand);
	}
	else if (strcmp(node->opcode, "BYTE") == 0) {
		int byte_len = 0; int j = 0;
		if (node->operand[0] == 'C') {
			while (node->operand[j++] != '\'');
			while (node->operand[j++] != '\'') byte_len++;
		}
		else if (node->operand[0] == 'X') {
			byte_len = 1;
		}
		else {
			return -1;
		}
		*LOC += byte_len;
	}
	else if (strcmp(node->opcode, "START") == 0) {
		if(*LOC!=0) return -1; //START Duplicate Error
		*start_add = strtol(node->operand, NULL, 16);
		*LOC = *start_add;
		node->loc = *LOC;
	}
	else if ((strcmp(node->opcode, "BASE") == 0) || (strcmp(node->opcode, "END") == 0));
	else {
		find = find_opcode(node->opcode);
		if (find == NULL) return -1;
		*LOC += find->format;
	}
	return 0;
}
/*function get_nix()*/
/*To get the value of n, i , x according to statement*/
/*return void*/
void get_nix(char *opcode, char *opcode2, int*nn, int* ii, int* xx, int* op_index)
{
	*xx = 0; *op_index = 1;
	//immediate addressing
	if (opcode[0] == '#') {
		*nn = 0; *ii = 1;
	}
	//indirect addressing
	else if (opcode[0] == '@') {
		*nn = 1; *ii = 0;
	}
	//simple addressing
	else {
		*nn = 1; *ii = 1; *op_index = 0;
	}
	//if uses X register
	if (*opcode2 == 'X') {
		*xx = 1;
	}
	return;
}
/*function get_regnum()*/
/*To get register number*/
/*return: corresponding register number*/
int get_regnum(char reg)
{
	int value = 0;
	switch (reg)
	{
	case 'X':value = 1; break;
	case 'A':value = 0; break;
	case 'S':value = 4; break;
	case 'L':value = 2; break;
	case 'T':value = 5; break;
	case 'B':value = 3; break;
	case 'F':value = 6; break;

	}
	return value;
}
/*funciton print_asm_error()*/
/*To print error message with line for debugging*/
/*return void*/
void print_asm_error(int linenum)
{
	printf("Error at line %d\n", linenum); return;
}
/*function pass1()*/
/*To get location number of each line and make symboltable, check syntax error of the code*/
/*return: if there is error in code or symbol duplicate, return -1, else return 0*/
int pass1(FILE *asmf, statePtr state_start, statePtr state_end, int* LOC, int* start_add)
{
	char buffer[255]; //scaned file line
	int index; //buffer index
	int i; int linenum = 0;
	*LOC = 0;
	//Read file and make Symbol Table
	while (strcmp(state_end->opcode, "END") != 0)
	{
		//initialize new statement node
		statePtr node = malloc(sizeof(state));
		node->buffer[0] = node->symbol[0] = node->opcode[0] = node->operand[0] = node->operand2[0] = node->object[0] = '\0';

		//read file
		fgets(buffer, 255, asmf);
		if (feof(asmf)) {
			printf("Error: EOF without END statement\n");
			return -1; //file ended
		}
		if(strcmp(buffer,"\n")==0) continue;

		//fgets(buffer, 255, asmf);
		strcpy(node->buffer, buffer);
		index = 0;

		//just comment
		if (buffer[0] == '.') {
			node->loc = -1;
			node->next = NULL;
			node->buffer[strlen(buffer) - 1] = '\0';
			state_end->next = node;
			state_end = node;
			continue;
		}

		//when there is SYMBOL, read SYMBOL
		if (buffer[0] != ' ')
		{
			for (i = 0; buffer[index] != ' '; index++, i++) {
				node->symbol[i] = buffer[index];
			}
			node->symbol[i] = '\0';
		}

		while (buffer[index++] == ' '); index--;

		//scan OPCODE
		if (buffer[index] == '\n') continue; //just blank line
		for (i = 0; (buffer[index] != ' '&&buffer[index] != '\n'); i++, index++) {
			node->opcode[i] = buffer[index];
		}
		node->opcode[i] = '\0';
		while (buffer[index++] == ' '); index--;

		//scan OPERAND
		if (buffer[index] != '\n') {
			for (i = 0; (buffer[index] != ' '&&buffer[index] != '\n'&&buffer[index] != ','); index++, i++) {
				node->operand[i] = buffer[index];
			}
			node->operand[i] = '\0';

			//scan OPERAND2
			if (buffer[index++] == ',') {
				while (buffer[index++] == ' '); index--;
				for (i = 0; (buffer[index] != ' '&&buffer[index] != '\n'); i++, index++) {
					node->operand2[i] = buffer[index];
				}
				node->operand2[i] = '\0';
			}
		}

		//insert node to statement list
		node->loc = *LOC;
		if (strcmp(node->opcode, "BASE") == 0 || strcmp(node->opcode, "END") == 0) node->loc = -1;
		node->next = NULL;
		node->buffer[strlen(buffer) - 1] = '\0';
		state_end->next = node;
		state_end = node;

		//increase LOCCTR
		if (change_loc(LOC, node, start_add) < 0) {
			printf("Error: invalid operation code\n");
			print_asm_error(linenum);
			return -1;
		}
		//if symbol exist, insert to SYMTAB
		if (node->symbol[0] != '\0' && strcmp(node->opcode,"START")!=0) {
			if (insert_symbol(node->symbol, node->loc)<0) {
				printf("Error: symbol duplicate\n");
				print_asm_error(linenum);
				return -1;
			}
		}
		linenum += 5;
		//printf("%X   %s\t%s\t%s\t%s\t\n", node->loc, node->symbol, node->opcode, node->operand, node->operand2);
	}
	if(strcmp(state_end->opcode,"END")!=0 && feof(asmf)) return -1;
	return 0;
}
/*function pass2()*/
/*To make objectcode, write lst file, write obj file*/
/*return: if there is error durint pass2, return -1 , else return 0*/
int pass2(char* lst_string, char* obj_string, statePtr state_start, statePtr state_end, int *LOC, int *start_add, int proglen)
{
	statePtr tmp;
	tmp = state_start;
	int linenum = 0;
	int oopcode, nn, ii, xx, bb, pp, ee, disp, addr, object, target;
	*LOC = 0; *start_add = 0; int BASE;
	int op_index, xbpe, value;
	opPtr find;
	char buf[100];
	char progname[10] = "TEST";
	for (tmp = state_start->next; tmp != NULL; tmp = tmp->next)
	{

		if (change_loc(LOC, tmp, start_add) < 0) {
			print_asm_error(linenum);
			return -1;
		}
		linenum += 5;

		//write object code
		object = 0x0;
		//except for comment
		if (tmp->buffer[0] == '.') {
			 continue;
		}
		//case for START, get program name
		if (strcmp(tmp->opcode, "START") == 0) {
			if (strlen(tmp->symbol) > 6) {
				printf("Error: program name is TOO LONG\n");
				print_asm_error(linenum);
				return -1;
			}
			strcpy(progname, tmp->symbol);
			*start_add = strtol(tmp->operand, NULL, 16); 
			continue;
		}
		//case for RESW, RESB
		if (strcmp(tmp->opcode, "RESW") == 0 || strcmp(tmp->opcode, "RESB") == 0) {
			continue;
		}
		//case for BASE
		if (strcmp(tmp->opcode, "BASE") == 0) {
			BASE = find_symbol(tmp->operand); continue;
		}
		//case for END
		if (strcmp(tmp->opcode, "END") == 0) {
			break;
		}

		//FORMAT 4 with+
		if (tmp->opcode[0] == '+') {
			bb = pp = 0; ee = 1;
			find = find_opcode(&(tmp->opcode[1]));
			if (find == NULL) {
				print_asm_error(linenum);
				break;
			}
			oopcode = find->opcode;

			get_nix(tmp->operand, tmp->operand2, &nn, &ii, &xx, &op_index);
			addr = find_symbol(&(tmp->operand[op_index]));
			//if there is no SYMBOL
			if (addr < 0) {
				//NUM rather than SYMBOL
				if (sscanf(&(tmp->operand[op_index]), "%d", &value)) {
					addr = value;
				}
				//No matching SYMBOL
				else {
					printf("Error: invalid operand\n");
					print_asm_error(linenum); return -1;
				}
			}
			object += oopcode * 0x1000000;
			object += (2 * nn + ii) * 0x1000000;
			object += (0b1000 * xx + 0b100 * bb + 0b10 * pp + 0b1 * ee) * 0x100000;
			object += addr;
			sprintf(tmp->object, "%08X", object);
		}
		//BYTE
		else if (strcmp(tmp->opcode, "BYTE") == 0) {
			if (tmp->operand[0] == 'C') {
				for (int k = 2; tmp->operand[k] != '\''; k++) {
					sprintf(buf, "%02X", tmp->operand[k]);
					strcat(tmp->object, buf);
				}
			}
			else if (tmp->operand[0] == 'X') {
				char *hex;
				hex = strtok(tmp->operand, "'");
				hex = strtok(NULL, "'");
				value = atoi(hex);
				sprintf(tmp->object, "%s", hex);
			}
		}
		else if (strcmp(tmp->opcode, "WORD") == 0) {
			sscanf(tmp->operand, "%d", &value);
			sprintf(tmp->object, "%06X", value);
		}
		//1Çü½Ä, 2Çü½Ä, 3Çü½Ä
		else {
			find = find_opcode(tmp->opcode);
			if (find == NULL) {
				print_asm_error(linenum);
				return -1;
			}
			//1Çü½Ä
			if (find->format == 1)
			{
				sprintf(tmp->object, "%02X", find->opcode);
			}
			//2Çü½Ä
			else if (find->format == 2)
			{
				int reg1 = 0, reg2 = 0;
				reg1 = get_regnum(tmp->operand[0]);
				reg2 = get_regnum(tmp->operand2[0]);

				sprintf(tmp->object, "%02X%01X%01X", find->opcode, reg1, reg2);
			}


			//3Çü½Ä
			else {
				oopcode = find->opcode;
				get_nix(tmp->operand, tmp->operand2, &nn, &ii, &xx, &op_index); ee = 0;
				target = find_symbol(&(tmp->operand[op_index]));
				//if there is SYMBOL
				if (target > 0) {
					if ((target - *LOC >= -2048) && (target - *LOC <= 2047)) {
						//pc relative
						disp = target - *LOC;
						bb = 0; pp = 1;
					}
					else if ((target - BASE >= 0) && (target - BASE <= 4095)) {
						//base relative
						disp = target - BASE;
						bb = 1; pp = 0;
					}
					else {
						//format 4
						if (strcmp(tmp->opcode, "RSUB") != 0) {
							bb = pp = 0; ee = 1;
							object += oopcode * 0x1000000;
							object += (2 * nn + ii) * 0x1000000;
							object += (0b1000 * xx + 0b100 * bb + 0b10 * pp + 0b1 * ee) * 0x100000;
							object += target;
							sprintf(tmp->object, "%08X", object);
							continue;
						}
					}
				}
				//if there is NUM rather than SYMBOL
				else if (sscanf(&(tmp->operand[op_index]), "%d", &value)) {
					disp = value; bb = 0; pp = 0;
				}
				//No Symbol
				else {
					printf("Error: invalid variable %s\n", &(tmp->operand[op_index]));
					print_asm_error(linenum); return -1;
				}

				//case for RSUB
				if (strcmp(tmp->opcode, "RSUB") == 0) {
					disp = 0;
				}
				oopcode += (2 * nn + ii);
				xbpe = (0b1000 * xx + 0b100 * bb + 0b10 * pp + 0b1 * ee);

				if (disp < 0) disp = disp & 0x0fff;

				sprintf(tmp->object, "%02X%01X%03hX", oopcode, xbpe, disp);
			}
		}
		
	}	

	//write lstfile
	FILE *lstf = fopen(lst_string, "w");
	
	linenum = 0;
	for (tmp = state_start->next; tmp != NULL; tmp = tmp->next)
	{
		linenum += 5;
		//write linenum, LOC, statement
		fprintf(lstf, "%4d\t", linenum);
		if (tmp->loc >= 0) fprintf(lstf, "%04X\t", tmp->loc);
		else fprintf(lstf, "\t");
		fprintf(lstf, "%-25s", tmp->buffer);
		//if comment or START or RESW or RESB don't print object code
		if (tmp->buffer[0] == '.' || strcmp(tmp->opcode, "START") == 0 || strcmp(tmp->opcode, "RESW") == 0 || strcmp(tmp->opcode, "RESB") == 0 \
			|| strcmp(tmp->opcode, "BASE") == 0) {
			fprintf(lstf, "\n"); continue;
		}
		//if meet END stop writing object code
		if (strcmp(tmp->opcode, "END") == 0) {
			fprintf(lstf, "\n"); break;
		}
		fprintf(lstf, "%s\n", tmp->object);
	}
	fclose(lstf);


	//write objfile
	FILE *objf = fopen(obj_string, "w");
	int objlen = 0;
	int objflg = 0;
	int objstart = 0;
	char objbuf[300];
	objbuf[0] = '\0';
	//H Record
	tmp = state_start->next;
	if (strcmp(tmp->opcode, "START") == 0) {
		fprintf(objf, "H%-6s%06X%06X\n", tmp->symbol, *start_add, proglen);
	}
	//  -Not start with "START"
	else {
		fprintf(objf, "H%-6s%06X%06X\n", progname, *start_add, proglen);
		tmp = state_start;
	}
	objstart = *start_add;
	//make object file
	while (1)
	{
		tmp = tmp->next;
		//if meet END, print all records in the buffer
		if (strcmp(tmp->opcode, "END") == 0) {
			fprintf(objf, "T%06X%02X%s\n", objstart, objlen, objbuf);
			break;
		}
		//if meet RESW or RESB, change the line
		else if (strcmp(tmp->opcode, "RESW") == 0 || strcmp(tmp->opcode, "RESB") == 0) {
			objflg = -1;
		}
		else if (tmp->object[0] == '\0') continue;
		//if record length exceeds 0x1D change the line
		else if (objlen + strlen(tmp->object) / 2 > 0x1D) objflg = -1;
		//when the algorithm change the line
		if (objflg == -1) {
			if ((strcmp(tmp->opcode, "RESW") == 0 || strcmp(tmp->opcode, "RESB") == 0)) {
				if (objbuf[0] == '\0') continue;
			}
			//print remain records in the buffer
			if (objbuf[0] != '\0') {
				fprintf(objf, "T%06X%02X%s\n", objstart, objlen, objbuf);
			}
			strcpy(objbuf, "\0");
			objflg = 0; objstart = tmp->loc; objlen = strlen(tmp->object) / 2;
			strcat(objbuf, tmp->object);
			continue;
		}
		//strcat the object code to the buffer and increase objlen
		else if (objflg == 0) {
			strcat(objbuf, tmp->object);
			objlen += strlen(tmp->object) / 2;
		}
	}

	//M Record
	for (tmp = state_start->next; tmp != NULL; tmp = tmp->next)
	{
		if (strncmp(tmp->object, "4B1", 3) == 0) {
			fprintf(objf, "M%06X%02X\n", tmp->loc + 1, 5);
		}
	}

	//E Record
	fprintf(objf, "E%06X\n", *start_add);

	fclose(objf);



	return 0;
}

/*start of program*/
int main(void)
{
	char command[MAX_MNE];
	int i;
	//initial history list
	hisPtr first, last;
	first = malloc(sizeof(hisNode));
	first->num = 0;
	last = first;

	//initialize memory
	char mem[MB_SIZE];
	reset_memory(mem);

	//initial opcode file
	FILE *fp = fopen("opcode.txt","r");
	if(fp == NULL){
		printf("Error: File Open Error!\n");
		return -1;
	}

	//initial hashtable
	for (i = 0; i < MAX_HASH; i++)	hashtable[i] = NULL;
	
	//make opcode hashtable
	make_opcodetable(fp);

	print_prompt();
	
	//initialize variables for functions below
	int dump_start, dump_end, dump_line=0x00;
	int input_error_flag;
	char x, y;

	int edit_address, edit_value;
	int fill_start, fill_end, fill_value;

	char op_str[MAX_MNE];
	char typefile[100];

	//repeat prompt
	while(scanf("%s",command)!=EOF)
	{
		switch(getcom(command,&last)) {
		case Error: break;  
		case Help: print_help();break;   //push history at getcom()
		case Dir: print_dir(); break;    //push history at getcom()
		case Quit: exit(1);break;   //push history at getcom()
		case History: print_history(first);break;   //push history at getcom()
		case Dump:{   //print dump and push history
			input_error_flag = 0;
			x = getchar();
			dump_start = dump_line;
			dump_end = dump_start+160-1;
	
			if(!(x == '\n' )){
				scanf("%x",&dump_start);

				//if dump start point doesn't math boundary print error
				if(dump_start<0x00 || dump_start>0xfffff) {
					input_error_flag = 1;
					while((getchar() != '\n'));
				} 
			
				
				//if dump start point is valid
				dump_end = dump_start+160-1;
				if(!input_error_flag){
					while(1)
					{
						y = getchar();
						//command only has starting point

						if(y=='\n'){
							 push_node(&last, command, 1, dump_start);
						 	break;
						}
						//command has both start and end point
						else if(y==','){
							scanf("%x",&dump_end);	
							if(dump_start>dump_end) input_error_flag = 1;
							else {
								push_node(&last, command,2, dump_start, dump_end); 
							}
							break;
						}
						else if(y==' ' || y=='\t') continue;
						//input error: don't have ','
						else{
							input_error_flag = 1;
							while((getchar() !='\n'));
							break;
						}
					
					}
				}
			}
			//command has no dump index
			else{
				if(dump_start<0x00 || dump_start>0xfffff){
					input_error_flag = 1;
				}
				else{ 
					push_node(&last, command, 0);
				}
			}

			if(input_error_flag) print_input_error();
			else{
				//dump do not exceed the end of memory
				if(dump_end>0xfffff) dump_end = 0xfffff;
				print_dump(mem, dump_start, dump_end);
			}
			//increase the dump line
			dump_line = dump_end+1;
			break;
		}
		case Edit:{
			//edit memory and push to history
			if(edit_memory(mem, &edit_address, &edit_value)>-1)
				push_node(&last, command, 2, edit_address, edit_value);
			else print_input_error();
			break;
		}
		case Fill:{
			//fill memory and push to history
			if(fill_memory(mem, &fill_start, &fill_end, &fill_value)>-1)
				push_node(&last, command, 3, fill_start, fill_end, fill_value);
			else print_input_error();
			break;
		}
		case Reset:{
			//reset memory
			reset_memory(mem);break;
		}
		case Opcode:{
			//print opcode and push to history
			if (print_opcode(op_str) > -1)
				push_node(&last, command, 4, 0, 0, 0, op_str);
			else printf("Error: mnemonic is not exist\n");
			break;
		}
		case Opcodelist:{
			//print opcode hashtable. push history at getcmd()
			print_opcodetable();
			break;
		}
		case Type:{
			if(type_file(typefile)>-1)
				push_node(&last, command, 4, 0, 0, 0, typefile);
			break;
		}
		case Assemble:{
			char file_string[100]; //input file name
			scanf("%s", file_string);
			if (strcmp(&(file_string[strlen(file_string) - 4]), ".asm") != 0) {
				printf("Error: please input asm file\n"); break;
			}
			//open asm file to read
			FILE *asmf = fopen(file_string, "r");
			if (asmf == NULL) {
				printf("Error: file not exist\n");
				break;
			}
			push_node(&last, command, 4, 0, 0, 0, file_string);
			//initialize address variables
			int LOC = 0, start_add = 0, proglen;

			//initialize symbolTable
			free(symTable);
			symTable = malloc(sizeof(symPtr) * MAX_SYMHASH);
			for (i = 0; i<MAX_SYMHASH; i++)	symTable[i] = NULL;

			//initialize state List
			statePtr state_start, state_end;
			state_start = malloc(sizeof(state));
			state_start->opcode[0] = '\0';
			state_end = state_start;

			//pass1
			if (pass1(asmf, state_start, state_end, &LOC, &start_add) < 0) break;
			proglen = LOC - start_add;

			//open lst file to write
			char lst_string[100]; char obj_string[100];
			strncpy(lst_string, file_string, strlen(file_string) - 4);
			lst_string[strlen(file_string) - 4] = '\0';
			strcpy(obj_string, lst_string);
			strcat(lst_string, ".lst");
			strcat(obj_string, ".obj");

			//pass2->write to lstf
			if (pass2(lst_string, obj_string, state_start, state_end, &LOC, &start_add, proglen) < 0) break;


			printf("\toutput file : [%s], [%s]\n", lst_string, obj_string);
			break;
		}
		case Symbol:{
			if(print_symbol()<0) break;
		}
		}	
		print_prompt();
	
	}	
	fclose(fp);
	return 0;	
}
