#include <stdio.h>

void me()
{

    printf("me");
}

void he()
{

    printf("he");
}

void she()
{
    printf("she");
}



void (*funcs[4])();
void (*funcs[4])() = {me, he, she};

int main()
{
    funcs[0]();
    funcs[1]();
    funcs[2]();
}