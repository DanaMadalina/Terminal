#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

void seteazaAct(const char *comanda, int *hasWords, int *hasLines, int *hasChars, char *ordine){

    for(int i = 1; i < strlen(comanda); i++){
        switch(comanda[i]){

            case 'w':
                if(!*hasWords){
                    *hasWords = 1;
                    strcat(ordine, "w");
                    }
                break;

            case 'l':
                if(!*hasLines){
                    *hasLines = 1;
                    strcat(ordine, "l");
                }
                break;

            case 'c':
                if(!*hasChars){
                    *hasChars = 1;
                    strcat(ordine, "c");
                }
                break;

            default:
                printf("WC: The option '%c' doesn't exist.\n", comanda[i]);
                exit(1);
        }
    }

    if(!(*hasChars || *hasLines || *hasWords))
        *hasChars = *hasLines = *hasWords = 1;
}

int wc (int argc, char **argv) {

int hasWords = 0,
        hasLines = 0,
        hasChars = 0,
        i = 0,
        fileIndex = 0,
        numarLinii = 0,
        numarCuvinte = 0,
        numarCaractere = 0;

    char *ordine = calloc(3, sizeof(char)),
        currentChar = NULL;

    FILE *file = NULL;


    /// Parcurge argumente.
    for(i = 0; i < argc; i++){

        if(argv[i][0]== '-')
            seteazaAct(argv[i], &hasWords, &hasLines, &hasChars, ordine);
        else
            if(!(file = fopen(argv[fileIndex = i], "r"))){
                fprintf(stdout, "The file '%s' does not exist or cannot be opened.\n", argv[fileIndex]);
                exit(1);
            }
    }

    if(!file)
        exit(1);//file = stdin;

    if(!hasChars && !hasLines && !hasWords) {
        hasChars = hasLines = hasWords = 1;

        ordine[0] = 'l';
        ordine[1] = 'w';
        ordine[2] = 'c';
    }

    while((currentChar = fgetc(file)) != EOF){

        switch(currentChar){
            case '\n':
                if(hasLines)
                    numarLinii++;

                if(hasWords)
                    numarCuvinte++;

                break;
            case ' ':
                if(hasWords)
                    if(currentChar)
                        numarCuvinte++;
                break;

            default:
                if(hasChars)
                    numarCaractere++;
        }
    }

    if(numarCaractere || numarLinii || numarCuvinte)
        numarCaractere += 3;

    if(file == stdin)
        numarCaractere--;


    for(int i = 0; i < strlen(ordine); i++)
        switch(ordine[i]){
            case 'l': fprintf(stdout,"%d ", numarLinii); break;

            case 'c': fprintf(stdout,"%d ", numarCaractere); break;

            case 'w': fprintf(stdout,"%d ", numarCuvinte); break;
        }



    fprintf(stdout, "%s\n", file == stdin ? "stdin" : argv[fileIndex]);

    /// Sterge pointerii.
    free(ordine);

    /// Inchide fisierele.
    if(file) fclose(file);

    return 0;
}
