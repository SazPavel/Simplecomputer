#ifndef SCOMP_H
#define SCOMP_H

#define RAM_SIZE 100

// enter alt_cm
#define BC_ENTER "\E(0"
// exit alt_cm
#define BC_EXIT "\E(B"
#define FIRST 1		   //overflow
#define SECOND 1 << 1  //division by 0
#define THIRD 1 << 2   //out of memory
#define FOURTH 1 << 3  //ignoring clock pulses
#define FIFTH 1 << 4    //incorrect command


// COMMAND LIST
#define READ 0x10
#define WRITE 0x11

#define LOAD 0x20
#define STORE 0x21

#define ADD 0x30
#define SUB 0x31
#define DIVIDE 0x32
#define MUL 0x33

#define JUMP 0x40
#define JNEG 0x41
#define JZ 0x42
#define HALT 0x43

#define NOT 0x51
#define AND 0x52
#define OR 0x53
#define XOR 0x54
#define JNS 0x55
#define JC 0x56
#define JNC 0x57
#define JP 0x58
#define JNP 0x59
#define CHL 0x60
#define SHR 0x61
#define RCL 0x62
#define RCR 0x63
#define NEG 0x64
#define ADDC 0x65
#define SUBC 0x66
#define LOGLC 0x67
#define LOGRC 0x68
#define RCCL 0x69
#define RCCR 0x70
#define MOVA 0x71
#define MOVR 0x72
#define MOVCA 0x73
#define MOVCR 0x74
#define ADDCbI 0x75
#define SUBCbI 0x76


int RAM[RAM_SIZE];


int accum;

int REGISTER; 
int INCOUNTER; 
int operation1;
int operation2;

int sc_regSet(int reg, int value)
{
    if (reg != FIRST && reg != SECOND && reg != THIRD && reg != FOURTH && reg != FIFTH) {
        return 1;    
    }

    if (value == 0) {
        REGISTER &= ~(reg);
    } else {
        if (value == 1) {
            REGISTER |= reg;
        } else {
            return 1;
        }
    }
    return 0;
}


int sc_memoryInit()
{
    int i;
    for (i = 0; i < RAM_SIZE; i++)
        RAM[i] = 0;

    return 0;
}

int sc_memorySet(int address, int value)
{
    int flag = 0;
    if (address >= 0 && address < RAM_SIZE)
        RAM[address] = value;
    else
        flag = 1;
    sc_regSet(THIRD, flag);
    return flag;
}

int sc_memoryGet(int address, int *value)
{
    int flag = 0;
    if (address >= 0 && address < RAM_SIZE)
        *value = RAM[address];
    else
        flag = 1;
    sc_regSet(THIRD, flag);
    return flag;
}

int sc_memorySave(char *filename)
{
    FILE *f = NULL;
    f = fopen(filename, "wb");
    int flag = 0;

    if (f == NULL)
        flag = 1;
    else {
        fwrite(RAM, sizeof(int), RAM_SIZE, f);
        fclose(f);
    }
    return flag;
}

int sc_memoryLoad(char *filename)
{
    FILE *f = NULL;
    f = fopen(filename, "rb");
    int flag = 0;

    if (f == NULL)
        flag = 1;
    else {
        fread(RAM, sizeof(int), RAM_SIZE, f);
        fclose(f);
    }

    if (flag == 1)
        printf("FILE READ ERROR!\n");
    
    return flag;
}

int sc_regInit()
{
    REGISTER = 0;
    INCOUNTER = 0;
    return 0;
}

int sc_regGet(int reg, int *value)
{
    int k = -1;
    switch(reg) {
        case FIRST: k = 0; break;
        case SECOND: k = 1; break;
        case THIRD: k = 2; break;
        case FOURTH: k = 3; break;
        case FIFTH: k = 4; break;
    }
    if (k == -1)
        return 1;
    *value = (REGISTER >> k) & 1;

    return 0;
}

int sc_commandEncode(int command, int operand, int *value)
{
    if (operand < 0 || operand > 255 || command < 0 || command > 127) {
        sc_regSet(THIRD, 1);
        return 1;
    }

    int tmp = 0;
    tmp |= (command << 8);
    tmp |= operand;
    *value = tmp;

    return 0;
}

int sc_commandDecode(int value, int *command, int *operand)
{
    if (value > 0x8000 || value < 0) {
        sc_regSet(FOURTH, 1);
        sc_regSet(FIFTH, 1);
        return 1;
    }
    int op = 255, com = 127;
    op &= value;
    com &= (value >> 8);
    
    *operand = op;    
    *command = com;

    return 0;
}





#endif
