#ifndef MYTERM_H
#define MYTERM_H

#define CLEAR "\E[H\E[J"
#define M_BEGIN "\E["
#define M_END "H"

#define C_BEGIN  "\E["
#define C_END "m"

typedef enum colors {
    C_BLACK = 0, C_RED = 1, C_GREEN = 2, C_YELLOW = 3, C_BLUE = 4, C_MAGENTA = 5, C_CYAN = 6, C_WHITE = 7
} COLOR;


// get screen size
int mt_getscreensize(int *rows, int *cols)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    if (rows == NULL || cols == NULL) {
        return -1;
    }

    *rows = w.ws_row;
    *cols = w.ws_col;
    return 0;
}

// move cursor
int mt_gotoXY(int y, int x)
{
    int a, b;
    mt_getscreensize(&a, &b);
    if (x > a || x < 0 || y > b || y < 0) {
        return -1;
    }
    printf("%s%d;%d%s", M_BEGIN, x, y, M_END);
    return 0;
}


// clear screen
int mt_clrscr()
{
    printf("%s", CLEAR);
    mt_gotoXY(0, 0);
    return 0;
}


// set text color
int mt_setfgcolor(COLOR color)
{
    if (color >= C_BLACK && color <= C_WHITE) {
        printf("%s3%d%s", C_BEGIN, color, C_END);
        return 0;
    }
    return -1;
}

// set background color
int mt_setbgcolor(COLOR color)
{
    if (color >= C_BLACK && color <= C_WHITE) {
        printf("%s4%d%s", C_BEGIN, color, C_END);
        return 0;
    }
    return -1;
}


#endif
