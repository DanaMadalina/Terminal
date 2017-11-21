#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sys/types.h"
#include "unistd.h"
#include <fcntl.h>

#include "Functions.h"
#include "function_ls.h"
#include "function_touch.h"
#include "function_rm.h"
#include "function_mkdir.h"
#include "function_mv.h"
#include "function_wc.h"
#include "function_echo.h"



typedef int bool;
#define true 1
#define false 0


typedef struct c{
                    int commandID;
                    char *parameters[4];
                    int paramCount;
                    bool append;
                    char *stdout;
                    char *stdin;
                } cmd;


char *validCommands[] = {"ls",
                        "echo",
                        "clear",
                        "touch",
                        "help",
                        "wc",
                        "mv",
                        "rm",
                        "mkdirr"};

/// struct used to link a command to its execution function
typedef struct i
{
    char *commandName;
    int (*execFct)(int,char*[]);
} cmdExecution;


#define NB_VALID_COMMANDS  11
#define EXECUTE_COMMAND(x,y,z) x(y,z)

const char delimitators[5] = ";|&";

cmd extractCommand(int* index, char *command);
bool InputValidity(char *argv);
int executeCommand(cmd *command);
void printCommand (cmd *command);
int resolveCommand(char* commandName);
char* getBufferFromFile(char* fileName);
void clearFileContents(char* fileName);


/// local implemented commands:
int clear (int argc, char* argv[]);
int Exit (int argc, char* argv[]);
int help (int argc, char* argv[]);
int version (int argc, char* argv[]);

/// array containing all known commands with the corresponding execution functions
cmdExecution execFunctions[] = {
                            {"ls", &ls},
                            {"clear", &clear},
                            {"Exit", &Exit},
                            {"help", &help},
                            {"version", &version},
                            {"touch", &touch},
                            {"wc", &wc},
                            {"mv", &mv},
                            {"mkdirr", &mkdirr},
                            {"rm", &rm},
                            {"echo", &echo}
                            };

char pipeFile[] = "pipe.out";
char pipeFileIn[] = "pipe.in";

void copyFile(char* sourceFileName, char* destFileName)
{
	FILE *sourceFile = fopen (sourceFileName, "r");
	FILE *destFile = fopen (destFileName, "w");

    if ((sourceFile != NULL) && (destFile != NULL))
    {
		char character;
		/// copy each character from sourceFile to destFile
        do {
			character = fgetc(sourceFile);
			fputc(character, destFile);
			} while (character != EOF);

        fclose (sourceFile);
		fclose (destFile);
    }
}

int processCommand (char *command)
{
    int index = 0;
    int retVal = 0;
    cmd cmdExec;
    bool exitCommand = false;
    bool hasPipeParam = false;

    /// until we reach the end of command line
    while (index != strlen(command))
    {
        cmdExec = extractCommand(&index, command); ///parse the command line and extract one command
        printCommand(&cmdExec);

        if(hasPipeParam)
        {
            copyFile(pipeFile, pipeFileIn);
            cmdExec.parameters[cmdExec.paramCount] = malloc(strlen(pipeFileIn));
            printf("paraC = %d, pipeFI = %d , p_pipeFI = %l", cmdExec.paramCount, strlen(pipeFileIn), pipeFileIn );
			strcpy(cmdExec.parameters[cmdExec.paramCount], pipeFileIn);
			cmdExec.paramCount++;

			hasPipeParam = false;
			printCommand(&cmdExec);
        }

        /// check if extracted command is valid
        if (cmdExec.commandID < 0)
        {
            printf("command %d is not valid \n", cmdExec.commandID);
            retVal = 1;
            break;
        }

        /// check what is after the command
        switch(command[index])
        {
            case '\0':
                {
                    /// just execute the command - it is last one
                    retVal = executeCommand(&cmdExec);
                    break;
                }
            case '&':
                {
                    index++;
                    /// check if && is present
                    if(command[index] == '&')
                    {
                        /// execute the command and stop if negative result
                        index++;
                        retVal = executeCommand(&cmdExec);
                        if(retVal != 0)
                        {
                            printf("Command %s failed, aborting execution!", execFunctions[cmdExec.commandID].commandName);
                            exitCommand = true;
                        }

                    }
                    else
                    {
                        printf("Invalid command separator: & please use && !");
                        exitCommand = true;
                    }
                    break;
                }
            case '|':
                {
                    index++;
                    /// if || or simple pipe
                    if(command[index] == '|')
                    {
                        /// execute the command and stop if positive result
                        index++;
                        retVal = executeCommand(&cmdExec);
                        if(retVal == 0)
                        {
                            printf("Command %s failed, aborting execution!", execFunctions[cmdExec.commandID].commandName);
                            exitCommand = true;
                        }
                    }
                    else
                    {
                        /// pipe logic: the output of previous command is cached and sent
                        /// to the next command as a parameter

                        /// if there is no output file defined - redirect everything to pipefile
                        if(cmdExec.stdout == NULL)
                        {
                            cmdExec.stdout = malloc(strlen(pipeFile));
                            strcpy(cmdExec.stdout, pipeFile);
                            //cmdExec.stdout = pipeFile;
                            hasPipeParam = true;
                        }

                        retVal = executeCommand(&cmdExec);
                    }
                    break;
                }
            case ';':
                {
                    /// just execute the command - no restrictions
                    index++;
                    retVal = executeCommand(&cmdExec);
                }

        }
        printf("\n processCommand index = %d command = %d \n", index, strlen(command));
        /// clear command data to avoid memory leaks
        for(int i = 0; i < cmdExec.paramCount; i++)
        {
            free(cmdExec.parameters[i]);
        }

        if(exitCommand)
            break;

    }
    return retVal;
}

cmd extractCommand(int* index, char *command)
{
    char *temp, *commandWithoutRedirection, *param, *auxCommand, *firstCommand;
    cmd execCmd; ///command to execute after parse
    execCmd.paramCount = 0;
    execCmd.append = false;
    execCmd.stdout = NULL;
    execCmd.stdin = NULL;


    printf("extractCommand: index = %d  command = %s \n", *index, command);
    auxCommand = strdup(command);
    /// extract command until delimitator
    firstCommand = strtok((auxCommand + *index), delimitators);
    commandWithoutRedirection = strdup(firstCommand);
    *index += strlen(firstCommand);
    printf("extractCommand: commandWithoutRedirection = %s \n index = %d firstCommand = %s leftover = %s \n", commandWithoutRedirection, *index, firstCommand, command + *index);

    temp = strdup(strtok(commandWithoutRedirection, "><"));

    /// extract command name
    char *commandName = strdup(strtok(temp, " "));
    execCmd.commandID = resolveCommand(commandName);

    printf("extractCommand: id:%d length = %d \n", execCmd.commandID, strlen(commandName));

    /// extract parameters of command
    param = strtok(NULL, " ");
    printf("extractCommand: param = %s \n", param);

    while (param != NULL)
    {
        execCmd.parameters[execCmd.paramCount++] = strdup(param);
        param = strtok(NULL, " ");
        printf("extractCommand: param = %s \n", param);
    }

    /// check if redirection is needed
    if(commandWithoutRedirection != NULL)
    {
        int auxIndex = strlen(commandWithoutRedirection);
        if(firstCommand[auxIndex] == '>')
        {
            char *auxStr;
            auxStr = strstr(firstCommand, "<");
            /// check if append needed
            if(firstCommand[auxIndex + 1] == '>')
            {
                execCmd.append = true;
                auxIndex++;
            }
            execCmd.stdout = strtok(firstCommand + auxIndex + 1, " ");


            if(auxStr != NULL)
            {
            printf("extractCommand: > found \n");
                execCmd.stdin = strtok(auxStr + 1, " ");
            }
        }
        else if(firstCommand[auxIndex] == '<')
        {
            char *auxStr;
            auxStr = strstr(firstCommand, ">");

            execCmd.stdin = strtok(firstCommand + auxIndex + 1, " ");


            if(auxStr != NULL)
            {
                int ind = 1;
                /// check if append needed
                if(auxStr[1] == '>')
                {
                    execCmd.append = true;
                    ind++;
                }
                execCmd.stdout = strtok(auxStr + ind, " ");
            }
        }
    }

    free(auxCommand);
    free(commandWithoutRedirection);
    free(temp);
    free(commandName);

    return execCmd;

}


bool InputValidity(char *argv)
{
    bool fit = false;
	int i = 0;

	for(i = 0; i < NB_VALID_COMMANDS; i++)
    {
		if(strcmp(validCommands[i], argv) == 0)
		{
			fit = true;
			break;
        }
	}
	return fit;
}

int clear (int argc, char* argv[])
{
    printf("clear called");
    /// special character used by unix terminals - escape sequence
    printf("\033c" );
}

int Exit (int argc, char* argv[])
{
    printf("User called exit");
    exit(0);
}

int help (int argc, char* argv[])
{
    printf("help called");
}

int version (int argc, char* argv[])
{
    printf("\nVersion Terminal: 1.0\n");
    printf("\nAuthors: \n\t Enachiuc Dana-Madalina, \n\t Costescu Cristi, \n\t Doros Romina\n");
}

int resolveCommand(char* commandName)
{
    int cmd_id = -1;
    for(int i=0; i < NB_VALID_COMMANDS; i++)
    {
        if(strcmp(commandName, execFunctions[i].commandName) == 0)
        {
            cmd_id = i;
            break;
        }
    }

    return cmd_id;
}

void printCommand (cmd *command)
{
    printf("name: %s", execFunctions[command->commandID].commandName);
    for (int i = 0; i < command->paramCount; i++)
    {
        printf(" param %d: %s ", i, command->parameters[i]);
    }
    printf(" append: %d", command->append);
    printf(" stdin: %s stdout: %s \n", command->stdin, command->stdout);
}

int executeCommand(cmd *command)
{
    int retVal = 1;

    /// special case for Exit as no new process needed
    if(strcmp(execFunctions[command->commandID].commandName, "Exit") == 0)
        Exit(command->paramCount, command->parameters);

    pid_t pid = vfork();
    if(pid == 0)
    {
        int fdO, fdI;
        /// if different output device wanted
        if(command->stdout != NULL)
        {
            /// The function fileno() examines the argument stream and returns its integer descriptor.
            /// stdout = standard output
            close(fileno(stdout));
            if(command->append)
                fdO = open(command->stdout, O_WRONLY|O_CREAT|O_APPEND, 0600);
            else
                fdO = open(command->stdout, O_WRONLY|O_CREAT|O_TRUNC, 0600);
            dup2(fileno(stdout), fdO);
        }
        if(command->stdin != NULL)
        {
            close(fileno(stdin));
            printf("file contains: %s", getBufferFromFile(command->stdin));
            command->parameters[command->paramCount++] = getBufferFromFile(command->stdin);
            printCommand(command);
            //fdI = open(command->stdin, O_RDONLY, 0600);
            //dup2(fileno(stdin), fdI);
        }

        execFunctions[command->commandID].execFct(command->paramCount, command->parameters);

        if(command->stdin != NULL)
            close(fdI);
        if(command->stdout != NULL)
            close(fdO);

    }
    else if (pid < 0)
    {
        printf("new process could not be created!");
    }
    waitpid(pid, &retVal, 0);
    printf("Exit status was %d\n", retVal);

    return retVal;
}

char* getBufferFromFile(char* fileName)
{
    FILE *fd = fopen (fileName, "rb");
    char* data;
    long size;

    if (fd)
    {
        /// check the size od file data
        fseek (fd, 0, SEEK_END);
        size = ftell (fd);

        /// go to the begining of the file and copy all data
        fseek (fd, 0, SEEK_SET);
        data = malloc (size);
        if (data != NULL)
        {
            fread (data, 1, size, fd);
        }
        fclose (fd);
    }
    return data;
}

void clearFileContents(char* fileName)
{
    FILE *fd = fopen(fileName, "w");
    fclose(fd);
}




