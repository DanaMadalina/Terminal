#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parseCommand(const char *command, int *hasDoNotCreate, int *hasIgnored, int i){

    for(i = 1; i < strlen(command); i++){
        switch(command[i]){

            case 'c': {
                if(!*hasDoNotCreate)
                    *hasDoNotCreate = !*hasDoNotCreate;
                break;
            };

            case 'f': {
                if(!*hasIgnored)
                    *hasIgnored = !*hasIgnored;
                break;
            };

            default:{
                fprintf(stdout, "The command '%c' does not exist.\n", command[i]);
                exit(1);
            };
        }
    }
};


void setCommands(int argc, char **argv, int *hasDoNotCreate, int *hasIgnored, int i, char **filePath){

    for(i = 0; i < argc; i++)
        if(argv[i][0] == '-')

            parseCommand(argv[i], hasDoNotCreate, hasIgnored, i);
        else
            if(!*filePath){

                *filePath = (char *)calloc(strlen(argv[i]) + 1, sizeof(char));

                strcpy(*filePath, argv[i]);
            };

}

void runCommands(int hasDoNotCreate, int hasIgnore, char *filePath){

    FILE *fisier = fopen(filePath, "r");

    /// Verifica daca fisierul trebuie creat.
    if(!hasDoNotCreate)
        if(fisier == NULL)
            fclose(fopen(filePath, "w"));

    if(fisier)
        fclose(fisier);

}

int touch (int argc, char **argv) {

    int hasIgnore = 0,
        hasDoNotCreate = 0,
        i = 0;

    char *fileName = NULL;

    /// Seteaza comenzile.
    setCommands(argc, argv, &hasDoNotCreate, &hasIgnore, i, &fileName);

    /// Executa comenzile.
    runCommands(hasDoNotCreate, hasIgnore, fileName);

    /// Elibereaza.
    free(fileName);

    return 0;
}
