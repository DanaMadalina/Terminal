#include <stdio.h>
#include <stdlib.h>

int echo (int argc, char ** argv) {

    char *currentChar = 0;

        while ((currentChar = fgetc(stdin) != EOF))
            printf("%c", currentChar);


    return 0;
}
