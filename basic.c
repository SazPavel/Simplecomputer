#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "basic.h"
//BASIC TRANSLATOR
int val_table[26];
int stack_addr[100];
struct b_label labels[100];
struct memory_t memory[100];
int code_pos = 0, val_pos = 99, label_pos = 0;



int find_label(int label) //Finding matches for Assembly and basic strings
{
	int i;
	
	for (i = 0; i < label_pos; i++){
		if (labels[i].label == label)
			return labels[i].pos;
	}
	return -1;
}


// SAVE TO FILE
void save_as(FILE *file){
	int i;
	int high, low;

	for (i = 0; i < code_pos; i++){
			
		if(memory[i].is_val != 1 && memory[i].is_val != 0){//Checks that all jumps have a destination point
			int j = find_label(memory[i].is_val);
			if (j < 0) {
				printf("Label not found!\n");
				exit(1);
			}
			memory[i].operand = j;
		}
		switch (memory[i].command){
			case 10: //READ
				fprintf(file, "%02d READ %d\n", i, memory[i].operand);
			break;
			
			case 11: //WRITE
				fprintf(file, "%02d WRITE %d\n", i, memory[i].operand);
			break;
			
			case 20: //LOAD
				fprintf(file, "%02d LOAD %d\n", i, memory[i].operand);
			break;
			
			case 21: //STORE
				fprintf(file, "%02d STORE %d\n", i, memory[i].operand);
			break;
			
			case 30: //ADD
				fprintf(file, "%02d ADD %d\n", i, memory[i].operand);
			break;
			
			case 31: //SUB
				fprintf(file, "%02d SUB %d\n", i, memory[i].operand);
			break;
			
			case 32: //DIVIDE
				fprintf(file, "%02d DIVIDE %d\n", i, memory[i].operand);
			break;
			
			case 33: //MUL
				fprintf(file, "%02d MUL %d\n", i, memory[i].operand);
			break;
			
			case 40: //JUMP
				fprintf(file, "%02d JUMP %d\n", i, memory[i].operand);
			break;
			
			case 41: //JNEG
				fprintf(file, "%02d JNEG %d\n", i, memory[i].operand);
			break;
			
			case 42: //JZ
				fprintf(file, "%02d JZ %d\n", i, memory[i].operand);
			break;
			
			case 43: //HALT
				fprintf(file, "%02d HALT %d\n", i, memory[i].operand);
			break;
		}
	}
	for (i = val_pos+1; i < 100; i++){
		if (memory[i].command != 0) {
			high = (memory[i].command >> 7) & 0x7F;
			low = memory[i].command & 0x7F;
			fprintf(file, "%02d = +%02x%02x\n", i, high, low);// in ram %X
		}
	}
}


char *get_label_and_keyw(char *str, int *label, char *keyword)//reads line number and command
{
	int cnt;
	int i = 0;
	
	cnt = sscanf(str, "%d%s", label, keyword);
	if (cnt != 2)
		return NULL;
	for (; (str[i] == ' ') || (str[i] == '\t'); i++);
	for (; (str[i] != ' ') && (str[i] != '\t'); i++);
	for (; (str[i] == ' ') || (str[i] == '\t'); i++);
	for (; (str[i] != ' ') && (str[i] != '\t'); i++);
	for (; (str[i] == ' ') || (str[i] == '\t'); i++);
	return str + i;
}

int get_command_code(char *str){//finds the command code
	
	if (strcmp(str, "REM") == 0)
		return KEYW_REM;
	if (strcmp(str, "INPUT") == 0)
		return KEYW_INPUT;
	if (strcmp(str, "PRINT") == 0)
		return KEYW_OUTPUT;
	if (strcmp(str, "GOTO") == 0)
		return KEYW_GOTO;
	if (strcmp(str, "IF") == 0)
		return KEYW_IF;
	if (strcmp(str, "LET") == 0)
		return KEYW_LET;
	if (strcmp(str, "END") == 0)
		return KEYW_END;
	
		return -1;
}

int is_delim(char c){
	
	if ((c == ' ') || (c == '\t') || (c == '\0') || (c == '\n'))
		return 0;
	else
		return -1;
}

char *cpy_token(char *token, char *str){
	int i;
	
	for (i = 0; (str[i] == ' ') || (str[i] == '\t'); i++);
	str += i;
	for (i = 0; is_delim(str[i]) != 0; i++){
		token[i] = str[i];
	}
	token[i] = '\0';
	
	return str+i;
}

int str_is_empty(char *str){
	int i;
	
	for (i = 0; (str[i] == ' ') || (str[i] == '\t'); i++);
	if ((str[i] == '\n') || (str[i] == '\0'))
		return 0;
	else
		return -1;
}

int test_argv(char *argv[]){	
	char *ptr1;
	
	ptr1 = strrchr(argv[2], '.');
	if ((strcmp(ptr1, ".sa") != 0))
		return -1;
	else
		return 0;
}

int is_value(char *str){
	if (isupper(str[0])) {
			return 0;
	}
	else
		return -1;
}

int char_to_int(char c){
	return (int)(c - 48);
}

// GET VARIABLE ADDRESS
int get_val_addr(char c){
	if (isupper(c)){
		if (val_table[c-'A'] < 0){
			val_table[c-'A'] = val_pos;
			memory[val_pos].is_val = 1;
			memory[val_pos].command = 0;
			val_pos--;
		}
		return val_table[c-'A'];
	}
	else
		return -1;
}

// GET CONSTANT ADDRESS
int get_const_addr(int n){
	int i;
	for(i = 0; i < 100; i++){
		if(memory[i].is_val == 1 && memory[i].command == n)
		return i;
	}	
	return -1;
}

// ADD CONSTANT
int add_const(int n){
	if(memory[val_pos].command != 0){
		printf("Out of memory1\n");
		exit(1);
	}
	memory[val_pos].is_val = 1;
	memory[val_pos].command = n;
	val_pos--;
	return val_pos+1;
}

// ADD CODE JUMP FORWARD
void add_code_jump(int command, int operand, int lab){
	if(memory[code_pos].command != 0){
		printf("Out of memory2\n");
		exit(1);
	}
	memory[code_pos].is_val = lab;
	memory[code_pos].command = command;
	memory[code_pos].operand = operand;
	code_pos++;
}

// ADD CODE
void add_code(int command, int operand){
	if(memory[code_pos].command != 0){
		printf("Out of memory3\n");
		exit(1);
	}
	memory[code_pos].is_val = 0;
	memory[code_pos].command = command;
	memory[code_pos].operand = operand;
	code_pos++;
}

// GET STACK ADDRESS
int get_stack_addr(int n){
	if(memory[code_pos].command != 0){
		printf("Out of memory4\n");
		exit(1);
	}
	if (stack_addr[n] < 0){
		stack_addr[n] = val_pos;
		memory[code_pos].is_val = 1;
		memory[code_pos].command = 0;
		val_pos--;
	}
	return stack_addr[n];
}

// PUSH TO STACK
struct stack_t *stack_push(struct stack_t *head, char a){
	struct stack_t *ptr;

	if((ptr = malloc(sizeof(struct stack_t))) == NULL) {
		printf("Out of memory\n");
		exit(1);
	}
	ptr->c = a;
	ptr->next = head;
	return ptr;
}

// POP FROM STACK
char stack_pop(struct stack_t **head){
	struct stack_t  *ptr;
	char a;

	if(*head == NULL)
		return '\0';
	ptr = *head;
    a = ptr->c;
    *head = ptr->next;
	free(ptr);
    return a;
}

// GET OPERATION PRIORITY
int get_prior(char c){
    switch(c)
    {
    	case '*':
    	case '/':
    		return 3;
    	case '-':
    	case '+':
    		return 2;
    	case '(':
    		return 1;
    }
	return 0;
}
/*
   /)/)
=( '.' )=
*/
// EXPRESSION TO STACK
void translate_to_stack(char *outstr, char *a){
	struct stack_t *opers = NULL;
	int k, point;

	k = 0;
	point = 0;
	while(a[k] != '\0' && a[k] != '\n'){//while not end
		if(a[k] == ')') {
			while((opers->c) != '(')
				outstr[point++]=stack_pop(&opers);
			stack_pop(&opers);
		}
		if((a[k] >= 'A' && a[k] <= 'Z') || isdigit(a[k]))
			outstr[point++] = a[k];
		if(a[k] == '(')
			opers = stack_push(opers, '(');
		if(a[k] == '+' || a[k] == '-' || a[k] == '/' || a[k] == '*'){
			if(opers == NULL)
				opers = stack_push(opers, a[k]);
			else
				if(get_prior(opers->c) < get_prior(a[k]))
					opers = stack_push(opers, a[k]);
				else {
					while((opers != NULL) && (get_prior(opers->c) >= get_prior(a[k])))
						outstr[point++] = stack_pop(&opers); 
					opers = stack_push(opers, a[k]);
				}
		}
		k++;
	}
	while(opers != NULL)
		outstr[point++] = stack_pop(&opers);
	outstr[point] = '\0';
}

void stack_pars(char *stac, int val){
	int i = 0;
	int depth = 0;
	int addr;
	int st1, st2;
	
	while (stac[i] != '\0'){
		if ((stac[i] >= 'A') && (stac[i] <= 'Z')){
			addr = get_stack_addr(depth);
			add_code(20, get_val_addr(stac[i]));
			add_code(21, addr);
			depth++;
		}
		if(isdigit(stac[i])){
			int num = char_to_int(stac[i]);
			addr = add_const(num);
			addr = get_stack_addr(depth);	
			add_code(20, get_const_addr(num));
			add_code(21, addr);
			depth++;
		}			
		if ((stac[i] == '+') || (stac[i] == '-') || (stac[i] == '*') || (stac[i] == '/')){
			if (depth < 2){
				printf("Uncorrect LET statement2\n");
				exit(1);
			}
			st1 = get_stack_addr(depth - 2); 
			st2 = get_stack_addr(depth - 1); 
			add_code(20, st1);
			switch (stac[i]){
				case '+':
					add_code(30, st2);
					break;
				
				case '-':
					add_code(31, st2);
					break;
				
				case '/':
					add_code(32, st2);
					break;
				
				case '*':
					add_code(33, st2);
					break;
			}
			add_code(21, st1);
			depth--;
		}
		i++;
	}
	if (depth != 1){
		printf("Uncorrect LET statement3\n");
		exit(1);
	}
	addr = get_stack_addr(0);
	add_code(20, addr);
	add_code(21, val);
}

int parse_line(char *str, int key_w){
	char *ptr;
	char token[256];
	char stac[256];
	int label;
	int if_val1, if_val2; 
	int if_jmp, num;
	char sign;
	int keyw;
	int val; // LET val
	
	switch (key_w) {
		case KEYW_INPUT:
			ptr = cpy_token(token, str);
			if ((!str_is_empty(ptr)) && (!is_value(token))){                                                                  
				add_code(10, get_val_addr(token[0])); // READ
			}
			else {
				printf("Not a valid value3\n");
				exit(1);
			}
			break;
		
		case KEYW_OUTPUT:
			ptr = cpy_token(token, str);
			if ((!str_is_empty(ptr)) && (!is_value(token))){
				add_code(11, get_val_addr(token[0])); // WRITE
			}
			else {
				printf("Not a valid value2\n");
				exit(1);
			}
			break;
		
		case KEYW_GOTO:
			if (sscanf(str, "%d", &label)){
				int addr;
				
				addr = find_label(label);
				if (addr < 0){
					add_code_jump(40, 1, label);// JUMP forward
				}else
					add_code(40, addr); // JUMP
			}
			else  {
				printf("Not a valid value1\n");
				exit(1);
			}
			break;
		
		case KEYW_END:
			add_code(43, 0); // HALT
			break;
		
		case KEYW_IF:
			ptr = cpy_token(token, str);
			if (is_value(token) == 0) {//only one letter
				if_val1 = get_val_addr(token[0]); //addr of value
			}
			else if (sscanf(token, "%d", &num) == 1){//if digit
				if_val1 = add_const(num);
			}
			else {
				printf("Not a value or number2\n");
				exit(1);
			}
			
			ptr = cpy_token(token, ptr);
			if (strcmp(token, "<") == 0)
				sign = '<';
			else if (strcmp(token, ">") == 0)
				sign = '>';
			else if (strcmp(token, "=") == 0)
				sign = '=';
			else{
				printf("Unknown logical operation\n");
				exit(1);
			}
			
			ptr = cpy_token(token, ptr);
			if (is_value(token) == 0){
				if_val2 = get_val_addr(token[0]);
			}
			else if (sscanf(token, "%d", &num) == 1){
				if_val2 = add_const(num);
			}
			else{
				printf("Not a value or number1\n");
				exit(1);
			}
			
			switch (sign){
				case '<'://val1 - val2 < 0
					add_code(20, if_val1);//LOAD
					add_code(31, if_val2);//SUB
					add_code(41, code_pos + 2);//JNEG
					break;
				
				case '>'://val2 - val1 < 0
					add_code(20, if_val2);//LOAD
					add_code(31, if_val1);//SUB
					add_code(41, code_pos + 2);//JNEG
					break;
				
				case '=':////val1 - val2 == 0
					add_code(20, if_val1);//LOAD
					add_code(31, if_val2);//SUB
					add_code(42, code_pos + 2);//JZ
					break;
			}
			if_jmp = code_pos;
			add_code(40, 0);
			ptr = cpy_token(token, ptr);
			keyw = get_command_code(token);
			if (keyw < 0) {
				printf("Unknown operator3\n");
				exit(1);
			}
			if (keyw == KEYW_IF){
				printf("Multiple if!\n");
				exit(1);
			}
			parse_line(ptr, keyw);
			memory[if_jmp].operand = code_pos;
			break;
			
			case KEYW_LET:
				ptr = cpy_token(token, str);
				if (!is_value(token)){
					val = get_val_addr(token[0]);
				}
				else{
					printf("Not a valid value\n");
					exit(1);
				}
				ptr = cpy_token(token, ptr);
				if (strcmp(token, "=") != 0){
					printf("Uncorrect LET statement1!\n");
					exit(1);
				}
				translate_to_stack(stac, ptr);//TO STACK AND BACK
				stack_pars(stac, val);
				break;
	}
	return 0;
}

int main(int argc, char *argv[]){
	FILE *input, *output;
	char line[256], keyw_str[256];
	char *str;
	int i;
	int label, keyw;
	
	if (argc != 3){
		printf("Incorrect arguments1!\n");
		return -1;
	}
	if (test_argv(argv) != 0){
		printf("Incorrect arguments2!\n");
		return -1;
	}
	if ((input = fopen(argv[1], "rb")) == NULL){
		printf("Cannot open file: %s", argv[1]);
		return -1;
	}
	if ((output = fopen(argv[2], "wb")) == NULL){
		printf("Cannot open file: %s", argv[2]);
		return -1;
	}
	
	for (i = 0; i < 26; i++)
		val_table[i] = -1;
	for (i = 0; i < 100; i++)
		stack_addr[i] = -1;
	while (fgets(line, 256, input)){
		if (str_is_empty(line) == 0)
			continue;
		str = get_label_and_keyw(line, &label, keyw_str);//label - %d  keyw_str - command str - next
		if (str == NULL) {
			printf("Unknown operator1!\n");
			return -1;
		}
		keyw = get_command_code(keyw_str);
		if (keyw == -1) {
			printf("Unknown operator2!\n");
			return -1;
		}
		labels[label_pos].label = label;
		labels[label_pos].pos = code_pos;
		label_pos++;
		parse_line(str, keyw);
	}
	save_as(output);
	printf("Done\n");
	fclose(output);
	fclose(input);
	return 0;
}
