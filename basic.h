#ifndef BASIC
#define BASIC



struct memory_t {
	int is_val;
	int command;
	int operand;
};

struct b_label {
	int label;
	int pos;
};

struct stack_t {
	char c;
	struct stack_t *next;
};

enum keywords {
	KEYW_REM = 1,
	KEYW_INPUT = 2,
	KEYW_OUTPUT = 3,
	KEYW_LET = 4,
	KEYW_IF = 5,
	KEYW_GOTO = 6,
	KEYW_END = 7,
};

#endif
