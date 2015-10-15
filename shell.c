#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h> // signal()
#include <ctype.h> // isspace() asdsad
#include <limits.h> // PATH_MAX
#include <unistd.h> // getcwd()
#include <glob.h>

#include "command.h"
#include "buildinCommands.h"
#include "job.h"

char *getCurrentDir(){
	char *cwd = (char *) malloc(sizeof(char) * (PATH_MAX + 1));
	return getcwd(cwd, PATH_MAX + 1);
}

int main(int argc, char *argv[]){
	int isRunBuildinCommand;

	initJob();

	while(1){
		setSignal(0);
		char* inputBuffer = (char *) malloc(sizeof(char) * INPUT_BUFFER_SIZE);
		printf("[3150 shell:%s]$ ", getCurrentDir());
		fgets(inputBuffer, INPUT_BUFFER_SIZE, stdin);
		inputBuffer = trimString(inputBuffer);
		printf("result string: <%s>\n", inputBuffer);

		Command c = tokenizeCommand(inputBuffer);
		printCommand(c);

		//printf("isBuildinCommand = %d\n", isBuildinCommand(c.commandv[0]));

		if(c.commandc == 1 && isBuildinCommand(c.commandv[0]))
			runBuildinCommands(c.commandv[0]);
		else{
			wildcast(c);
			runPipeCommand(c);
		}

		//free(c.command);
		//free(c.commandv);

		free(inputBuffer);

		//--> No pipe commaand
		//Argument a = tokenizeArguments(inputBuffer);
		//printArgument(a);

		//isRunBuildinCommand = runBuildinCommands(a);
		//if(!isRunBuildinCommand)
		//	runSystemCommand(a, inputBuffer);

	}


	return 0;
}
