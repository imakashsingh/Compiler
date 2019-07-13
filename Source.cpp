#define _CRT_SECURE_NO_WARNINGS
#include "compiler_header.h"


int main()
{
	stackk stack_arr;
	stack_arr.top = 0;
	symbol_table *st = NULL;
	block_table *bt = NULL;
	intercode *ic = NULL;
	int instructions_read = 0;
	int *memory = (int*)calloc(512 , sizeof(int));

	FILE *fp = fopen(INPUT_FILE, "r");
	if (fp == NULL)
	{
		printf("Could not open the file %s\n",INPUT_FILE);
		return 0;
	}
	char input_command[100];
	while (fgets(input_command,100,fp) != NULL)
	{
		parse_command(input_command,&st,&ic,&instructions_read,&bt,memory,&stack_arr);
	}
	fclose(fp);
	
	write_to_file(ic,st,bt);

	system("pause");
	return 0;

}