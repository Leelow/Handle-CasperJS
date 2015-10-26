#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <signal.h>

#include "daemon.h"
#include "signalManager.h"

#define PATH_PROFILES "/home/leo/casperjs_profiles/"


HandleManager* hdl_mng;
int volatile shmid_hdl_mng;

void handlerStop(int sig, siginfo_t* info, void* vp) {

	// Say the shared memory segment need to be destroyed
	freeHandleManager(shmid_hdl_mng);	
	
	exit(0);
	
}

void handlerMessage(int sig, siginfo_t* info, void* vp) {

	hdl_mng = accessToHandleManager(shmid_hdl_mng);

	Message msg;
	getLastMessageInHandleManager(hdl_mng, &msg);
	printf("+ Last message :\n   timestamp: %i\n   sender_pid: %i\n   type: %i \n   text: %s\n", msg.timestamp, msg.sender_pid, msg.type, msg.text);

	dissociateHandleManager(hdl_mng);
		
}

int main(int argc, char *argv[]) {

	// Create necessary dir and files at the first startup if necessary
	if (dir_exist(PATH_PROFILES) == -1)
		mkdir(PATH_PROFILES, 0777);

	// Initalize the handle manager
	initiliazeHandleManager(hdl_mng);
	
	// Create the shared memory segment containing the handle manager
	shmid_hdl_mng = shmget(IPC_PRIVATE, sizeof(*hdl_mng), IPC_CREAT | 0777);
	
	// Register stop handler
	if(signalWithInfo(SIGINT, (*handlerStop)) < 0)
		printf("Can't catch SIGINT.\n");
	
	printf("%i %i\n", getpid(), shmid_hdl_mng);
	
	// Register messages receveiver handler
	if(signalWithInfo(SIGUSR1, (*handlerMessage)) < 0)
		printf("Can't catch SIGUSR1.\n");	
	
	for(;;);
	
}