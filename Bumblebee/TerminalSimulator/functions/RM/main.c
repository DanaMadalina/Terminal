#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void parseComma(const char *command, int *hasVerbose, int *hasInteractive, int *hasEmptyDirectory){

    for(int i = 1; i < strlen(command); i++)
        switch(command[i]){
            case 'i':{
                if(!*hasInteractive)
                    *hasInteractive = !*hasInteractive;
                break;
            };

            case 'v':{
                if(!*hasVerbose)
                    *hasVerbose = !*hasVerbose;
                break;
            };

            case 'd':{
                if(!*hasEmptyDirectory)
                    *hasEmptyDirectory = !*hasEmptyDirectory;
                break;
            };

            default:{
                fprintf(stdout, "The command '%c' does not exist.\n", command[i]);
                exit(0);
            };
        };
}


void setCom(int argc, char **args, int *hasVerbose, int *hasInteractive, int *hasEmptyDirectory, char **path){

    for(int i = 0; i < argc; i++)
        if(args[i][0] == '-')
            parseComma(args[i], hasVerbose, hasInteractive, hasEmptyDirectory);

        else
            if(*path == NULL){

                /// Seteaza calea catre fisierul sau folderul ce va fi sters.
                    *path = (char *)calloc(strlen(args[i]), sizeof(char));
                    strcpy(*path, args[i]);
            }
}

int getFileType(const char* path){

    /// Fisier -> 1;
    /// Folder -> 2;
    /// Nu exista -> 3;

    FILE *file = NULL;
    DIR *folder = NULL;

    if((folder = opendir(path))){

        closedir(folder);
        return 2;
    };

    if((file = fopen(path, "r"))){

        fclose(file);
        return 1;
    };

    return 3;
}

int isFolderEmpty(const char *path){

    /// isEmpty -> 1;
    /// isNotEmpty -> 0;

    int isEmpty = 1, fileIndex = 0;
    DIR *folder = opendir(path);

    /// Sari peste . si ..;
    while(readdir(folder) && fileIndex <= 2)
        ++fileIndex;

    if(fileIndex > 2)
        isEmpty = 0;

    return isEmpty;
};

int rm(int argc, char **argv) {

    int hasInteractive = 0,
        hasEmptyDirectory = 0,
        hasVerbose = 0,
        successfullyDeleted = 1;

    char *path = NULL; /// Se va verifica mai tarziu daca e folder sau daca e fisier.

    ///Seteaza comenzile...
    setCom(argc, argv, &hasVerbose, &hasInteractive, &hasEmptyDirectory, &path);

    /// Verifica ce tip e la calea specificata. Fisier, folder sau nu exista.

    switch(getFileType(path)){

        case 1: /* E fisier... */ {

            if(hasInteractive){

                ///Show the message.
                fprintf(stdout, "Delete '%s'? ", path);

                /// Prompt option.
                char selectedOption = fgetc(stdin);

                if((selectedOption == 'y' || selectedOption == 'Y'))
                    remove(path);
                else
                    successfullyDeleted = 0;

            } else
                remove(path);

            break;
        };

        case 2 : /* E folder */ {
            if(hasEmptyDirectory)
                if(!isFolderEmpty(path)){

                    fprintf(stdout, "The folder '%s' is not empty.\n", path);
                    exit(4);
                } else {

                    if(hasInteractive){

                        /// Print message.
                        fprintf(stdout, "Delete empty folder '%s'? ", path);

                        /// Get response.
                        char selectedOption = fgetc(stdin);

                        if(selectedOption == 'y' || selectedOption == 'Y')

                            rmdir(path);
                        else
                            successfullyDeleted = 0;
                    } else
                        rmdir(path);
                }
            else {

                fprintf(stdout, "'%s' is a folder.\n", path);
                exit(5);
            }

            break;
        };

        default: /* Fisierul sau folderul nu exista. */ {

            fprintf(stdout, "The file or folder '%s' does not exist at the specified location.\n", path);
            exit(3);
        };
    }

    /// Afiseaza verbose.
    if(hasVerbose && successfullyDeleted)
        fprintf(stdout, "Deleted '%s'.\n", path);

    /// Sterge pointerii.
    free(path);

    return 0;
}
