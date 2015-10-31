#ifndef FOLLOWPROFILE_H_INCLUDED
#define FOLLOWPROFILE_H_INCLUDED

int allowed_pid;

/*** Stop handler corresponding to a CTRL-C command, not a following daemon replacement ***/
void handlerStop(int sig, siginfo_t* info, void* vp) {

	// Indicate to the handle that there is no signal to send to the daemon
	//HandleManager* hdl_mng = accessToHandleManager(shmid_hdl_mng);
	//stopSendSignalToFollowingDaemon(allowed_pid, hdl_mng);
	//dissociateHandleManager(hdl_mng);
	printf("Another instance follow now this handle.\n");
	
	exit(-1);
	
}

/*** Handler to receive message. It's only accepts message from the first process which gave it a message. ***/
void handlerMessage(int sig, siginfo_t* info, void* vp) {

	// Remember the first process to give him a message
	if(allowed_pid == -1) {

		allowed_pid = info->si_pid;
	
		//HandleManager* hdl_mng = accessToHandleManager(shmid_hdl_mng);
				
		//dissociateHandleManager(hdl_mng);
		
	}

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
// profile_id : profile id corresponding to the profile to load
// active     : if active = 1, indicates that the handle is active
int follow_profile(char* profile_id, int active) {

	allowed_pid = -1;

	if(active == 1) {
		
		HandleManager* hdl_mng = accessToHandleManager(shmid_hdl_mng);
		allowed_pid = getHandlePidFromProfileId(profile_id, hdl_mng);
		updateFollowingDaemonPid(allowed_pid, profile_id, getpid(), hdl_mng);
		dissociateHandleManager(hdl_mng);

		printf("daemon id : %i\n", getpid());
		printf("allowed pid : %i\n", allowed_pid);
		
	}

	// Register messages receiver handler
	if(signalWithInfo(SIGUSR1, (*handlerMessage)) < 0)
		printf("Can't catch SIGUSR1.\n");
	
	// Intercept SIGUSR2 signal send by the new following daemon to stop this instance of daemon
	if(signalWithInfo(SIGUSR2, handlerStop) < 0)
		printf("Can't catch SIGINT.\n");
	
	for(;;);
	
}

#endif