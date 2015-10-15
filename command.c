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


//fuck github
void trimString(char *string){
	while(isspace(*string)) // trim space in string head
		string++;
	if(string[strlen(string) - 1] == '\n')
		string[strlen(string) - 1] = '\0';
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
printf("tokenizeCommand\n=================\n");
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

	trimString(input);
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

	argv = (char **) malloc(sizeof(char*) * argc);

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

//getchar();

	a->arg = inputBackup;
	a->argc = argc;
	a->argv = argv;

	return *a;
}


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

		for(k = 0; k < matchCount; k++){



			a.argv[k] = (char*)malloc(sizeof(char)*strlen(globBuffer.gl_pathv[k]));
			memcpy(a.argv[k], globBuffer.gl_pathv[k], sizeof(char)*strlen(globBuffer.gl_pathv[k]));

			a.argv[k][strlen(globBuffer.gl_pathv[k])] = '\0';

			printf("match[%d] = %s \n",k, a.argv[k]);

			//a.argv[k][strlen(a.argv[k])-1] = '\0';
		}

		a.argc = matchCount;

		globfree(&globBuffer);
	}

}

void suspendProcesses(pid_t *pidList, int count){
	int j;
	for(j=0; j<count; j++)
		kill(pidList[j], SIGSTOP);
}

void runPipeCommand(Command c){
	int i, j;
	pid_t childPid;
	int fd[2];
	int fd_in = 0;
	pid_t *pidList = (pid_t *) malloc(sizeof(pid_t) * c.commandc);

	pipe(fd);

	for(i=0; i<c.commandc; i++){
		Argument a = c.commandv[i];
		char **temp = malloc(sizeof(char*) * a.argc);
		memcpy(temp, a.argv, a.argc*sizeof(char*));

		pidList[i] = childPid;
		if(!(childPid = fork())){ // child
			setSignal(1);
			setenv("PATH", envPaths, 1);

			dup2(fd_in, 0);
			if(i < c.commandc - 1)
				dup2(fd[1], 1);
			close(fd[0]);

			for(j = 0; j < a.argc; j++){
				printf("%s\n", temp[j]);
			}

			if(execvp(*temp, temp)){
				if(errno == -2){
					printf("%s: No such command or program\n", temp[0]);
				}
				else{
					printf("%s: unknown error\n", temp[0]);
				}
				exit(0);
			}
		}
		else{ // parent
			int status;
			pidList[i] = childPid;

			close(fd[1]);
			fd_in = fd[0];

			waitpid(childPid, &status, WUNTRACED);
			if(WIFSTOPPED(status)){
	            printf("\n");
							suspendProcesses(pidList, i+1);
	            jobsNewNode(pidList, c.command);
	            //kill(childPid,SIGSTOP);
	            //waitpid(child_pid,&status,WUNTRACED);
	        }
		}
	}
}
