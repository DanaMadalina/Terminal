#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

void seteazaComanda(const char *comanda, int *hasVerbose, int *hasForce, int *hasPrompt){

    for(int i = 1; i < strlen(comanda); i++)
        switch(comanda[i]){

            case 'v':{
                if(!*hasVerbose)
                    *hasVerbose = 1;
                break;
            };

            case 'f':{
                if(!*hasForce)
                    *hasForce = 1;
                break;
            };

            case 'i':{
                if(!*hasPrompt)
                    *hasPrompt = 1;
                break;
            };

            default:{
                fprintf(stdout, "The command '%c' does not exist.\n", comanda[i]);
                exit(0);
            }
        }
}

int mv(int argc, char **argv) {

    int hasVerbose = 0,
        hasForce = 0,
        hasPrompt = 0,
        i = 0,
        overWritten = 1;

        DIR *destinationDirectory = NULL;

        char *source = NULL,
            *destination = NULL,
            *newFilePath = NULL,
            *finalPath;

    switch(argc){
        case 0: {
            fprintf(stdout, "The source file is missing.\n"); exit(1);
        };

        case 1: {
            fprintf(stdout, "The destination file or folder is missing.\n"); exit(2);
        };

        default:

            for(i = 0; i < argc; i++)
                if(argv[i][0] == '-')
                    seteazaComanda(argv[i], &hasVerbose, &hasForce, &hasPrompt);

            strcpy(source = (char *)calloc(strlen(argv[1]), sizeof(char)), argv[1]);
            strcpy(destination = (char *)calloc(strlen(argv[2]), sizeof(char)), argv[2]);

        }

    /// Verifica daca fisierul sursa exista.
    if(!fopen(source, "r")){
        fprintf(stdout, "The file '%s' doesn't exist.\n", source);
        exit(3);
    }

    /// Verifica daca sursa si destinatia sunt aceleasi.
    if(!strcmp(source, destination)){
        fprintf(stdout, "The source and the destination files are the same.\n");
        exit(4);
    }

    ///Verifica daca destinatia e un folder. Daca da, creeaza destinatia finala a fisierului tinta.
    if((destinationDirectory = opendir(destination))){
        newFilePath = (char *)calloc(strlen(destination) + basename(source), sizeof(char));
        strcat(newFilePath, destination);

        if(destination[strlen(destination) - 1] != '/')
            strcat(newFilePath, "/");

        strcat(newFilePath, basename(source));

        closedir(destinationDirectory);

    }

    if(newFilePath)
        strcpy(finalPath = (char *)calloc(strlen(newFilePath), sizeof(char)), newFilePath);
    else
        strcpy(finalPath = (char *)calloc(strlen(destination), sizeof(char)), destination);

    /// Daca nu e fortat, verifica daca fisierul exista.
    /// Daca exista si are propmt pentru overwrite, afiseaza prompt-ul, altfel
    /// suprascrie.

    /// Daca e fortat, muta direct.
    if(!hasForce){

        if(fopen(finalPath, "r"))

            if(hasPrompt){

                fprintf(stdout, "File '%s' already exists. Overwrite it? [y/n] ", finalPath);

                if(fgetc(stdin) == 'y')
                    rename(source, finalPath);
                else
                    overWritten = 0;


            } else
                rename(source, finalPath);
        else
            rename(source, finalPath);


    } else
        rename(source, finalPath);

    if(hasVerbose && overWritten)
        fprintf(stdout, "'%s' -> '%s'\n", source, finalPath);

    return 0;
}
