

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>

void seteazaActiunea(const char *comanda, int *indexes, int *dots, int *column){

    for(int i = 1; i < strlen(comanda); i++){
        switch(comanda[i]){

            case 'i':
                if(!*indexes){
                    *indexes = 1;
                    }
                break;

            case 'a':
                if(!*dots){
                    *dots = 1;
                }
                break;

            case '1':
                if(!*column){
                    *column = 1;
                }
                break;

            default:
                printf("LS: The option '%c' doesn't exist.\n", comanda[i]);
                exit(1);
        }
    }
}

int ls (int argc, char* argv[]){


    DIR *folder = NULL;
    struct stat file_stats;
    struct dirent *file;

    int hasColumn = 0,
        hasIndex = 0,
        hasDots = 0;

    printf ("executing LS! \n");

    if(argc > 1){

        for(int i = 1; i < argc; i++)

            if(argv[i][0] == '-')
                seteazaActiunea(argv[i], &hasIndex, &hasDots, &hasColumn);
            else {
                    if(!(folder = opendir(argv[i]))){
                        printf("Folder '%s' does not exist.", argv[i]);
                        exit(1);
                    }
            }
        }

    if(!folder)
        folder = opendir(getcwd(NULL, 1024));

    while((file = readdir(folder))){

        stat(file -> d_name, &file_stats);

        if(( !strcmp(file -> d_name, ".") || !strcmp(file -> d_name, "..")) && !hasDots)
            continue;

        if(hasIndex)
            printf("%d ", file -> d_ino);

        printf("%s ", file -> d_name);

        if(hasColumn)
            printf("\n");

    }

    if(!hasColumn)
        fprintf(stdout, "\n");

    closedir(folder);
    exit(0);
}
