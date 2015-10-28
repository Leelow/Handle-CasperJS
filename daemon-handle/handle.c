#include "handle.h"

Buffer* buff;
volatile int shmid;
sem_t mutex;
char output_file[MAX_STRING_LENGTH];
pid_t daemon_pid;

int volatile shmid_hdl_mng;

int startsWith(const char *str, const char *pre)
{
    return strncmp(str, pre, strlen(pre));
}

void signalManager(char line[MAX_STRING_LENGTH]) {

	HandleManager* hdl_mng = accessToHandleManager(shmid_hdl_mng);

	if(startsWith(line, "Unable to open file") == 0) {
		printf("[SIGNAL SENDER] Script file can't be finded.\n");
		
	} else if(startsWith(line, "SyntaxError") == 0) {
		printf("[SIGNAL SENDER] Syntax error.\n");

	}  else if(startsWith(line, "[INFOS]") == 0) {
		printf("[INFOS] Debug.\n");
		
	}  else if(startsWith(line, "[DEBUG]") == 0) {
		printf("[SIGNAL SENDER] Debug.\n");
		
	}  else if(startsWith(line, "[ERROR]") == 0) {
		printf("[SIGNAL SENDER] Error.\n");
		
	}   else if(strcmp(line, "END") == 0) {
		printf("[SIGNAL SENDER] Program end.\n");
		
		int timestamp = (int)time(NULL);
		MessageType type = END;
		Message msg = {getpid(), timestamp, type, "\"The handle has finished.\""};
		sendMessageToDaemon(daemon_pid, msg, hdl_mng);

	}
	
	dissociateHandleManager(hdl_mng);
	
}

void handlerStop(int sig, siginfo_t* info, void* vp) {

	//printf("int code: %i\nsender pid: %i\n", info->si_signo, info->si_pid);

	// Save the lines from the buffer
	saveBuffer(output_file, shmid, &mutex);

	// Say the shared memory segment need to be destroyed
	freeBuffer(shmid);	
	
}

int main(int argc, char * argv[]) {

	// <--- NON GENERIC CODE ---> //

	// Check arg
	// ARG --version
	if(argc > 1 && strcmp("--version", argv[1]) == 0) {
		printf("%s\n", VERSION);
		exit(-1);
	}

	// ARG daemon_id shmid_hdl_mng program arg1 output_file ///////////////
	// ARG daemon_id shmid_hdl_mng profile_id
	if(argc != 4) { //if(argc != 6) {
		printf("Usage: %s daemon_pid shmid_hdl_mng profile_id\n", argv[0]); //printf("Usage: %s daemon_pid shmid_hdl_mng program arg1 output_file\n", argv[0]);
		exit(-1);
	}
	
	// TODO : check if the profile id corresponds to a real profile id
	
	// Get the id of the daemon process to send signal
	daemon_pid = atoi(argv[1]);
	
	// Get the if of the shared_segment which is used to communicate with the daemon
	shmid_hdl_mng = atoi(argv[2]);
	
	// Get the settings of the handle's profile to obtain the web webrowser engine
	SettingsHandle s;
	loadSettingsHandleFromProfileId(argv[3], &s);
	
	char* script_path = concat(PATH_PROFILES, argv[3]);
	script_path = concat(script_path, "/");
	script_path = concat(script_path, PROFILE_SCRIPT_FILE);


	// // Generate command
	char command[MAX_STRING_LENGTH];
	strcpy(command, "casperjs --engine=");
	strcat(command, s.web_browser_engine);
	strcat(command, " \"");
	strcat(command, script_path);
	strcat(command, "\"");

	// <--- NON GENERIC CODE ---> //
	
	// init the mutex
	sem_init(&mutex, 0, 1);
	
	// Create shared memory segment
	shmid = shmget(IPC_PRIVATE, sizeof(*buff), IPC_CREAT | 0777);
	if(shmid == -1) {
		printf("Can't create the shared memory segment.\n");
		exit(-1);
	}
	
	// Open the pipe on the program
	FILE *fpin;
    fpin = popen(command, "r");
		
	// Fork the program
	pid_t pidChild = fork();
	if(pidChild == -1) {
		printf("Can't fork.\n");
		exit(-1);
	}
 
	// Thread to periodically save lines
	if(pidChild == 0) {
	
		// Register stop handler once
		if(signalWithInfo(SIGINT, (*handlerStop)) < 0)
			printf("Can't catch SIGINT.\n");
	
		while(1) {

			sleep(MAX_DELAY_BETWEEN_LOGS);

			// Save the lines from the buffer
			saveBuffer(output_file, shmid, &mutex);
		
		}
	
	}
	// Thread to save automatically when a number of bufferized lines is reached
	else {
	
		// Log line
		char line_output[MAX_STRING_LENGTH];
	
		// For each line
		while(fgets(line_output, MAX_STRING_LENGTH, fpin) != NULL) {
			
			signalManager(line_output);
			
			char line[MAX_STRING_LENGTH];
			strcpy(line, getSystemDate());
			strcat(line, " ");
			strcat(line, line_output);

			addLineToBuffer(shmid, &mutex, line);
			
			printf("%s", line);
			
			// Save into the log file if there is enough line to save
			if(getNumberOfLineInBuffer(shmid) >= MAX_LINE_BUFFER)			
				saveBuffer(output_file, shmid, &mutex);
			
		}
		
		// Indicates to the signalManager that the program has finished
		signalManager("END");
		
		// Kill the other thread
		kill(pidChild, SIGKILL);
		
		// Save the lines from the buffer
		saveBuffer(output_file, shmid, &mutex);

		// Say the shared memory segment need to be destroyed
		freeBuffer(shmid);
		
	}

}