#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // fork()
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <glob.h>


#define INPUT_BUFFER_SIZE 255

struct command{
	char *command;
	int commandc;
	struct arg *commandv;
};

struct arg
{
	char *arg;
	int argc;
	char **argv;
};

typedef struct command Command;
typedef struct arg Argument;

void printCommand(Command c);
void printArgument(Argument a);
char *trimString(char *string);
void setSignal(int mode);
Command tokenizeCommand(char *input);
Argument tokenizeArguments(char *input);
void wildcast(Command c);
void suspendProcesses(pid_t *pidList, int count);
void runPipeCommand(Command c);

#endif
