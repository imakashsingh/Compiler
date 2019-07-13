#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <string.h>	
#define NO_OF_OPCODES 14
#define NO_OF_REGISTERS 8
#define INPUT_FILE "input1.txt"
#define OUTPUT_FILE "ouput1.txt"

typedef struct symbol_table
{
	char name[10];
	int address;
	int size;
	symbol_table *next;
}symbol_table;

typedef struct intercode
{
	int line_no;
	int operation;
	int op1;
	int op2;
	int op3;
	int jump_line;
	intercode *next;
}intercode;

typedef struct block_table
{
	char block_name[20];
	int address;
	block_table *next;
}block_table;

typedef struct stackk
{
	int arr[100];
	int top;
}stackk;

void parse_command(char *parsed_input, symbol_table** st, intercode **ic, int* instructions_read, block_table **bt, int *memory,stackk* stack_arr);

void tokenize(char *first_part, char *remaining_part, symbol_table** st, intercode **ic, int* instructions_read, block_table **bt, int *memory, stackk* stack_arr);

void process_mov(char* rem_part, symbol_table** st, intercode **ic, int* instructions_read, block_table **bt, int *memory);

void process_arith(char* first_name, char* rem_part, symbol_table** st, intercode **ic, int* instructions_read, block_table **bt, int *memory);

void process_io(char *first_part, char *remaining_part, symbol_table** st, intercode **ic, int* instructions_read, block_table **bt, int *memory);

void process_label(char* first_part,int* instructions_read,block_table** bt);

void process_if(char *remaining_part,symbol_table** st,intercode **ic,int *instructions_read,block_table **bt,stackk *stack_arr);

void process_jump(char *rem_part,intercode **ic, int *instructions_read, block_table **bt);

void process_else(intercode **ic,block_table **bt, int* instructions_read,stackk *stack_arr);

void process_endif(intercode **ic, block_table **bt, int* instructions_read, stackk *stack_arr);

void process_data(char* first_part, char* remaining_part, symbol_table** st, intercode** ic, int* instructions_read,block_table **bt, int* memory);

void write_to_file(intercode *ic, symbol_table *st, block_table *bt);

/*typedef struct opcode
{
char opcode_name[10];
int opcode;
}opcode;

typedef struct registercode
{
char register_name[4];
int regitser_value;
}registercode;*/