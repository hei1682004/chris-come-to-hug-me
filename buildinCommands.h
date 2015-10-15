#ifndef BUILD_INCOMMANDS_H_INCLUDED
#define BUILD_INCOMMANDS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "job.h"

extern char *buildinCommandList[INPUT_BUFFER_SIZE];

int isBuildinCommand(Argument a);
int runBuildinCommands(Argument);
void doCd(Argument);
void doExit(Argument);

#endif