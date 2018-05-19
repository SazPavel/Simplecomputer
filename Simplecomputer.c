#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h> 
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include "mysimplecomp.h"
#include "myterm.h"
#include "mybigchar.h"
#include "myreadkey.h"

/*
16384 - 0100 0000 0000 0000
65535 - 1111 1111 1111 1111
32768 - 1000 0000 0000 0000
32767 - 0111 1111 1111 1111
*/

int position_cursor = 0;
char error_str[256];

struct itimerval nval, oval; //timer


void printGUI(){

    mt_gotoXY(1, 1);

    mt_clrscr();
    mt_setfgcolor(C_WHITE);
    mt_setbgcolor(C_BLACK); 


	bc_box(1, 1, 12, 62);
	bc_box(13, 1, 22, 44);
	bc_box(13, 46, 22, 83);
	bc_box(1, 63, 3, 83);
	bc_box(4, 63, 6, 83); 
	bc_box(7, 63, 9, 83); 
	bc_box(10, 63, 12, 83); 

	
	mt_gotoXY(30, 1);	
	printf(" Memory ");
	mt_gotoXY(66, 1);
	printf(" accumulator ");
	mt_gotoXY(64, 4);
	printf(" instuctionCounter ");
	mt_gotoXY(68, 7);
	printf(" Operation ");
	mt_gotoXY(68, 10);
	printf(" Flags ");
	mt_gotoXY(48, 13);
	printf(" Keys: ");	
	
	//accumulator
	mt_gotoXY(70, 2);
	printf("%04X", accum & 65535);
	
	//incounter
	mt_gotoXY(70, 5);	
	printf("%04X",  INCOUNTER);
	
	mt_gotoXY(68, 8);
	int com, c, o;
	com = sc_commandDecode(RAM[position_cursor], &c, &o);
	com = RAM[position_cursor] & 32768;
	if(com == 0)
	    printf("+ %02X : %02X", c, o);
	else
		printf("%02X%02X", c, o);
	
	mt_gotoXY(68, 11);
	int i;
	sc_regGet(FIRST, &i);
	if (i == 1)
		printf("O ");
	else
		printf(" ");
	sc_regGet(SECOND, &i);
	if (i == 1)
		printf("0 ");
	else
		printf(" ");
	sc_regGet(THIRD, &i);	
	if (i == 1)
		printf("M ");
	else
		printf(" ");
	sc_regGet(FOURTH, &i);	
	if (i == 1)
		printf("T ");
	else
		printf(" ");
	sc_regGet(FIFTH, &i);	
	if (i == 1)
		printf("E");
	else
		printf(" ");
	
    //RAM
    mt_setfgcolor(C_WHITE);	
    mt_gotoXY(2, 2);
    for (i = 0; i < 100; i++) {
        if (i % 10 == 0 && i > 0)
            mt_gotoXY(2, 2 + i / 10);

        if (i == INCOUNTER) {
            mt_setfgcolor(C_RED);
        }

        if (i == position_cursor) { 
            mt_setbgcolor(C_WHITE);
            mt_setfgcolor(C_BLACK);
        }
        if ((RAM[i] & 32768) == 0)
            printf("+");
        else
            printf(" ");
        printf("%04X", RAM[i]  & 65535);
        mt_setbgcolor(C_BLACK);
        mt_setfgcolor(C_WHITE);
        printf(" ");
    }

    int arr[5][2];
    arr[0][0] = PLUS_BEGIN;
    arr[0][1] = PLUS_END;

    if ((RAM[position_cursor] & 32768) == 0)
        bc_printbigchar(arr[0], 14, 2);
    
    int wat[4];
    wat[3] = RAM[position_cursor] % 16;
    wat[2] = (RAM[position_cursor] / 16) % 16;
    wat[1] = (RAM[position_cursor] / 256) % 16;
    wat[0] = (RAM[position_cursor] / 4096);

    for (i = 0; i < 4; i++) {//digits
        if (wat[i] == 0) {arr[i + 1][0] = ZERO_BEGIN; arr[i + 1][1] = ZERO_END;}
        if (wat[i] == 1) {arr[i + 1][0] = ONE_BEGIN; arr[i + 1][1] = ONE_END;}
        if (wat[i] == 2) {arr[i + 1][0] = TWO_BEGIN; arr[i + 1][1] = TWO_END;}
        if (wat[i] == 3) {arr[i + 1][0] = THREE_BEGIN; arr[i + 1][1] = THREE_END;}
        if (wat[i] == 4) {arr[i + 1][0] = FOUR_BEGIN; arr[i + 1][1] = FOUR_END;}
        if (wat[i] == 5) {arr[i + 1][0] = FIVE_BEGIN; arr[i + 1][1] = FIVE_END;}
        if (wat[i] == 6) {arr[i + 1][0] = SIX_BEGIN; arr[i + 1][1] = SIX_END;}
        if (wat[i] == 7) {arr[i + 1][0] = SEVEN_BEGIN; arr[i + 1][1] = SEVEN_END;}
        if (wat[i] == 8) {arr[i + 1][0] = EIGHT_BEGIN; arr[i + 1][1] = EIGHT_END;}
        if (wat[i] == 9) {arr[i + 1][0] = NINE_BEGIN; arr[i + 1][1] = NINE_END;}
        if (wat[i] == 0xA) {arr[i + 1][0] = A_BEGIN; arr[i + 1][1] = A_END;}
        if (wat[i] == 0xB) {arr[i + 1][0] = B_BEGIN; arr[i + 1][1] = B_END;}
        if (wat[i] == 0xC) {arr[i + 1][0] = C1_BEGIN; arr[i + 1][1] = C1_END;}
        if (wat[i] == 0xD) {arr[i + 1][0] = D_BEGIN; arr[i + 1][1] = D_END;}
        if (wat[i] == 0xE) {arr[i + 1][0] = E_BEGIN; arr[i + 1][1] = E_END;}
        if (wat[i] == 0xF) {arr[i + 1][0] = F_BEGIN; arr[i + 1][1] = F_END;}

        bc_printbigchar(arr[i + 1], 14, 2 + 8 * (i + 1));
    }

	//keys
	mt_gotoXY(48, 14);
	printf("l  - load");
	mt_gotoXY(48, 14 + 1);
	printf("s  - save");
	mt_gotoXY(48, 14 + 2);
	printf("r  - run");
	mt_gotoXY(48, 14 + 3);
	printf("t  - step");
	mt_gotoXY(48, 14 + 4);
	printf("i  - reset");
	mt_gotoXY(48, 14 + 5);
	printf("F5 - accumulator");
    mt_gotoXY(48, 14 + 6);
	printf("F6 - instructionCounter");
	
	
    mt_gotoXY(1, 24);

    mt_setbgcolor(C_BLACK); 
    mt_setfgcolor(C_WHITE);	

    printf("%s\n", error_str);
}



void timer_init()
{
    nval.it_interval.tv_sec = 0;
    nval.it_interval.tv_usec = 100000;
    nval.it_value.tv_sec = 0;
    nval.it_value.tv_usec = 100000;
}

//BIT INVERSION
int switchbit(const int value, const int position) {
    return (value ^ (1 << position));
}

int is_positive(int num) 
{
    if ((num & 65535) < 32768)
        return 0;
    return 1;
}


// READ FUNCTION
int get_num(int *n)
{
	char buf[256];
	
    sprintf(error_str, "Enter the value:");
    printGUI();

    fflush(stdin);
	fgets(buf, 256, stdin);

	if (sscanf(buf, "%x", n) != 1)
		return -1;
	return 0;
}


int ALU(int command, int operand){
	switch(command){
		case 0x30:{//ADD
			accum += (switchbit(RAM[operand], 15) & 65535) & 32767;
		} break;
		case 0x31:{//SUB
			accum -= (switchbit(RAM[operand], 15) & 65535) & 32767;
		} break;
		case 0x32:{//DIVIDE
			if(RAM[operand] == 32768 || ((switchbit(RAM[operand], 15) & 65535) & 32767) == 0){
				sc_regSet(SECOND, 1);
				return 1;				
			}
			accum /= (switchbit(RAM[operand], 15) & 65535) & 32767;
		} break;
		case 0x33:{//MUL
			accum *= (switchbit(RAM[operand], 15) & 65535) & 32767;
		} break;
        case 0x70: {// RCCR
            int i;
            int tmp = RAM[operand] & 65535;
            for (i = 0; i < accum; i++) {
                int d = tmp % 2;
                tmp >>= 1;
                tmp |= (d * 32768);
            }
            accum = tmp;
        } break;
	}
	return 0;
}


int CU(){


	int command, operand, t = INCOUNTER;
    accum &= 65535;

	if ((RAM[INCOUNTER] & 32768) == 1){
		sc_regSet(FIFTH, 1);
		sc_regSet(FOURTH, 1);
        printGUI();
       	return 1;
	}	

	int tmp = sc_commandDecode(RAM[INCOUNTER], &command, &operand);
	if(tmp == 1){
		sc_regSet(FIFTH, 1);
		sc_regSet(FOURTH, 1);
        printGUI();
		return 1;	
	}	
	
	if(operand >= 100 || operand < 0){
		sc_regSet(THIRD, 1);
		sc_regSet(FOURTH, 1);
        printGUI();
       	return 1;
	}	
	
	switch(command){
		case 0x10:{//READ
			operation1 = command;
			operation2 = operand;
			
            int p, n = 0;
            setitimer(ITIMER_REAL, NULL, NULL);//STOP			

            sprintf(error_str, "Enter the value:");
            printGUI();
			if(!scanf("%d", &n) || n > 16383 || n < 0){
				sc_regSet(FIRST, 1);
				sc_regSet(FOURTH, 1);
                sprintf(error_str, "Error!");
                printGUI();
       			return 1;
			}				

			RAM[operand] = n | 32768;
            sc_regGet(FOURTH, &p);

            if (p == 0)
                setitimer(ITIMER_REAL, &nval, &oval);//START  						
            sprintf(error_str, " ");
	        printGUI();		
		} break;
		case 0x11:{//WRITE
			operation1 = command;
			operation2 = operand;
    		sprintf(error_str, "%X", (RAM[operand] & 32767);
		} break;
        case 0x20:{//LOAD
            operation1 = command;
            operation2 = operand;
            if (is_positive(RAM[operand]))
                accum = (switchbit(RAM[operand], 15) & 65535) & 32767;
            else
                accum = (switchbit(RAM[operand], 15) & 65535) | 32768;
            
            printGUI();
        } break;
        case 0x21:{//STORE
            operation1 = command;
            operation2 = operand;
            if (accum > 16384)
                RAM[operand] = (switchbit(accum, 15) & 65535);
            else
                RAM[operand] = (switchbit(accum, 15) & 65535) | 32768 ;
            printGUI();
        } break;
		case 0x30:{//ADD
			operation1 = command;
			operation2 = operand;
			tmp = ALU(command, operand);	
			if(tmp == 1){	
				sc_regSet(FOURTH, 1);
				return 1;	
			}	
            printGUI();
		} break;
		case 0x31:{//SUB
			operation1 = command;
			operation2 = operand;
			tmp = ALU(command, operand);	
			
			if(tmp == 1){	
				sc_regSet(FOURTH, 1);
				return 1;	
			}	
            printGUI();
		} break;
		case 0x32:{//DIVIDE
			operation1 = command;
			operation2 = operand;
			tmp = ALU(command, operand);	
			
			if(tmp == 1){	
				sc_regSet(FOURTH, 1);
				return 1;	
			}	
            printGUI();
		} break;
		case 0x33:{//MUL
			operation1 = command;
			operation2 = operand;
			tmp = ALU(command, operand);	
			
			if(tmp == 1){	
				sc_regSet(FOURTH, 1);
				return 1;	
			}	
            printGUI();
		} break;
		case 0x40:{//JUMP
			operation1 = command;
			operation2 = operand;
			INCOUNTER = operand;			
            printGUI();
		} break;
		case 0x41:{//JNEG
			operation1 = command;
			operation2 = operand;            
			if((accum & 65535)> 16384)
				INCOUNTER = operand;			
            
            printGUI();
		} break;
		case 0x42:{//JZ
			operation1 = command;
			operation2 = operand;
			if(accum == 0)
				INCOUNTER = operand;			
            printGUI();
		} break;
		case 0x43:{//HALT
			operation1 = command;
			operation2 = 0;
            sc_regSet(FOURTH, 1);
			t = -1;
            printGUI();
		} break;
	
		case 0x55:{//JNS
			operation1 = command;
			operation2 = operand;
			if(accum > 0)
				INCOUNTER = operand;			
            printGUI();
		} break;
		
        case 0x70:{//RCCR
            operation1 = command;
            operation2 = operand;
            tmp = ALU(command, operand);
			if(tmp == 1){
				sc_regSet(FOURTH, 1);
				return 1;	
			}	
            printGUI();
        } break;
		default:{
			sprintf(error_str, "Command %02X is not supported\n", command);
			sc_regSet(FOURTH, 1);
            return 0;
		} break;
	}
	if(t == INCOUNTER)	
		INCOUNTER++;
	return 0;	
}

// SIGNAL HANDLER
void sighandler(int sig)
{
    if (sig == SIGALRM) {//signal from timer
    	CU();
        printGUI();
	}
    if (sig == SIGUSR1) {//user-defined signal
    	alarm(0);
    	sc_regInit();
    	sc_regSet(FOURTH, 1);
    	INCOUNTER = 0;
        accum = 0;
    }
}



// MAIN FUNCTION
int main(int argc, char **argv)
{	

    sc_regInit();
    sc_memoryInit();

    //signal init
    signal(SIGALRM, sighandler);
    signal(SIGUSR1, sighandler);

    timer_init();	

    INCOUNTER = 0;	
    sc_regSet(FOURTH, 1);

    enum keys ch = 0;    
    setbuf(stdout, NULL); 
    char fname[256];
    int all = 0, clock_puls = -1;

    while (!all) {

        printGUI(); 
        
        sprintf(error_str, " ");      
        ch = K_OTHER;
        rk_readkey(&ch);  

        sc_regGet(FIRST, &clock_puls);
        if (clock_puls) setitimer(ITIMER_REAL, NULL, NULL);
        sc_regGet(SECOND, &clock_puls);
        if (clock_puls) setitimer(ITIMER_REAL, NULL, NULL);
        sc_regGet(THIRD, &clock_puls);
        if (clock_puls) setitimer(ITIMER_REAL, NULL, NULL);
        sc_regGet(FIFTH, &clock_puls);
        if (clock_puls) setitimer(ITIMER_REAL, NULL, NULL);

        sc_regGet(FOURTH, &clock_puls);


        if (clock_puls) {
            setitimer(ITIMER_REAL, NULL, NULL);
            switch(ch) {
                case K_F5: { //ACCUMULATOR
                   	int num;  
                	if (get_num(&num) != 0) {
                		sprintf(error_str, "Not a number!");
                		return -1;
                	}
                	if ((num >= 0) && (num < 32768)) {
                		accum = num;
                	}
                	else {
                		sprintf(error_str, "Big number");
                	}
                    sprintf(error_str, " ");                     
                } break;
                case K_F6: { //INCOUNTER
                   	int num;
                    
                	if (get_num(&num) != 0) {
                		sprintf(error_str, "Not a number!");
                	}
                	if ((num >= 0) && (num < 100)) {
                		INCOUNTER = num;
                	}
                	else {
                		sprintf(error_str, "0 >= x > 100");
                	}
                    sprintf(error_str, " ");                     
                } break;
                
                case K_LEFT: if (position_cursor % 10 > 0) position_cursor--; else position_cursor += 9; break; // left
                case K_UP: if (position_cursor >= 10) position_cursor -= 10; else position_cursor = 90 + position_cursor % 10; break; // up
                case K_RIGHT: if (position_cursor % 10 < 9) position_cursor++; else position_cursor -= 9; break; // right
                case K_DOWN: if (position_cursor <= 89) position_cursor += 10; else position_cursor = position_cursor % 10; break; // down
                
                case K_l:{//load
                	printf("Enter filename: ");
                	fgets(fname, 256, stdin);
                	fname[strlen(fname) - 1] = '\0';
                	if (sc_memoryLoad(fname) == 0) {
                        sc_memoryLoad(fname);
                		sprintf(error_str, "File successfully loaded");
                        printGUI();
                	}
                	else {
                		sprintf(error_str, "Cannot load file: %s", fname);
                	}
                    sprintf(error_str, " "); 
                } break;
                case K_s:{//save
                	printf("Enter filename: ");
                	fgets(fname, 256, stdin);
                	fname[strlen(fname) - 1] = '\0';
                	sc_memorySave(fname);
                    sprintf(error_str, " "); 
                } break;
                
                case K_t: { //one step
                    sighandler(SIGALRM);
                    printGUI();
                } break;
                
                default: {}
            }        

        }
        if (ch == K_r){//run
            int treg = -1;
            sc_regGet(FOURTH, &treg);
            if (treg == 1) {
                sc_regSet(FOURTH, 0);
                setitimer(ITIMER_REAL, &nval, &oval);                
            }else{
                alarm(0);
                sc_regSet(FOURTH, 1);
                sprintf(error_str, " "); 
            }
        }
        if (ch == K_q){//quit
            all = 1;
        }
        if (ch == K_i){//reset
            raise(SIGUSR1);
            INCOUNTER = 0;
            accum = 0;
            //int a = sc_memoryInit();
			sc_regSet(FIRST, 0);
			sc_regSet(SECOND, 0);
			sc_regSet(THIRD, 0);
			//sc_regSet(FOURTH, 0);
			sc_regSet(FIFTH, 0);
            position_cursor = 0;
        } 
    }
    return 0;
}
