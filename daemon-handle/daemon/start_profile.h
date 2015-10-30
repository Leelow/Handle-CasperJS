#ifndef STARTPROFILE_H_INCLUDED
#define STARTPROFILE_H_INCLUDED

#include <unistd.h>

void handlerStopMessage(int sig, siginfo_t* info, void* vp);

/*** Start a profile thanks to its profile id ***/
// profile_id    : profile id corresponding to the profile to load
// follow        : if follow != -1, wait for handle messages
int start_profile(char* profile_id, int follow) {
	
	int daemon_id = getpid();
	
	// Start the handle
	pid_t pidChild = fork();
	if(pidChild == -1) {
		printf("Can't fork.\n");
		exit(-1);
	}
	
	// Child process
	if(pidChild == 0) {

		char shmid_hdl_mng_str[20];
		sprintf(shmid_hdl_mng_str, "%d", shmid_hdl_mng);
		
		char daemon_id_str[20];
		sprintf(daemon_id_str, "%d", daemon_id);	
		
		// Detach this process
		setsid();
		
		// Redirect stdout
		freopen("newin", "r", stdout);
		
		// Execute the handle
		execl("build/handle", "build/handle", daemon_id_str, shmid_hdl_mng_str, profile_id, NULL);
	
	// Parent process
	} else {
		
		// If we want to follow the handle
		if(follow > -1) 
			follow_profile(profile_id, 0);
		else {
			
			// Register messages receiver handler. The handler, receiving the START message, indicates he dosen't want to follow the handle
			if(signalWithInfo(SIGUSR1, (*handlerStopMessage)) < 0)
				printf("Can't catch SIGUSR1.\n");
			
			for(;;);
			
		}		
		
	}
	
}

/*** Stop handler corresponding to a CTRL-C command, not a folloqing daemon replacement ***/
void handlerStopMessage(int sig, siginfo_t* info, void* vp) {

	// Get handle's pid
	int hdl_pid = info->si_pid;

	// Indicate to the handle that there is no daemon to send signal
	HandleManager* hdl_mng = accessToHandleManager(shmid_hdl_mng);
	stopSendSignalToFollowingDaemon(hdl_pid, hdl_mng);
	dissociateHandleManager(hdl_mng);

	exit(-1);
	
}

#endif