#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("argv[%f] = %s \n", i, argv[i]);
        bozo;
    }
    return 0;
}