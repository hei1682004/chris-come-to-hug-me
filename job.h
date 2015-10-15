#ifndef JOB_H_INCLUDED
#define JOB_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#include "command.h"

typedef struct jobs {
    char cmd[256];
    pid_t *pidList;
    struct jobs *next;
} Jobs;

void doFg(Argument a);
void initJob();
void printJobList();
void jobsNewNode(pid_t *pidList, char *command);
void jobsDelNode(int jobID);
int findJob(int jobID);

#endif