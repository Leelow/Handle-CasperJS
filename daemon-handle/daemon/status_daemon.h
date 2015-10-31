#ifndef STATUSDAEMON_H_INCLUDED
#define STATUSDAEMON_H_INCLUDED

/*** Print the status of the daemon, all about actives profiles ***/
char* status_daemon() {

	HandleManager* hdl_mng = accessToHandleManager(shmid_hdl_mng);
	
	char* res = printHandleManagerInfos(hdl_mng);
	
	dissociateHandleManager(hdl_mng);
	return res;
	
}

#endif