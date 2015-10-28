#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include "daemon.h"

// TODO :
//
// + Clear output error

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

	// TEST
	// SettingsDaemon s = {42, "ee"};
	// saveSettingsDaemonToFile("test.json", &s);
	// SettingsDaemon s2;

	// loadSettingsDaemonFromFile("test.json", &s2);
	// //char* str = settingsToJson(&s);
	// //const Settings s2 = jsonToSettings(str);
	// printf("\"%i\" - \"%s\"\n", s2.id_daemon, s2.path_daemon);
	// exit(-1);
	// FIN TEST

	// Create necessary dir and files at the first startup if necessary
	if (dir_exist(PATH_DAEMON) == -1)
		mkdir(PATH_DAEMON, 0777);
	
	if (dir_exist(PATH_PROFILES) == -1)
		mkdir(PATH_PROFILES, 0777);

	//************************ ARG ************************//

	if(argc < 2) {
		// printf("Wrong arguments.\n");
		// exit(-1);
	}
	
	/*** create name file (phantomjs|slimejs) ***/
	if(strcmp(argv[1], "create") == 0 && (argc == 4 || (argc == 5 && (strcmp(argv[4], "phantomjs") == 0 || strcmp(argv[4], "slimerjs") == 0)))) {
		
		char* web_browser_engine = "phantomjs";
		if(argc == 5)
			web_browser_engine = argv[4];
			
		
		char* id = create_profile(argv[2], argv[3], web_browser_engine);
		printf("\"%s\"\n", id);
		exit(-1);
	}
	
	/*** start profile_id ***/
	if(strcmp(argv[1], "start") == 0 && argc == 3) {
		
		int res = start_profile(argv[2]);
		printf("\"%i\"\n", res);
		exit(-1);
	}
	
	/*** delete profile_id ***/
	if(strcmp(argv[1], "delete") == 0 && argc == 3) {
		
		int res = delete_profile(argv[2]);
		printf("\"%i\"\n", res);
		exit(-1);
	}
	
	/*** infos profile_id ***/
	if(strcmp(argv[1], "infos") == 0 && argc == 3) {
		
		char* res = infos_profile(argv[2]);
		printf("\"%s\"\n", res);
		exit(-1);
	}
	
	//exit(-1);

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
	
	//if(strcmp(argv[1], "start") == 0 && argc > 2 && argc <= 4)
	
	
	// ARG daemon_id shmid_hdl_mng program arg1 output_file
	//if(argc != 6) {
	//	printf("Usage: %s daemon_pid shmid_hdl_mng program arg1 output_file\n", argv[0]);
	//	exit(-1);
	//}
	
	
	for(;;);
	
}