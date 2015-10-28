#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "daemon.h"
#include "signalManager.h"

HandleManager* hdl_mng;
int volatile shmid_hdl_mng;

void create(char* name, char* file);

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
	if (dir_exist(PATH_DAEMON) == -1)
		mkdir(PATH_DAEMON, 0777);
	
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
	
	//************************ ARG ************************//

	if(argc < 2) {
		printf("Wrong arguments.\n");
		exit(-1);
	}
	
	if(strcmp(argv[1], "create") == 0 && argc == 4)
		create(argv[2], argv[3]);
	
	//if(strcmp(argv[1], "start") == 0 && argc > 2 && argc <= 4)
	
	
	// ARG daemon_id shmid_hdl_mng program arg1 output_file
	//if(argc != 6) {
	//	printf("Usage: %s daemon_pid shmid_hdl_mng program arg1 output_file\n", argv[0]);
	//	exit(-1);
	//}
	
	
	//for(;;);
	
}

// Create a new profile with a name and a file. Return the id of the new profile
void create(char* name, char* file) {
	
	// TODO : Protect name with "/"
	
	// TODO : Check if the name is already taken or not ???
	
	// Check if the given file's path is correct
	if(file_exist(file) == -1) {
		printf("The file \"%s\" dosen't exist.\n", file);
		exit(-1);
	}
	
	// Generate an unique id thanks to the timestamp
	char timestamp[11];
	sprintf(timestamp, "%d", (int) time(NULL));
	
	char* id = concat(name, "_");
	strcat(id, timestamp);

	// Create the dir corresponding to the new profile
	char* profile_path = concat(PATH_PROFILES, id);
	strcat(profile_path, "/");
	if (dir_exist(profile_path) == -1)
		mkdir(profile_path, 0777);
	
	// Get the absolute path for the copied file
	char* file_path = concat(profile_path, file);
	
	// Copy the file to the profile path
	copy_file(file_path, file);
	
	printf("\"%s\"\n", profile_path);
	printf("\"%s\"\n", file_path);
	
}