#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdbool.h>

#include <unistd.h>
#include "function_ls.h"
#include "function_wc.h"
#include "function_touch.h"
#include "function_rm.h"
#include "function_mkdir.h"
#include "function_mv.h"
#include "function_echo.h"


#define INPUT_COMMAND 1024  /// the input command

void  main()
{
    char  command [INPUT_COMMAND];        /// the input command

    printf("\nCommands: ");
    printf("\n Exit, \n clear, \n version, \nls (-i -a -1), \n mv(-v -f -i), \n mkdir(-v -p), \n rm(-i -v -d), \n touch(-c -f),  \n wc(-l -c -w)");
    printf("\n");
    while (1)
    {
        printf("\nBumblebee >> ");                 /// the prompt
        gets(command);                           /// read in the command line
        printf("\n");


		///Signals to catch the Ctrl-C and Ctrl/ combination
        signal(SIGINT, SIG_IGN);       	        ///The instructions said to ignore the SIGINT signal
        signal(SIGTERM, SIG_DFL);               ///SIGTERM signal must be caught.


        ///process command -
        processCommand(command);

     }
}




