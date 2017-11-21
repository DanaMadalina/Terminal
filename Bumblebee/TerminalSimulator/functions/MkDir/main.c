#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>



void setComm(const char *command, int *hasVerbose, int *createWithoutError){

    for(int i = 1; i < strlen(command); i++)
        switch(command[i]){

            case 'v': {
                if(!*hasVerbose)
                    *hasVerbose = !*hasVerbose;
                break;
            };

            case 'p': {
                if(!*createWithoutError)
                    *createWithoutError = !*createWithoutError;
                 break;
            };

            default: {

                fprintf(stdout, "The command '%c' does not exist.\n", command[i]);
                exit(2);
            };
        };
}


int folderAlreadyExists(const char *folderName){

    DIR *folder = opendir(folderName);

    if(folder){

        closedir(folder);
        return 1;
    };

    return 0;
}

void executeComm(int hasVerbose, int createWithoutError, const char *folderName, int *Status){

    mode_t n = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;

    if(folderAlreadyExists(folderName)){
        if(createWithoutError)
            if(mkdir(folderName, n) == 0){
                *Status = 1;
            } else
                *Status = 0;
        else {

            fprintf(stdout, "Folder already exists.\n");
            exit(*Status = 3);
        }
    } else
        if(mkdir(folderName, n) == 0){
                *Status = 1;
            } else
                *Status = 0;
}

int mkdirr(int argc, char **argv) {

    int hasVerbose = 0,
        createWithoutError = 0,
        i = 0,
        successfullyCreated = 1;

    char *folderName = NULL;

    /// Verifica argumentele. Daca > 1, seteaza comenzile programului.

        for(i = 0; i < argc; i++)
            if(argv[i][0] == '-')

                setComm(argv[i], &hasVerbose, &createWithoutError);
            else
                if(!folderName)
                    strcpy(folderName = (char *) calloc(strlen(argv[i]) + 1, sizeof(char)), argv[i]);

    /// Executa comenzile.
    executeComm(hasVerbose, createWithoutError, folderName, &successfullyCreated);

    ///Afiseaza verbose-ul.
    if(hasVerbose)
        fprintf(stdout, "%s", folderName);

    /// Elibereaza memoria.
    free(folderName);

    return successfullyCreated;
}
