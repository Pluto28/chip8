#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <string.h>
#include <time.h>

// get value of the rflags
extern uint8_t flags(void);

int sp = 0;
int stack[10];

void push(int el) 
{
    stack[sp] = el;
    ++sp;
}

int pop(void)
{
    return (stack[--sp]);
}

int main()
{
    time_t time = clock();
    char ia[10];
    uint8_t y = 234;

    getrandom(&ia[0], 1, 0x0);
    
    srandom(ia[0]);
    long foda_se = rand();


    uint8_t digits[3];

    int i;
    // store separate digits into the digits array
    for (i = 2; i > 0; i)
    {
        digits[--i] = y % 10;
        //printf("%i %i\n", i, y % 10);
        y /= 10;
    }
    
    uint8_t vx = 0x7;
    //printf("%i %i\n", i, y % 10);
    for (;i < vx; i++)
    { 
        printf("%i %i\n", i, vx);
    }
    printf("%i %i\n", i, vx);

    time = clock() - time;
    double time_taken = ((double)time)/CLOCKS_PER_SEC; 
    
    printf("%f", time_taken);
}