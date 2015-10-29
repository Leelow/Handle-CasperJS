#ifndef STARTPROFILE_H_INCLUDED
#define STARTPROFILE_H_INCLUDED

#include <unistd.h>

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
		
		//freopen("newin", "r", stdout);
		
		execl("build/handle", "build/handle", daemon_id_str, shmid_hdl_mng_str, profile_id, NULL);
	
	// Parent process
	} else {
		
		// If we want to follow the handle
		if(follow > -1) 
			follow_profile(profile_id);
		
	}
	
}

#endif