#ifndef STOP_PROFILE_H_INCLUDED
#define STOP_PROFILE_H_INCLUDED

/*** Stop a profile thanks to its profile id ***/
// profile_id : profile id corresponding to the profile to stop
int stop_profile(char* profile_id) {

	HandleManager* hdl_mng = accessToHandleManager(shmid_hdl_mng);
	int hdl_pid = getHandlePidFromProfileId(profile_id, hdl_mng);
	dissociateHandleManager(hdl_mng);
	
	kill(hdl_pid, SIGINT);
	
	printf("The handle \"%s\" was stopped.\n", profile_id);
	
	exit(-1);
	
}

#endif