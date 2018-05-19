#ifndef BIG_F_CHAR
#define BIG_F_CHAR

// enter alt_cm
#define BC_ENTER "\E(0"
// exit alt_cm
#define BC_EXIT "\E(B"

#define ZERO_BEGIN 471999010
#define ZERO_END 572662300
#define ONE_BEGIN 67900452
#define ONE_END 67372036
#define TWO_BEGIN 471998980
#define TWO_END 135274558
#define THREE_BEGIN 471990796
#define THREE_END 33694236
#define FOUR_BEGIN 101323298
#define FOUR_END 1040318978
#define FIVE_BEGIN 1042292796
#define FIVE_END 33694236
#define SIX_BEGIN 471998524
#define SIX_END 572662300
#define SEVEN_BEGIN 1040318980
#define SEVEN_END 67635208
#define EIGHT_BEGIN 471999004
#define EIGHT_END 572662300
#define NINE_BEGIN 471999006
#define NINE_END 33694236
#define PLUS_BEGIN 1579134
#define PLUS_END 2115508224
#define A_BEGIN 1848931
#define A_END 1669292899
#define B_BEGIN 8282979
#define B_END 2120442750
#define C1_BEGIN 1979232
#define C1_END 1616917278
#define D_BEGIN 8152675
#define D_END 1667458684
#define E_BEGIN 8347744
#define E_END 2120245375
#define F_BEGIN 8347744
#define F_END 2120245344



int **numbers_to_bits(int *array)
{
    int i, j, k, num;

    int **arr = (int**)malloc(2 * sizeof(int*));
    for (i = 0; i < 2; i++) {
        arr[i] = (int*)malloc(32 * sizeof(int));
    }
    
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            num = array[i] >> ((7 - j) * 8);
            for (k = 0; k < 8; k++) {
                arr[i][k + j * 8] = (num >> (7 - k)) & 1;
            }
        }        
    }

    return arr;
}

int pow2(int degree)
{
    int x = 1;
    int i;
    for (i = 0; i < degree; i++)
        x *= 2;
    return x;
}

int *bits_to_numbers(int **arr, int *big)
{
    int i, j;

    big[0] = 0;
    big[1] = 0;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 32; j++) {
            big[i] += pow2(j) * arr[i][32 - j - 1];
        }        
    }

    return big;
}



int bc_printA(char *str)
{
    if (str == NULL) {
        return -1;
    }
    printf("%s", BC_ENTER);
    printf("%s", str);
    printf("%s", BC_EXIT);
    return 0;
}

int bc_box(int x1, int y1, int x2, int y2)
{
    if (x1 >= x2 || y1 >= y2) {
        return -1;
    }
    int i, j;
    //mt_clrscr();
    printf("%s", BC_ENTER);

    for (i = x1; i <= x2; i++) {
        if (i == x1) { // top
            mt_gotoXY(y1, x1);
            printf("l");
            for (j = y1 + 1; j < y2; j++)
                printf("r");
            printf("k");
        } else {
            if (i == x2) { // bottom
                mt_gotoXY(y1, x2);
                printf("m");
                for (j = y1 + 1; j < y2; j++)
                    printf("r");
                printf("j");
            } else {
                mt_gotoXY(y1, i);
                printf("x");
                mt_gotoXY(y2, i);
                printf("x");
            }
        }
    }  
    
    printf("%s", BC_EXIT); 
    printf("\n");
    return 0;
}

int bc_printbigchar(int *array, int y, int x)
{
    mt_gotoXY(x, y);
    //mt_setfgcolor(fg);
    //mt_setbgcolor(bg);
    int i, k;
    int **arr = numbers_to_bits(array);

    for (k = 0; k < 2; k++) {
        for (i = 0; i < 32; i++) {
            if (i % 8 == 0 && (i != 0)) {
                mt_gotoXY(x, y + (i / 8 + (k * 4)));
            }
            if (arr[k][i] == 0)
                printf(" ");
            else 
 //               printf("a");
                bc_printA("a");
//            printf("%d ", arr[k][i]);        
        }   
        mt_gotoXY(x, y + 4 * (k + 1));
    }   
    
    free(arr);
    return 0;
}

int bc_setbigcharpos(int *big, int x, int y, int value)
{
    int **arr = numbers_to_bits(big);
    if (value != 1 && value != 0) {
        printf("ERROR\n");
        return -1;
    }

    int i = 0, j = 0;
    if (y < 4) {
        i = 0;
        j = x + y * 8;
    } else {
        i = 1;
        j = x + (y - 4) * 8;
    }

    arr[i][j] = value;
    //printf("AAA%d\n", arr[i][j]);
    big = bits_to_numbers(arr, big);
    free(arr);
    //printf("BIG  = %d  %d\n", big[0], big[1]);

    return 0;
}

int bc_getbigcharpos(int *big, int x, int y, int *value)
{
    //printf("BIG2  = %d  %d\n", big[0], big[1]);


    int **arr = numbers_to_bits(big);
    if (value == NULL)
        return -1;

    if (y < 4)
        *value = arr[0][x + y * 8];
    else
        *value = arr[1][x + (y - 4) * 8];

    free(arr);
    return 0;
}

int bc_bigcharwrite(char *fname, int *big, int count)
{
    FILE *f = fopen(fname, "wb");

    if (f == NULL) {
        return -1;
    }

    fwrite(big, sizeof(int) * 2, count, f);

    fclose(f);

    return 0;
}

int bc_bigcharread(char *fname, int *big, int need_count, int *count)
{
    FILE *f = fopen(fname,"rb");

    if (f == NULL) {
        *count = 0;
        return -1;
    }

    fread(big, sizeof(int) * 2, need_count, f);

    fclose(f);

    return 0;
}

#endif
