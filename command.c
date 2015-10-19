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
		subCommands[count] = (char *) malloc(sizeof(char) * (strlen(token) + 1));
		for(i=0; i<(strlen(token) + 1); i++)
			subCommands[count][i] = '\0';
		memcpy(subCommands[count++], token, strlen(token) + 1);
		token = strtok(NULL, "|");
	}
	for(i=0; i<commandc; i++)
		commandv[i] = tokenizeArguments(subCommands[i]);

	command->command = inputBackup;
	command->commandc = commandc;
	command->commandv = commandv;

	return *command;
}

Argument tokenizeArguments(char *input){
	int i;
	int argc = 0;
	char **argv;
	Argument *a = (Argument *) malloc(sizeof(Argument));

	input = trimString(input);

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

	argv = (char **) malloc(sizeof(char*) * (argc + 1));

	char *inputBackup = (char *) malloc(sizeof(char) * INPUT_BUFFER_SIZE);
	for(i=0; i<INPUT_BUFFER_SIZE; i++)
		inputBackup[i] = '\0';
	memcpy(inputBackup, input, strlen(input) * sizeof(char));

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

	a->arg = inputBackup;
	a->argc = argc;
	a->argv = argv;

	return *a;
}

void suspendProcesses(pid_t *pidList, int count){
	int j;
	for(j=0; j<count; j++)
		kill(pidList[j], SIGSTOP);
}

void handleErrno(int err, char *cmd){
	//printf("errno: %d\n", err);
	if(err == ENOENT){
		fprintf(stderr, "%s: No such command or program\n", cmd);
	}
	else{
		fprintf(stderr, "%s: unknown error\n", cmd);
	}
	exit(0);
}

int runPipeCommand(Command c){
	int i, j;
  pid_t pid;
  int in, fd [2];
	pid_t *pidList = (pid_t *) malloc(sizeof(pid_t) * c.commandc);

	setenv("PATH", envPaths, 1);

	/* original file descriptor 0. (not pipe) */
  in = 0;

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
	      pidList[i] = executeCommand(in, fd[1], globBuffer.gl_pathv);

	      close(fd[1]);
	      in = fd[0];
			}
			else{
				 if ((pidList[i] = fork ()) == 0){
			       if (in != 0){
			           dup2 (in, 0);
			       }
						 setSignal(1);
			       execvp (*globBuffer.gl_pathv, globBuffer.gl_pathv);
						 handleErrno(errno, globBuffer.gl_pathv[0]);
			   }
				 else{
					 int status;
					 int isHandledSuspension = 0;

					 for(j=0; j< 2; j++)
					  	close(fd[j]);

					 for(j=0; j<c.commandc; j++){
	 					  waitpid(pidList[j], &status, WUNTRACED);
							//printf("status: %d\n", status);
						 if(WIFSTOPPED(status)){
							 	if(!isHandledSuspension){
							  	printf("\n");
									suspendProcesses(pidList, c.commandc);
						      jobsNewNode(pidList, c.command, c.commandc);
									isHandledSuspension = 1;
								}
								//printf("%d: WIFSTOPPED - suspended by signal\n", pid);

							}
							else if(WIFSIGNALED(status)){
								if(!isHandledSuspension){
									printf("\n");
									isHandledSuspension = 1;
								}
								//printf("%d: WIFSIGNALED - terminated by signals\n", pid);
							}
							else if(WIFEXITED(status)){
								isHandledSuspension = 1;
								//printf("%d: WIFEXITED - terminated normally\n", pid);
							}
						}
				 }
			}



			globfree(&globBuffer);
    }

}

int executeCommand(int in, int out, char **argv){
	pid_t pid;

  if ((pid = fork ()) == 0){
		int value;
		setSignal(1);
    if (in != 0){
      dup2 (in, 0);
      close (in);
    }

    if (out != 1){
      dup2 (out, 1);
      close (out);
    }

    value = execvp (*argv, argv);
		handleErrno(errno, argv[0]);
		return value;
  }

  return pid;
}
