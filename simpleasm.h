#ifndef SIM_AS_H
#define SIM_AS_H

#include <stdio.h>
#include <string.h>



int str2sc_word(char *str, int *value);
int str2command(char *str);
int pars_line(char *str, int *addr, int *value);
void print_error(char *line, int line_cnt, int err);
int test_argv(char *argv[]);
int main(int argc, char *argv[]);

#endif 
