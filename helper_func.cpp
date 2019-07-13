#define _CRT_SECURE_NO_WARNINGS
#include "compiler_header.h"


int find_operand_code(char *str,symbol_table **st)
{
	int arr_index = -1;
	char arr_str[15];
	int arr_str_len = 0;
	char registers[8][3] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };
	char opcode[14][8] = { "MOV", "MOV", "ADD", "SUB", "MUL", "JMP", "IF", "EQ", "LT", "GT", "LTEQ", "GTEQ", "PRINT", "READ" };
	for (int i = 0; i < 8; i++)
	{
		if (strcmp(registers[i], str) == 0)
			return i;
	}
	
	for (int i = 0; i < 14; i++)
	{
		if (strcmp(opcode[i], str) == 0)
			return i+1;
	}

	int i;
	for (i = 0; i < strlen(str); i++)
	{
		if (str[i] == '[')
			break;
	}
	if (i < strlen(str) - 1)
	{
		int j = 0;
		for (; j < i; j++)
			arr_str[arr_str_len++] = str[j];
		arr_str[arr_str_len] = '\0';
		j = i+1;
		arr_index = 0;
		while (str[j] >= 48 && str[j] <= 57)
		{
			arr_index = (arr_index * 10) + (str[j] - '0');
			j++;
		}
		strcpy(str, arr_str);
	}
	symbol_table *ptr = *st;
	while (ptr != NULL)
	{
		if (strcmp(str, ptr->name) == 0)
		{
			if (arr_index == -1)
			{
				return ptr->address;
			}
			else
			{
				return (ptr->address) + arr_index;
			}
		}
		ptr = ptr->next;
	}
	return -1;
}

void parse_command(char *input_command, symbol_table** st, intercode** ic, int *instructions_read, block_table **bt, int* memory, stackk* stack_arr)
{
	bool space_chk = false;
	char *parsed_str = (char*)malloc(100 * sizeof(char));
	int parse_str_ptr = 0;
	int i = 0;
	while (input_command[i] == ' ' && i < strlen(input_command))
	{
		i++;
	}
	for (; i < strlen(input_command); i++)
	{
		if ((input_command[i] == ' ') && (space_chk == false))
		{
			parsed_str[parse_str_ptr++] = input_command[i];
			space_chk = true;
		}
		else if ((input_command[i] == ' ') && (space_chk))
			continue;
		else
		{
			parsed_str[parse_str_ptr++] = input_command[i];
		}
	}
	parsed_str[parse_str_ptr] = '\0';

	char first_part[10], second_part[15];
	i = 0;
	
	while (parsed_str[i] != ' ' && parsed_str[i] != '\n' && i < strlen(parsed_str))
	{
		first_part[i] = parsed_str[i];
		i++;
	}
	first_part[i++] = '\0';
	
	int k = 0;
	
	if (strcmp(first_part, "IF") == 0)
	{
		int itr = 0;
		while (input_command[itr] == ' ' && itr < strlen(input_command))
		{
			itr++;
		}
		while (input_command[itr] != ' ')
		{
			itr++;
		}
		while (input_command[itr] == ' ' && itr < strlen(input_command))
		{
			itr++;
		}
		for (; itr < strlen(input_command); itr++)
			second_part[k++] = input_command[itr];
		second_part[k-5] = '\0';
	}
	else
	{
		while (i < strlen(parsed_str) && parsed_str[i] != '\n')
		{
			second_part[k++] = parsed_str[i++];
		}
		second_part[k] = '\0';
	}

	tokenize(first_part, second_part,st,ic,instructions_read,bt,memory,stack_arr);
}

void tokenize(char *first_part, char *remaining_part, symbol_table** st, intercode **ic, int* instructions_read, block_table **bt, int* memory, stackk* stack_arr)
{
	char opcode[14][8] = { "MOV", "MOV", "ADD", "SUB", "MUL", "JMP", "IF", "EQ", "LT", "GT", "LTEQ", "GTEQ", "PRINT", "READ" };
	char registers[8][3] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };
	
	char data_var[2][8] = { "DATA","CONST"};

	if ((first_part[strlen(first_part) - 1] == ':') && (strcmp(first_part, "START:") != 0))
		process_label(first_part,instructions_read,bt);
	else if ((strcmp(first_part, "ADD") == 0) || (strcmp(first_part, "SUB") == 0) || (strcmp(first_part, "MUL") == 0))
		process_arith(first_part, remaining_part, st, ic, instructions_read, bt, memory);
	else if ((strcmp(first_part, "DATA") == 0) || (strcmp(first_part, "CONST") == 0))
		process_data(first_part, remaining_part, st, ic, instructions_read, bt, memory);
	else if (strcmp(first_part, "MOV") == 0)
		process_mov(remaining_part, st, ic, instructions_read, bt, memory);
	else if ((strcmp(first_part, "PRINT") == 0) || (strcmp(first_part, "READ") == 0))
		process_io(first_part, remaining_part, st, ic, instructions_read, bt, memory);
	else if (strcmp(first_part, "IF") == 0)
		process_if(remaining_part, st, ic, instructions_read, bt, stack_arr);
	else if (strcmp(first_part, "ELSE") == 0)
		process_else(ic, bt, instructions_read, stack_arr);
	else if (strcmp(first_part, "ENDIF") == 0)
		process_endif(ic, bt, instructions_read, stack_arr);
	else if (strcmp(first_part, "JUMP") == 0)
		process_jump(remaining_part, ic, instructions_read, bt);
}

void process_io(char *first_part, char *remaining_part, symbol_table** st, intercode **ic, int* instructions_read, block_table **bt, int* memory)
{
	int operation_code = -1;
	if (strcmp(first_part, "PRINT") == 0)
		operation_code = 13;
	else
		operation_code = 14;
	int i = 0, temp_str_len = 0,op = -1;
	char temp_str[15];
	while (i <= strlen(remaining_part))
	{
		if (i == strlen(remaining_part))
		{
			temp_str[temp_str_len] = '\0';
			op = find_operand_code(temp_str,st);
		}
		else
		{
			if (remaining_part[i] != ' ')
				temp_str[temp_str_len++] = remaining_part[i];
		}
		i++;
	}

	intercode *newnode = (intercode *)malloc(sizeof(intercode));
	newnode->line_no = *(instructions_read)+1;
	newnode->operation = operation_code;
	newnode->op1 = op;
	newnode->op2 = -1;
	newnode->op3 = -1;
	newnode->jump_line = -1;
	*(instructions_read) += 1;

	if (*ic == NULL)
	{
		*ic = newnode;
		newnode->next = NULL;
	}
	else
	{
		intercode *ptr = *ic;
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		ptr->next = newnode;
		newnode->next = NULL;
	}
}

void process_arith(char* first_name, char* rem_part, symbol_table** st, intercode **ic, int* instructions_read, block_table **bt, int* memory)
{
	int operation_code = 0;
	if (strcmp(first_name, "ADD") == 0)
		operation_code = 3;
	else if (strcmp(first_name, "SUB") == 0)
		operation_code = 4;
	else if (strcmp(first_name, "MUL") == 0)
		operation_code = 5;

	int temp_op[3],temp_str_len = 0,count = 0;
	char temp_str[15];
	for (int i = 0; i <= strlen(rem_part); i++)
	{
		if (rem_part[i] == ',' || i == strlen(rem_part))
		{
			temp_str[temp_str_len] = '\0';
			temp_op[count++] = find_operand_code(temp_str,st);
			temp_str_len = 0;
		}
		else if (rem_part[i] != ' ')
		{
			temp_str[temp_str_len++] = rem_part[i];
		}
	}

	intercode *newnode = (intercode *)malloc(sizeof(intercode));
	newnode->line_no = *(instructions_read)+1;
	newnode->operation = operation_code;
	newnode->op1 = temp_op[0];
	newnode->op2 = temp_op[1];
	newnode->op3 = temp_op[2];
	newnode->jump_line = -1;
	*(instructions_read) += 1;

	if (*ic == NULL)
	{
		*ic = newnode;
		newnode->next = NULL;
	}
	else
	{
		intercode *ptr = *ic;
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		ptr->next = newnode;
		newnode->next = NULL;
	}
}

void process_mov(char* rem_part, symbol_table** st, intercode **ic, int *instructions_read, block_table **bt, int* memory)
{
	char registers[8][3] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };
	int op_arr[2];
	char temp_str[15];
	int flag = 0, temp_str_len = 0,i=0,count = 0;
	while (i <= strlen(rem_part))
	{
		if (rem_part[i] == ',' || i == strlen(rem_part))
		{
			temp_str[temp_str_len] = '\0';
			op_arr[count++] = find_operand_code(temp_str,st);
			temp_str_len = 0;
			if (count == 1)
			{
				for (int z = 0; z < 8; z++)
				{
					if (strcmp(temp_str, registers[z]) == 0)
					{
						flag = 1;
						break;
					}
				}
			}
		}
		else
		{
			if (rem_part[i] != ' ')
				temp_str[temp_str_len++] = rem_part[i];
		}
		i++;
	}
	if (*ic == NULL)
	{
		intercode *newnode = (intercode*)malloc(sizeof(intercode));
		newnode->line_no = (*instructions_read) + 1;
		newnode->operation = flag + 1;
		newnode->op1 = op_arr[0];
		newnode->op2 = op_arr[1];
		newnode->op3 = -1;
		newnode->jump_line = -1;
		newnode->next = NULL;
		*ic = newnode;
		*(instructions_read) += 1;
	}
	else
	{
		intercode *ptr = *ic;
		intercode *newnode = (intercode*)malloc(sizeof(intercode));
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		newnode->line_no = (*instructions_read) + 1;
		newnode->operation = flag + 1;
		newnode->op1 = op_arr[0];
		newnode->op2 = op_arr[1];
		newnode->op3 = -1;
		newnode->jump_line = -1;
		newnode->next = NULL;
		*(instructions_read) += 1;
		ptr->next = newnode;
	}
}

void process_data(char* first_part, char* remaining_part, symbol_table** st, intercode** ic, int* instructions_read,block_table **bt, int* memory)
{
	if (strcmp(first_part, "DATA") == 0)
	{
		char local_buff[10];
		int local_buff_len = 0;
		int ssum = 0;
		int ssize = 0;
		if (remaining_part[strlen(remaining_part)-1] == ']')
		{
			int k = 0;
			while (remaining_part[k] != '[')
			{
				local_buff[local_buff_len++] = remaining_part[k++];
			}
			local_buff[local_buff_len++] = '\0';
			while (remaining_part[k] != ']' && (k<strlen(remaining_part)))
			{
				if (remaining_part[k] >= 48 && remaining_part[k] <= 57)
					ssize = ((ssize * 10) + (remaining_part[k] - '0'));
				k++;
			}
		}
		else
		{
			int k = 0;
			while (k < strlen(remaining_part))
			{
				local_buff[local_buff_len++] = remaining_part[k++];
				ssize = 1;
			}
			local_buff[local_buff_len] = '\0';
		}
		if (*st == NULL)
		{
			symbol_table *newnode = (symbol_table *)malloc(sizeof(symbol_table));
			strcpy(newnode->name, local_buff);
			newnode->size = ssize;
			newnode->address = 0;
			newnode->next = NULL;
			*st = newnode;
		}
		else
		{
			symbol_table *newnode = (symbol_table *)malloc(sizeof(symbol_table));
			symbol_table *pptr = *st;
			while (pptr->next != NULL)
			{
				pptr = pptr->next;
			}
			if (pptr->size == 0)
				newnode->address = pptr->address + 1;
			else
				newnode->address = (pptr->address)+(pptr->size);
			newnode->size = ssize;
			strcpy(newnode->name, local_buff);
			newnode->next = NULL;
			pptr->next = newnode;
		}
	}
	else if (strcmp(first_part, "CONST") == 0)
	{
		int i = 0, temp_str_len = 0,val = 0;
		char temp_str[15];
		while (remaining_part[i] != '=')
		{
			if (remaining_part[i] != ' ')
				temp_str[temp_str_len++] = remaining_part[i];
			i++;
		}
		temp_str[temp_str_len] = '\0';
		while (i<strlen(remaining_part))
		{
			if (remaining_part[i] >= 48 && remaining_part[i] <= 57)
				val = (val * 10) + remaining_part[i] - '0';
			i++;
		}
		if (*st == NULL)
		{
			symbol_table *newnode = (symbol_table *)malloc(sizeof(symbol_table));
			strcpy(newnode->name, temp_str);
			newnode->size = 0;
			newnode->address = 0;
			newnode->next = NULL;
			*st = newnode;
			memory[0] = val;
		}
		else
		{
			symbol_table *newnode = (symbol_table *)malloc(sizeof(symbol_table));
			symbol_table *pptr = *st;
			while (pptr->next != NULL)
			{
				pptr = pptr->next;
			}
			if (pptr->size == 0)
				newnode->address = pptr->address + 1;
			else
				newnode->address = (pptr->address) + (pptr->size);
			newnode->size = 0;
			strcpy(newnode->name, temp_str);
			newnode->next = NULL;
			pptr->next = newnode;
			memory[newnode->address] = val;
		}
	}
}

void process_label(char* first_part, int* instructions_read, block_table** bt)
{
	first_part[strlen(first_part) - 1] = '\0';
	
	block_table *newnode = (block_table*)malloc(sizeof(block_table));
	strcpy(newnode->block_name, first_part);
	newnode->address = *(instructions_read)+1;
	newnode->next = NULL;
	if (*bt == NULL)
	{
		*bt = newnode;
	}
	else
	{
		block_table *ptr = *bt;
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		ptr->next = newnode;
	}
}

void process_if(char *remaining_part, symbol_table** st, intercode **ic, int *instructions_read, block_table **bt, stackk *stack_arr)
{
	char temp_str[15];
	int op_array[3];
	int op_arr_len = 0,itr = 0,temp_str_len = 0;
	while (itr < strlen(remaining_part))
	{
		if (remaining_part[itr] == ' ')
		{
			temp_str[temp_str_len] = '\0';
			op_array[op_arr_len++] = find_operand_code(temp_str,st);
			temp_str_len = 0;
			itr++;
		}
		else
		{
			temp_str[temp_str_len++] = remaining_part[itr++];
		}
	}

	intercode *newnode = (intercode*)malloc(sizeof(intercode));
	newnode->line_no = *(instructions_read)+1;
	newnode->op1 = -1;
	newnode->op2 = -1;
	newnode->op3 = -1;
	newnode->jump_line = -1;
	newnode->operation = 7;
	for (itr = 0; itr < op_arr_len; itr++)
	{
		if (itr == 0)
			newnode->op1 = op_array[0];
		else if (itr == 1)
			newnode->op2 = op_array[1];
		else if (itr == 2)
			newnode->op3 = op_array[2];
	}
	newnode->next = NULL;
	if (*ic == NULL)
	{
		*ic = newnode;
	}
	else
	{
		intercode *ptr = *ic;
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		ptr->next = newnode;
	}
	*(instructions_read) += 1;
	stack_arr->arr[stack_arr->top] = *instructions_read;
	stack_arr->top += 1;
}

void process_else(intercode **ic, block_table **bt, int* instructions_read, stackk *stack_arr)
{
	intercode *newnode = (intercode*)malloc(sizeof(intercode));
	newnode->line_no = *(instructions_read)+1;
	newnode->operation = 6;
	newnode->op1 = -1;
	newnode->op2 = -1;
	newnode->op3 = -1;
	newnode->jump_line = -1;
	newnode->next = NULL;
	if (*ic == NULL)
	{
		*ic = newnode;
	}
	else
	{
		intercode *ptr = *ic;
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		ptr->next = newnode;
	}
	*(instructions_read) += 1;
	
	block_table *block_node = (block_table *)malloc(sizeof(block_table));
	strcpy(block_node->block_name, "ELSE");
	block_node->address = *(instructions_read)+1;
	block_node->next = NULL;
	if (*bt == NULL)
	{
		*bt = block_node;
	}
	else
	{
		block_table *bptr = *bt;
		while (bptr->next != NULL)
		{
			bptr = bptr->next;
		}
		bptr->next = block_node;
	}

	stack_arr->arr[stack_arr->top] = *instructions_read;
	stack_arr->top += 1;
}

void process_endif(intercode **ic, block_table **bt, int* instructions_read, stackk *stack_arr)
{
	int stk_top = stack_arr->arr[(stack_arr->top) - 1];
	stack_arr->top -= 1;

	block_table *block_node = (block_table*)malloc(sizeof(block_table));
	strcpy(block_node->block_name, "ENDIF");
	block_node->address = *(instructions_read)+1;
	block_node->next = NULL;
	if (*bt == NULL)
	{
		*bt = block_node;
	}
	else
	{
		block_table *bptr = *bt;
		while (bptr->next != NULL)
		{
			bptr = bptr->next;
		}
		bptr->next = block_node;
	}

	intercode *ptr = *ic;
	while (ptr->line_no != stk_top)
	{
		ptr = ptr->next;
	}

	if (ptr->operation == 6)
	{
		int else_line_no = ptr->line_no;
		ptr->jump_line = *(instructions_read)+1;
		stk_top = stack_arr->arr[(stack_arr->top) - 1];
		stack_arr->top -= 1;
		ptr = *ic;
		while (ptr->line_no != stk_top)
		{
			ptr = ptr->next;
		}
		ptr->jump_line = else_line_no + 1;
	}
	else if (ptr->operation == 7)
	{
		ptr->jump_line = *(instructions_read)+1;
	}
}

void process_jump(char *rem_part, intercode **ic, int *instructions_read, block_table **bt)
{
	block_table *bptr = *bt;
	while (strcmp(bptr->block_name, rem_part) != 0)
	{
		bptr = bptr->next;
	}
	int jump_line = bptr->address;

	intercode *newnode = (intercode *)malloc(sizeof(intercode));
	newnode->line_no = *(instructions_read)+1;
	newnode->operation = 6;
	newnode->op1 = -1;
	newnode->op2 = -1;
	newnode->op3 = -1;
	newnode->jump_line = jump_line;
	*(instructions_read) += 1;
	newnode->next = NULL;

	if (*ic == NULL)
	{
		*ic = newnode;
	}
	else
	{
		intercode *ptr = *ic;
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		ptr->next = newnode;
	}
}

void write_to_file(intercode *ic, symbol_table *st, block_table *bt)
{
	FILE *op = fopen(OUTPUT_FILE, "w+");
	if (op == NULL)
	{
		printf("Could not open the file %s\n", OUTPUT_FILE);
		return;
	}

	fputs("OpCode\n", op);
	fputs("\n", op);
	intercode *iptr = ic;
	while (iptr != NULL)
	{
		char local_buff[20];
		int local_len = 0;
		char temp[20];
		_itoa(iptr->line_no, temp, 10);
		for (int i = 0; i < strlen(temp); i++)
			local_buff[local_len++] = temp[i];
		local_buff[local_len++] = ' ';
		_itoa(iptr->operation, temp, 10);
		for (int i = 0; i < strlen(temp); i++)
			local_buff[local_len++] = temp[i];
		local_buff[local_len++] = ' ';

		if (iptr->op1 != -1)
		{
			_itoa(iptr->op1, temp, 10);
			for (int i = 0; i < strlen(temp); i++)
				local_buff[local_len++] = temp[i];
			local_buff[local_len++] = ' ';
		}
		if (iptr->op2 != -1)
		{
			_itoa(iptr->op2, temp, 10);
			for (int i = 0; i < strlen(temp); i++)
				local_buff[local_len++] = temp[i];
			local_buff[local_len++] = ' ';
		}
		if (iptr->op3 != -1)
		{
			_itoa(iptr->op3, temp, 10);
			for (int i = 0; i < strlen(temp); i++)
				local_buff[local_len++] = temp[i];
			local_buff[local_len++] = ' ';
		}
		if (iptr->jump_line != -1)
		{
			_itoa(iptr->jump_line, temp, 10);
			for (int i = 0; i < strlen(temp); i++)
				local_buff[local_len++] = temp[i];
			local_buff[local_len++] = ' ';
		}

		local_buff[local_len++] = '\n';
		local_buff[local_len] = '\0';

		fputs(local_buff, op);

		iptr = iptr->next;
	}
	
	fputs("\n", op);
	fputs("Symbol Table\n", op);
	fputs("\n", op);
	fputs("Name  Address  Size\n", op);
	symbol_table *sptr = st;
	while (sptr != NULL)
	{

		char local_buff[100];
		int local_len = 0;
		char temp[50];
		
		strcpy(temp, sptr->name);
		for (int i = 0; i < strlen(temp); i++)
			local_buff[local_len++] = temp[i];
		for (int i = 0; i < 6; i++)
			local_buff[local_len++] = ' ';
		_itoa(sptr->address, temp, 10);
		for (int i = 0; i < strlen(temp); i++)
			local_buff[local_len++] = temp[i];
		for (int i = 0; i < 8; i++)
			local_buff[local_len++] = ' ';
		_itoa(sptr->size, temp, 10);
		for (int i = 0; i < strlen(temp); i++)
			local_buff[local_len++] = temp[i];
		local_buff[local_len++] = '\n';
		local_buff[local_len] = '\0';

		fputs(local_buff, op);
		sptr = sptr->next;
	}

	fputs("\n", op);
	fputs("Block Table\n", op);
	fputs("\n", op);
	fputs("BlockName  Address\n", op);
	block_table *bptr = bt;
	while (bptr != NULL)
	{

		char local_buff[100];
		int local_len = 0;
		char temp[50];

		strcpy(temp, bptr->block_name);
		for (int i = 0; i < strlen(temp); i++)
			local_buff[local_len++] = temp[i];
		for (int i = 0; i < 10; i++)
			local_buff[local_len++] = ' ';
		_itoa(bptr->address, temp, 10);
		for (int i = 0; i < strlen(temp); i++)
			local_buff[local_len++] = temp[i];
		local_buff[local_len++] = '\n';
		local_buff[local_len] = '\0';

		fputs(local_buff, op);
		bptr = bptr->next;
	}

	fclose(op);

}




/*intercode* resize_ic(intercode_metadata* icm,intercode *ic)
{
int new_size = (icm->ic_capacity) + (icm->load_factor);
icm->ic_capacity = new_size;
intercode *temp = (intercode *)malloc(new_size * sizeof(intercode));
for (int i = 0; i <icm->ic_count ; i++)
{
temp->line_no = ic->line_no;
strcpy(temp->data, ic->data);
}
ic = temp;
return temp;
}*/

/*int count_no_lines_file(char *filename)
{
char buff[100];
FILE *fp = fopen(filename,"a+");
if (fp == NULL)
{
printf("\nError in opening file %s", filename);
return;
}
int count = 0;
fseek(fp, 0, SEEK_SET);
while (fgets(buff,100,fp) != NULL)
{
count++;
}
fclose(fp);
return count;
}*/