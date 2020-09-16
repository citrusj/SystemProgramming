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
	if(args == 4) sprintf(tmp->mnemonic, "%s", va_arg(argp,char *));
	
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
			printf("%s",tmp->mnemonic);
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
	printf("\nh[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n\n");
	return;
}	

/*function: print_prompt()*/
/*To pring prompt message*/
/*return: nothing*/
void print_prompt(void)
{
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
void make_opcodetable(FILE *fp, opPtr* hashtable)
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
		for(c=fgetc(fp);(c==' '||c=='\t');c=fgetc(fp));
		if(c==EOF) break;
		for(;(c!=' ');c=fgetc(fp)){
			if(c=='\t') break;
			node->mnemonic[i++] = c;
		}
		node->mnemonic[i] =  '\0';
		fscanf(fp, "%d",&up);
		if((c=fgetc(fp))=='/'){
			fscanf(fp, "%d",&down);
			c=fgetc(fp);
		}
		
		node->value = (double)up/down;
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
int print_opcode(opPtr* hashtable, char* op_str)
{	
	opPtr op_node;
	//error: no mnemonic
	if(getchar() == '\n') return -1;
	
	//print mnemonic with opcode
	scanf("%s",op_str);
	for(op_node = hashtable[get_hashkey(op_str)];op_node!=NULL;op_node = op_node->next)
	{
		if(strcmp(op_node->mnemonic, op_str) == 0){
			printf("opcode is %x\n", op_node->opcode);break;
		}
	}
	//when there is no mnemonic in hashtable
	if(op_node == NULL) return -1;
	
	return 0;
}

/*function: print_opcodetable()*/
/*To print hashtable of opcode-mnemonic*/
/*return: nothing*/
void print_opcodetable(opPtr* hashtable)
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
	opPtr hashtable[MAX_HASH];
	for(i=0;i<MAX_HASH;i++)	hashtable[i] = NULL;
	
	//make opcode hashtable
	make_opcodetable(fp, hashtable);


	print_prompt();
	
	//initialize variables for functions below
	int dump_start, dump_end, dump_line=0x00;
	int input_error_flag;
	char x, y;

	int edit_address, edit_value;
	int fill_start, fill_end, fill_value;

	char op_str[MAX_MNE];
	
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
			if(print_opcode(hashtable, op_str)>-1)
				push_node(&last, command, 4, 0,0,0,op_str);
			else printf("Error: mnemonic is not exist\n");
			break;
		}
		case Opcodelist:{
			//print opcode hashtable. push history at getcmd()
			print_opcodetable(hashtable);
			break;
		}

		}
	
		print_prompt();
	}
		
	fclose(fp);
	return 0;	
}
