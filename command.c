#include "command.h"

char envPaths[] = "/bin:/usr/bin:.";

void printCommand(Command c){
	int i;
	printf("command: %s\ncommandc: %d\ncommandv:\n", c.command, c.commandc);
	for(i=0; i<c.commandc; i++){
		printf("  ");
		printArgument(c.commandv[i]);
	}
}

void printArgument(Argument a){
	int i;
	printf("%s: argc: %d, argv:", a.arg, a.argc);
	for(i=0; i<a.argc; i++)
		printf("%s, ", a.argv[i]);
	printf("\n");
}

/*char *trimString(char *string){
	unsigned i;
	char *newString;

	printf("trimString:\n=============\n");

	while(isspace(*string)) // trim space in string head
		string++;

//getchar();

	// --> trim space and '\n' in string
	newString = malloc(sizeof(char) * (strlen(string) + 1));
printf("sizeof(char): %d, strlen(string)+1: %d\n", sizeof(char), strlen(string) + 1);
//getchar();
	for(i=0; i<strlen(string) + 1; i++)
		newString[i] = '\0';

//getchar();

printf("string: %s ||| strlen(string): %d\n", string, strlen(string));

//getchar();

	for(i=0; i<strlen(string); i++){
		if(i > 0)
			if((isspace(string[i-1]) && isspace(string[i])) || (string[i] == '\n'))
				continue;

//getchar();

printf("newString: %s ||| strlen(newString): %d\n", newString, strlen(newString));

		newString[strlen(newString)] = string[i];
	}

printf("newString: %s ||| strlen(newString): %d\n", newString, strlen(newString));
	// <-- END! trim space and '\n' in string

	if(newString[strlen(newString) - 1] == ' ') // trim space character in the last index of string
		newString[strlen(newString) - 1] = '\0';
	else
		newString[strlen(newString)] = '\0';

printf("final newString: <%s>\n", newString);

	return newString;
}*/


char *trimString(char *string){
	int i, j, isLast;
	while(isspace(*string)) // trim space in string head
		string++;

	for(i=0; i<strlen(string); i++){
		if(string[i] == ' '){
			isLast = 1;
			for(j=i+1; j<strlen(string); j++){
				if(string[j] != ' ' && string[j] != '\n'){
					isLast = 0;
					break;
				}
			}
			if(isLast){
				string[i] = '\0';
				break;
			}
		}
	}
	if(string[strlen(string) - 1] == '\n')
		string[strlen(string) - 1] = '\0';
	return string;
}

void setSignal(int mode){ // mode : 1=enable signals, 0=disable signals
	if(mode == 0){ // Disable signals
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGTERM, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);
	}
	else{ // Enable signals
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGTERM, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
	}

	//--> Signal with default handlers
	signal(SIGSTOP, SIG_DFL);
	signal(SIGKILL, SIG_DFL);
	//<-- END! Signal with default handlers
}

Command tokenizeCommand(char *input){
//printf("tokenizeCommand\n=================\n");
	int i;
	int commandc = 0;
	Argument *commandv;
	Command *command = (Command *) malloc(sizeof(Command));

	if(strlen(input) == 0){
		commandv = NULL;
		commandc = 0;
		command->command = input;
		command->commandc = 0;
		command->commandv = NULL;
		return *command;
	}

	for(i=0; i<strlen(input); i++)
		if(input[i] == '|')
			commandc++;
	commandc++;

	commandv = (Argument *) malloc(sizeof(Argument) * commandc);

	char *inputBackup = (char *) malloc(sizeof(char) * strlen(input));
	memcpy(inputBackup, input, strlen(input));

	char **subCommands = (char **) malloc(sizeof(char *) * commandc);
	char *token = strtok(input, "|");
	int count = 0;

	while(token != NULL){
		//printf("token: <%s>\n", token);
		subCommands[count] = (char *) malloc(sizeof(char) * (strlen(token) + 1));
		for(i=0; i<(strlen(token) + 1); i++)
			subCommands[count][i] = '\0';
		memcpy(subCommands[count++], token, strlen(token) + 1);
		token = strtok(NULL, "|");
//getchar();
	}
	for(i=0; i<commandc; i++)
		commandv[i] = tokenizeArguments(subCommands[i]);
//getchar();
	command->command = inputBackup;
	command->commandc = commandc;
	command->commandv = commandv;
//getchar();
	return *command;
}

Argument tokenizeArguments(char *input){
	int i;
	int argc = 0;
	char **argv;
	Argument *a = (Argument *) malloc(sizeof(Argument));

//printf("tokenizeArguments\n================\n");

	input = trimString(input);
	//printf("argument: %s\n", input);

//getchar();

	if(strlen(input) == 0){
		argv = NULL;
		argc = 0;
		a->argc = argc;
		a->argv = argv;
		return *a;
	}

	for(i=0; i<strlen(input); i++)
		if(input[i] == ' ')
			argc++;
	argc++;

//getchar();

	argv = (char **) malloc(sizeof(char*) * (argc + 1));

	char *inputBackup = (char *) malloc(sizeof(char) * INPUT_BUFFER_SIZE);
	for(i=0; i<INPUT_BUFFER_SIZE; i++)
		inputBackup[i] = '\0';
	memcpy(inputBackup, input, strlen(input) * sizeof(char));

//getchar();

	char *token = strtok(input, " ");
	int count = 0;
	while(token != NULL){
		//printf("%s\n", token);
		argv[count] = (char *) malloc(sizeof(char) * INPUT_BUFFER_SIZE);
		for(i=0; i<INPUT_BUFFER_SIZE; i++)
			argv[count][i] = '\0';
		memcpy(argv[count++], token, strlen(token) * sizeof(char));
		token = strtok(NULL, " ");
	}

	argv[count] = NULL;

//getchar();

	a->arg = inputBackup;
	a->argc = argc;
	a->argv = argv;

	return *a;
}

/*
void wildcast(Command c){
	int i, matchCount = 0, j, k;

	for(i = 0; i<c.commandc; i++){
		Argument a = c.commandv[i];
		glob_t globBuffer;
		for(j = 0; j < a.argc; j++){
			char *newArgv = a.argv[j];
			if(matchCount == 0)
				glob(newArgv, GLOB_NOCHECK, NULL , &globBuffer );
			else
				glob(newArgv, GLOB_NOCHECK|GLOB_APPEND, NULL , &globBuffer );
			matchCount = globBuffer.gl_pathc;
		}

		free(c.commandv[i].argv);

		c.commandv[i].argv = (char**)malloc(sizeof(char*) * (matchCount+1));

		for(k = 0; k < matchCount; k++){
			a.argv[k] = (char*)malloc(sizeof(char)*strlen(globBuffer.gl_pathv[k]));
			memcpy(a.argv[k], globBuffer.gl_pathv[k], sizeof(char)*strlen(globBuffer.gl_pathv[k]));

			a.argv[k][strlen(globBuffer.gl_pathv[k])] = '\0';

			printf("match[%d] = %s \n",k, a.argv[k]);

			//a.argv[k][strlen(a.argv[k])-1] = '\0';
		}



		a.argv[matchCount] = NULL;

		a.argc = matchCount;
		c.commandv[i].argc = matchCount;

		printf("%d\n", a.argc);

		memcpy(a.argv, c.commandv[i].argv, sizeof(char*) * (matchCount+1));

		globfree(&globBuffer);
	}

}
*/
void suspendProcesses(pid_t *pidList, int count){
	int j;
	for(j=0; j<count; j++)
		kill(pidList[j], SIGSTOP);
}

int runPipeCommand(Command c){
	int i, j;
  pid_t pid;
  int in, fd [2];
	pid_t *pidList = (pid_t *) malloc(sizeof(pid_t) * c.commandc);

	/* The first process should get its input from the original file descriptor 0.  */
  in = 0;

  /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
  for (i = 0; i < c.commandc; ++i)
    {
      pipe(fd);

			Argument a = c.commandv[i];

			glob_t globBuffer;
			int matchCount=0;

			for(j = 0; j < a.argc; j++){
				char *newArgv = a.argv[j];
				if(matchCount == 0)
					glob(newArgv, GLOB_NOCHECK, NULL , &globBuffer );
				else
					glob(newArgv, GLOB_NOCHECK|GLOB_APPEND, NULL , &globBuffer );
				matchCount = globBuffer.gl_pathc;
			}

			if(i < c.commandc - 1){
	      /* f [1] is the write end of the pipe, we carry `in` from the prev iteration.  */
	      pidList[i] = executeCommand(in, fd[1], globBuffer.gl_pathv);

				/* No need for the write end of the pipe, the child will write here.  */
	      close(fd[1]);

	      /* Keep the read end of the pipe, the next child will read from there.  */
	      in = fd[0];
			}
			else{
				/* Last stage of the pipeline - set stdin be the read end of the previous pipe
			     and output to the original file descriptor 1. */

				 if ((pidList[i] = fork ()) == 0){
			       if (in != 0){
			           dup2 (in, 0);
			       }
						 setSignal(1);
			       execvp (*globBuffer.gl_pathv, globBuffer.gl_pathv);
			   }
				 else{
					 int status;
					 //for(j=0; j<c.commandc; j++)
					 //		printf("pidList[%d]: %d\n", j, pidList[j]);

					 for(j=0; j< 2; j++)
					  	close(fd[j]);

					 for(j=0; j<c.commandc; j++){
	 					  waitpid(pidList[j], &status, WUNTRACED);
							printf("status: %d\n", status);
						 if(WIFSTOPPED(status)){
							  printf("\n");
								printf("%d: WIFSTOPPED - suspended by signal\n", pid);
								suspendProcesses(pidList, c.commandc);
								printf("pidList:\n===========\n")
								for(j=0; j< c.commandc; j++)
									printf("pid: %d\n", pidList[j]);
					      jobsNewNode(pidList, c.command);
					      //kill(childPid,SIGSTOP);
					      //waitpid(child_pid,&status,WUNTRACED);
							}
							else if(WIFSIGNALED(status)){
								printf("\n");
								printf("%d: WIFSIGNALED - terminated by signals\n", pid);
							}
							else if(WIFEXITED(status)){
								printf("%d: WIFEXITED - terminated normally\n", pid);
							}
						}
				 }
			}



			globfree(&globBuffer);
    }

  //if (in != 0)
  //  dup2 (in, 0);

  /* Execute the last stage with the current process. */
  //return execvp (*c.commandv[i].argv, c.commandv[i].argv);

}

int executeCommand(int in, int out, char **argv){
	pid_t pid;

  if ((pid = fork ()) == 0){
		setSignal(1);
    if (in != 0){
      dup2 (in, 0);
      close (in);
    }

    if (out != 1){
      dup2 (out, 1);
      close (out);
    }

    return execvp (*argv, argv);
  }

  return pid;
}
