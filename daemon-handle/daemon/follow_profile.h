#ifndef FOLLOWPROFILE_H_INCLUDED
#define FOLLOWPROFILE_H_INCLUDED

int allowed_pid;

/*** Stop handler ***/
void handlerStop(int sig, siginfo_t* info, void* vp) {

	// Alert the handle
	if(allowed_pid > 0)
		kill(allowed_pid, SIGINT);

	exit(-1);
	
}

/*** Handler to receive message. It's only accepts message from the first process which gave it a message. ***/
void handlerMessage(int sig, siginfo_t* info, void* vp) {

	// Remember the first process to give him a message
	if(allowed_pid = -1)
		allowed_pid = info->si_pid;

	HandleManager* hdl_mng = accessToHandleManager(shmid_hdl_mng);
	
	// Check if the message comes from the good handle
	if(info->si_pid == allowed_pid) {
	
		Message msg;
		getLastMessageInHandleManager(hdl_mng, allowed_pid, &msg);
		//printf("+ Last message :\n   timestamp: %i\n   sender_pid: %i\n   type: %i \n   text: %s\n", msg.timestamp, msg.sender_pid, msg.type, msg.text);
		printf("%s\n", messageToJson(&msg));
		// before : dissociateHandleManager(hdl_mng);
	
	}
	
	dissociateHandleManager(hdl_mng); //BEFORE
		
}

/*** Follow a profile thanks to its profile id ***/
// profile_id    : profile id corresponding to the profile to load
int follow_profile(char* profile_id) {

	allowed_pid = -1;

	// Register messages receiver handler
	if(signalWithInfo(SIGUSR1, (*handlerMessage)) < 0)
		printf("Can't catch SIGUSR1.\n");

	// Register stop handler
	if(signalWithInfo(SIGINT, (*handlerStop)) < 0)
		printf("Can't catch SIGINT.\n");
	
	for(;;);
	
}



#endif