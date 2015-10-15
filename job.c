#include "job.h"

// job list
Jobs *head;
Jobs *job;
int jobCount;

void doFg(Argument a) {
    if(a.argc == 2){
        int jobID, i;
        Jobs *temp;
        sscanf(a.argv[1], "%d", &jobID);
        temp=jobsDelNode(jobID);
        if (temp!=NULL) {
            int status;
            kill(temp->pidList[0], SIGCONT);
            //setSignal(1);
            waitpid(temp->pidList[0],&status,WUNTRACED);
            if (WIFSTOPPED(status)) {
                jobsNewNode(temp->pidList, temp->cmd);
                printf("\n");
            }
        } else {
            printf("fg: no such job\n");
        }
    }
    else{
        printf("fg: wrong number of arguments - %d\n", a.argc);
    }
}

void initJob(){
    head=(Jobs *) malloc(sizeof(Jobs));
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
    job = (Jobs *) malloc(sizeof(Jobs));
    strcpy(job->cmd,cmd);
    job->pidList= pidList;
    job->next = NULL;
    //printf("%d\n", job->pidList);

    if(head == NULL){
        head = job;
        //printf("%d", head->pidList);
    }else if(head != NULL) {
        Jobs* temp;
        for(temp = head;temp->next != NULL;temp=temp->next);
        temp->next = job;
    }
}

Jobs* jobsDelNode(int jobID) {
    int i;
    Jobs* returnJob = NULL;
    if (jobID <= jobCount && jobID > 0) {
        if (jobID == 1) {
            if (jobCount > 1) {
                Jobs* newHead = head->next;
                memcpy(&returnJob, &head, sizeof(head));
                free(head->pidList);
                free(head);
                head = newHead;
            } else {
                // memcpy(returnJob, head, sizeof());
                // free(head->pidList);
                // free(head);
                returnJob = head;
                head = NULL;
            }
        } else {
            Jobs* temp = head;
            for (i=0;temp!=NULL && i!=jobID-1;i++,temp=temp->next);
            if (temp->next!=NULL) { // delete middle note
                Jobs* delPtr = temp->next;
                temp->next = temp->next->next;
                memcpy(&returnJob, &delPtr, sizeof(head));
                free(delPtr->pidList);
                free(delPtr);
            } else { // delete tail
                Jobs* delPtr = temp->next;
                temp->next = NULL;
                memcpy(&returnJob, &delPtr, sizeof(head));
                free(delPtr->pidList);
                free(delPtr);
            }
        }
    }
    return returnJob;
}
