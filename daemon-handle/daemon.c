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

 
int main(int argc, char *argv[]) {
	
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
		
		/*** start profile_id (--follow) ***/
		if(strcmp(argv[1], "stop") == 0 && argc == 3)				
			stop_profile(argv[2]);			
		
		/*** follow profile_id ***/
		if(strcmp(argv[1], "follow") == 0 && argc == 3) {
			
			int res = follow_profile(argv[2], 1);
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
		
		/*** status ***/
		if(strcmp(argv[1], "status") == 0 && argc == 2) {
			
			char* res = status_daemon();
			printf("\"%s\"\n", res);
			exit(-1);
		}
		
	}
	
}