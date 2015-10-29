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

void handlerStop(int sig, siginfo_t* info, void* vp) {

	// Say the shared memory segment need to be destroyed
	//freeHandleManager(shmid_hdl_mng);	
	
	exit(0);
	
}




int main(int argc, char *argv[]) {

	// TEST

	// int timestamp = (int)time(NULL);
	// MessageType type = START;
	// Message msg = {getpid(), timestamp, type, "\"Start !\""};
	
	// printf("%s\n", messageToJson(&msg));
	// exit(-1);
	
	// FIN TEST
	
	// Create necessary dir and files at the first startup if necessary
	if (dir_exist(PATH_DAEMON) == -1)
		mkdir(PATH_DAEMON, 0777);
	
	if (dir_exist(PATH_PROFILES) == -1)
		mkdir(PATH_PROFILES, 0777);

	// Initalize the handle manager
	initiliazeHandleManager(hdl_mng);

	// Create the shared memory segment containing the handle manager if it doesn't exist
	shmid_hdl_mng = shmget(SHARED_MEMORY_KEY, sizeof(*hdl_mng), IPC_CREAT | 0777);
	
	//************************ ARG ************************//
	
	if(argc > 1) {
	
		/*** create name file (phantomjs|slimejs) ***/
		if(strcmp(argv[1], "create") == 0 && (argc == 4 || (argc == 5 && (strcmp(argv[4], "phantomjs") == 0 || strcmp(argv[4], "slimerjs") == 0)))) {
			
			char* web_browser_engine = "phantomjs";
			if(argc == 5)
				web_browser_engine = argv[4];
				
			
			char* id = create_profile(argv[2], argv[3], web_browser_engine);
			printf("\"%s\"\n", id);
			exit(-1);
		}
		
		/*** start profile_id (--follow) ***/
		if(strcmp(argv[1], "start") == 0 && (argc == 3 || (argc == 4 && strcmp(argv[3], "--follow") == 0))) {
			
			int follow = -1;
			if(argc == 4)
				follow = 1;
			
			int res = start_profile(argv[2], follow);
			//printf("\"%i\"\n", res);
			exit(-1);
		}
		
		/*** follow profile_id ***/
		if(strcmp(argv[1], "follow") == 0 && argc == 3) {
			
			int res = follow_profile(argv[2]);
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
		
	}
	
	// exit(-1);
	// printf("eeeee");
	// exit(-1);
	
	

	
	// Register stop handler
	if(signalWithInfo(SIGINT, (*handlerStop)) < 0)
		printf("Can't catch SIGINT.\n");
	

	

	
	//if(strcmp(argv[1], "start") == 0 && argc > 2 && argc <= 4)
	
	
	// ARG daemon_id shmid_hdl_mng program arg1 output_file
	//if(argc != 6) {
	//	printf("Usage: %s daemon_pid shmid_hdl_mng program arg1 output_file\n", argv[0]);
	//	exit(-1);
	//}
	
	
	// for(;;);
	
}