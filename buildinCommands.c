#include "buildinCommands.h"

char *buildinCommandList[INPUT_BUFFER_SIZE] =
	{	//ID: Row number - 5
		"cd",
		"exit",
		"jobs",
		"fg",
		"bg",
		NULL
	};

int isBuildinCommand(Argument a){
	int i = 0;
	while(buildinCommandList[i] != NULL){
		if(strcmp(a.argv[0], buildinCommandList[i++]) == 0)
			return 1;
	}
	return 0;
}

int runBuildinCommands(Argument a){
	int i;

	if(strcmp(a.argv[0], buildinCommandList[0]) == 0){
		doCd(a);
		return 1;
	}

	else if(strcmp(a.argv[0], buildinCommandList[1]) == 0){
		doExit(a);
		return 1;
	}

	else if(strcmp(a.argv[0], buildinCommandList[2]) == 0){
		printJobList();
		return 1;
	}

	else if(strcmp(a.argv[0], buildinCommandList[3]) == 0){
		resumeJob(a, 0);
		return 1;
	}

	else if(strcmp(a.argv[0], buildinCommandList[4]) == 0){
		resumeJob(a, 1);
		return 1;
	}

	return 0;
}

void doCd(Argument a){
	if(a.argc == 2){
		if(chdir(a.argv[1]) == -1)
			printf("Cannot change directory\n");
	}
	else{
		printf("cd: wrong number of arguments\n");
	}
}

void doExit(Argument a){
	if(a.argc == 1){
		if (returnJobCount() > 0) {
			printf("There is at least one suspended job\n");
		} else {
			exit(0);
		}
	}
	else
		printf("exit: wrong number of arguments\n");
}
