#ifndef MY_READKEY
#define MY_READKEY
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h> 
#include <fcntl.h>
#include <string.h>
#define REG_NOCANON 2
#define REG_CANON 1

enum keys { K_F5 = 0xF5, K_F6 = 0xF6, K_OTHER = '0',
            K_s = 's', K_l = 'l', K_i = 'i', K_r = 'r', K_t = 't',
            K_q = 'E', K_LEFT = 'L', K_UP = 'U', K_RIGHT = 'R', K_DOWN = 'D', K_ENTER = 'N'};

struct termios savetty;
struct termios tty;



int rk_mytermsave()
{
	struct termios options;
	FILE *save;
	
	if (tcgetattr(STDIN_FILENO, &options) != 0)
		return -1;
	if ((save = fopen("termsettings", "wb")) == NULL)
		return -1;
	fwrite(&options, sizeof(options), 1, save);
	fclose(save);
	
	return 0;
}

int rk_mytermrestore()
{
	struct termios options;
	FILE *save;

	if ((save = fopen("termsettings", "rb")) == NULL)
		return -1;
	if (fread(&options, sizeof(options), 1, save) > 0) {
		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &options) != 0)
			return -1;
    }
	else
		return -1;

	return 0;
}

int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint)
{
	struct termios options;
	
	if (tcgetattr(STDIN_FILENO, &options) != 0)
		return -1;
	if (regime == 1)
		options.c_lflag |= ICANON;
	else
	if (regime == 0)
		options.c_lflag &= ~ICANON;
	else
		return -1;
	if (regime == 0) {
		options.c_cc[VTIME] = vtime;
		options.c_cc[VMIN] = vmin;
		if (echo == 1)
			options.c_lflag |= ECHO;
		else
		if (echo == 0)
			options.c_lflag &= ~ECHO;
		else
			return -1;
		if (sigint == 1)
			options.c_lflag |= ISIG;
		else
		if (sigint == 0)
			options.c_lflag &= ~ISIG;
		else
			return -1;
	}
	if (tcsetattr(STDIN_FILENO, TCSANOW, &options) != 0)
		return -1;
	
	return 0;
}


int rk_readkey(enum keys *key)
{
	struct termios orig_options;
	char buf[16];
	int num_read;
	
	if (tcgetattr(STDIN_FILENO, &orig_options) != 0)
		return -1;
	if (rk_mytermregime(0, 0, 1, 0, 1) != 0)
		return -1;
	num_read = read(STDIN_FILENO, buf, 15);
	if (num_read < 0)
		return -1;
	buf[num_read] = '\0';
	if (strcmp(buf, "l") == 0)
		*key = K_l;
	else
	if (strcmp(buf, "s") == 0)
		*key = K_s;
	else
	if (strcmp(buf, "r") == 0)
		*key = K_r;
	else
	if (strcmp(buf, "t") == 0)
		*key = K_t;
	else
	if (strcmp(buf, "i") == 0)
		*key = K_i;
	else
	if (strcmp(buf, "q") == 0)
		*key = K_q;
	else
	if (strcmp(buf, "\n") == 0)
		*key = K_ENTER;
	else
	if (strcmp(buf, "\033[15~") == 0)
		*key = K_F5;
	else
	if (strcmp(buf, "\033[17~") == 0)
		*key = K_F6;
	else
	if (strcmp(buf, "\033[A") == 0)
		*key = K_UP;
	else
	if (strcmp(buf, "\033[B") == 0)
		*key = K_DOWN;
	else
	if (strcmp(buf, "\033[C") == 0)
		*key = K_RIGHT;
	else
	if (strcmp(buf, "\033[D") == 0)
		*key = K_LEFT;
	else
		*key = K_OTHER;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &orig_options) != 0)
		return -1;

	return 0;
}

#endif
