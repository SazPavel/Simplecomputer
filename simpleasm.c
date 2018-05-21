#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h> 

#include "mysimplecomp.h"
#include "myterm.h"
#include "mybigchar.h"
#include "myreadkey.h"
#include "simpleasm.h"

//SIMPLE ASSEMBLER TRANSLATOR

int test_argv(char *argv[]){//verify input and output files
	char *ptr1, *ptr2;
	
	ptr1 = strrchr(argv[1], '.');
	ptr2 = strrchr(argv[2], '.');
	if ((strcmp(ptr1, ".sa") != 0) || (strcmp(ptr2, ".o") != 0))
		return -1;
	else
		return 0;
}

int str_command(char *str){//command code
	if (strcmp(str, "READ") == 0)
		return 0x10;
	if (strcmp(str, "WRITE") == 0)
		return 0x11;
	if (strcmp(str, "LOAD") == 0)
		return 0x20;
	if (strcmp(str, "STORE") == 0)
		return 0x21;
	if (strcmp(str, "ADD") == 0)
		return 0x30;
	if (strcmp(str, "SUB") == 0)
		return 0x31;
	if (strcmp(str, "DIVIDE") == 0)
		return 0x32;
	if (strcmp(str, "MUL") == 0)
		return 0x33;
	if (strcmp(str, "JUMP") == 0)
		return 0x40;
	if (strcmp(str, "JNEG") == 0)
		return 0x41;
	if (strcmp(str, "JZ") == 0)
		return 0x42;
	if (strcmp(str, "HALT") == 0)
		return 0x43;
	if (strcmp(str, "JNP") == 0)
		return 0x59;
		
	return -1;
	
	
}

int str_sc_word(char *str, int *value){//record numbers
	int pos = 0, plus = 1;
	int n;

	if (str[0] == '+') {
		plus = 0;
		pos = 1;
	}
	if (sscanf(str + pos, "%x", &n) != 1)
		return -1;
	if(n > 32767 || n < -32767)	
		return -1;
	if(plus == 1)	
		*value = n | 32768;
	else
		*value = n | 32768;	
	return 0;
}


int pars_line(char *str, int *addr, int *value){
	char *ptr;
	int operand, command;
	int flag = 0;
	ptr = strchr(str, '\n');
	if (ptr == NULL){
		return -1;
	}
	ptr = strtok(str, " \t");
	if (ptr == NULL)
		return -1;
	if (sscanf(ptr, "%d", addr) != 1)
		return -1;
	if ((*addr < 0) || (*addr >= 100))
		return -1;
		
	ptr = strtok(NULL, " \t");
	if (ptr == NULL)
		return -1;
	else
		if (strcmp(ptr, "=") == 0){
			flag = 1;
		}			
		else {			
			command = str_command(ptr);
			if (command == -1)
				return -1;
		}
	ptr = strtok(NULL, " \t");
	if (ptr == NULL)
		return -1;
	if (flag != 1){
		if (sscanf(ptr, "%d", &operand) != 1){
			
			return -1;
		}
	}else{		
		if (str_sc_word(ptr, value) == -1){
			return -1;
		}
	}
	if ((operand < 0) || (operand >= 100)){
		return -1;
	}
	if (flag != 1) {
		sc_commandEncode(command, operand, value);
	}
	return 0;
}


int main(int argc, char *argv[])
{
	char buf[256], line[256];
	FILE *input, *output;
	int value, addr, line_cnt = 1;
	int err = 0;
	
	if (argc != 3){
		perror("Incorrect arguments!\n");
		return -1;
	}
	if (test_argv(argv) != 0){
		perror("Incorrect arguments!\n");
		return -1;
	}
	if ((output = fopen(argv[2], "wb")) == NULL){
		fprintf(stderr, "Cannot open file: %s", argv[1]);
		return -1;
	}
	if ((input = fopen(argv[1], "rb")) == NULL){
		fprintf(stderr, "Cannot open file: %s", argv[2]);
		return -1;
	}
	
	memset(RAM, 0, 100 * sizeof(int));
	while (fgets(line, 256, input)) {
		
		strcpy(buf, line);
		err = pars_line(buf, &addr, &value);
			RAM[addr] = value;
		line_cnt++;
	}
	err = 0;
	//TEST
/*	for(err = 0; err < 100; err++){	
		if(err % 10 == 0 && err != 0)
			printf("\n");
		printf(" %x ", RAM[err]);
	}*/
	fwrite(RAM, 1, 100*sizeof(int), output);
	
	printf("Done\n");
	fclose(input);
	fclose(output);
	return 0;
}
