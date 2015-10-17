#include "job.h"

// job list sssss
Jobs *head;
int jobCount;

void initJob(){
    //head=(Jobs *) malloc(sizeof(Jobs));
    head = NULL;
    jobCount = 0;
}

void printJobList(){
    int count=0;
    Jobs *temp;

    temp = head;
    if (temp==NULL) {
        printf("No suspended jobs\n");
    }
    while(temp!=NULL){
        printf("[%d] %s\n", ++count, temp->cmd);
        temp = temp->next;
    }
}


void jobsNewNode(pid_t *pidList, char *command){

    char *cmd = command;

    jobCount++;
    Jobs *newJob = malloc(sizeof(Jobs));
    strcpy(newJob->cmd,cmd);
    newJob->pidList= pidList;
    newJob->next = NULL;
    //printf("%d\n", job->pidList);

    if(head == NULL){
        head = newJob;
        //printf("%d", head->pidList);
    }else if(head != NULL) {
        Jobs* temp;
        for(temp = head;temp->next != NULL;temp=temp->next);
        temp->next = newJob;
    }
}

void jobsDelNode(int jobID) {
    int i;
    if (jobID == 1) {
        if (jobCount > 1) {
            Jobs* temp = head;
            head = head->next;
            //free(head->pidList);
            //free(temp);
        } else {
            //initJob();
            Jobs* temp = head;
            head = NULL;
            //free(head->pidList);
            //free(temp);
        }
    } else {
        Jobs* temp = head;
        Jobs* delPtr = NULL;
        for (i=1;temp!=NULL && i!=jobID-1;i++,temp=temp->next);
        if (jobID < jobCount) { // delete middle note
            delPtr = temp->next;
            temp->next = temp->next->next;
            // free(delPtr->pidList);
            // free(delPtr);
        } else { // delete tail
            delPtr = temp->next;
            temp->next = NULL;
            // free(delPtr->pidList);
            // free(delPtr);
        }
        // free(delPtr->pidList);
        // free(delPtr);
    }
}

int findJob(int jobID) {
    if (jobID <= jobCount && jobID > 0) {
        return 1; // job exist
    }
    return 0;
}

void doFg(Argument a, int FGorBG) {
    // FGorBG
    // FG = 0 , BG = 1
    if(a.argc == 2){
        int jobID, i;
        Jobs *temp = head;
        sscanf(a.argv[1], "%d", &jobID);
        if (findJob(jobID) == 1) {
            for (i=0;i!=jobID-1;i++,temp=temp->next); //point to target job
            int status;
            printf("Job wake up:  %s\n", temp->cmd);
            //printf("----%d-----\n", temp->pidList[0]);
            int pidListSize = sizeof(temp->pidList)/sizeof(temp->pidList[0]); //get pidlist size
            printf("\npid list size : %d\n", pidListSize);
            if (pidListSize > 1) { // with pipe
                //wake all process
                for (i=0;i<pidListSize;i++) {
                    kill(temp->pidList[i], SIGCONT);
                }
                //wait all process
                for (i=0;i<pidListSize;i++) {
                    waitpid(temp->pidList[i],&status,WUNTRACED);
                    // if (!WIFSTOPPED(status)) {
                    //     printf("\nDelele JOB!!\n");
                    //     jobsDelNode(jobID);
                    //     jobCount--;
                    // }
                }
            } else { // without pipe
                kill(temp->pidList[0], SIGCONT);
                //setSignal(1);
                waitpid(temp->pidList[0],&status,WUNTRACED);
                if (!WIFSTOPPED(status)) {
                    printf("\nDelele JOB!!\n");
                    jobsDelNode(jobID);
                    jobCount--;
                }
            }
        } else {
            printf("fg: no such job\n");
        }
    }
    else{
        printf("fg: wrong number of arguments\n");
    }
}

int returnJobCount() {
    return jobCount;
}
