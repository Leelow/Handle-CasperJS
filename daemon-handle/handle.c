#include "handle.h"

Buffer* buff;
volatile int shmid;
sem_t mutex;
char output_file[MAX_STRING_LENGTH];
pid_t daemon_pid;
char profile_id[MAX_STRING_LENGTH];

int volatile shmid_hdl_mng;

int startsWith(const char *str, const char *pre)
{
    return strncmp(str, pre, strlen(pre));
}

void createMessage(Message* msg, char* text, MessageType type) {
	
	int timestamp = (int) time(NULL);
	Message msg_to_cpy = {getpid(), timestamp, type, ""};
	strcpy(msg_to_cpy.text, text);
	memcpy(msg, &msg_to_cpy, sizeof(msg_to_cpy));
	
}

void signalManager(char line[MAX_STRING_LENGTH]) {

	HandleManager* hdl_mng = accessToHandleManager(shmid_hdl_mng);

	// Get the following daemon pid to know if we have to send a message
	int following_daemon_pid = getFollowingDaemonPid(getpid(), hdl_mng);

	if(following_daemon_pid > 0) {
		
		Message msg = {0};
	
		if(startsWith(line, "Unable to open file") == 0) {
			printf("[SIGNAL SENDER] Script file can't be finded.\n");
			
		} else if(startsWith(line, "SyntaxError") == 0) {
			printf("[SIGNAL SENDER] Syntax error.\n");

		} else if(startsWith(line, "[INFOS]") == 0) {
			printf("[INFOS] Debug.\n");
			
		} else if(startsWith(line, "[DEBUG]") == 0) {
			printf("[SIGNAL SENDER] Debug.\n");
			
			createMessage(&msg, "Debug message ! ", DEBUG);
			
		} else if(startsWith(line, "[ERROR]") == 0) {
			printf("[SIGNAL SENDER] Error.\n");
		
		} else if(strcmp(line, "START") == 0) {
			printf("[SIGNAL SENDER] Program starts.\n");

			createMessage(&msg, "Start message ! ", START);
			
		
		} else if(strcmp(line, "END") == 0) {
			printf("[SIGNAL SENDER] Program ends.\n");
			
			createMessage(&msg, "End message ! ", END);

		}
		
		// If there is a message to send
		if(msg.sender_pid > 0)
			sendMessageToDaemon(following_daemon_pid, msg, profile_id, hdl_mng);

	}
	
	dissociateHandleManager(hdl_mng);
	
}

void handlerStop(int sig, siginfo_t* info, void* vp) {

	//printf("int code: %i\nsender pid: %i\n", info->si_signo, info->si_pid);
	
	HandleManager* hdl_mng = accessToHandleManager(shmid_hdl_mng);
		
	// Signal in the handle manager that the handle is unactive
	handleIsStopped(getpid(), hdl_mng);
	
	// Signal to the following daemon to stop
	int following_daemon_pid = getFollowingDaemonPid(getpid(), hdl_mng);
	if(following_daemon_pid > 0)
		kill(following_daemon_pid, SIGINT);
	
	dissociateHandleManager(hdl_mng);
	
	// Save the lines from the buffer
	saveBuffer(output_file, shmid, &mutex);

	// Say the shared memory segment need to be destroyed
	freeBuffer(shmid);	
	
}

// void handlerGetDaemonPid(int sig, siginfo_t* info, void* vp) {

	// daemon_pid = info->si_pid;
	
// }

int main(int argc, char * argv[]) {

	// printf("\n handle pid : %i\n", getpid());
	// printf("arg : \n");
	// printf("c : %i\n", argc);
	// printf("0 : %s\n", argv[0]);
	// printf("1 : %s\n", argv[1]);
	// printf("2 : %s\n", argv[2]);
	// printf("3 : %s\n\n", argv[3]);

	// <--- NON GENERIC CODE ---> //

	// Check arg
	// ARG --version
	if(argc > 1 && strcmp("--version", argv[1]) == 0) {
		printf("%s\n", VERSION);
		exit(-1);
	}

	// ARG daemon_pid shmid_hdl_mng profile_id
	if(argc != 4) {
		printf("Usage: %s daemon_pid shmid_hdl_mng profile_id\n", argv[0]);
		exit(-1);
	}

	// Register daemon pid handler's once
	// if(signalWithInfo(SIGUSR2, (*handlerStop)) < 0)
		// printf("Can't catch SIGUSR2.\n");
	
	// TODO : check if the profile id corresponds to a real profile id
	
	// Get the id of the daemon process to send signal
	daemon_pid = atoi(argv[1]);

	// Get the if of the shared_segment which is used to communicate with the daemon
	shmid_hdl_mng = atoi(argv[2]); // argv[2]

	// Get the handle's profile id
	strcpy(profile_id, argv[3]);

	// Update the daemon pid in the handle manager
	HandleManager* hdl_mng = accessToHandleManager(shmid_hdl_mng);
	updateFollowingDaemonPid(getpid(), profile_id, daemon_pid, hdl_mng);
	dissociateHandleManager(hdl_mng);
	
	// Get the settings of the handle's profile to obtain the web webrowser engine
	SettingsHandle s;
	loadSettingsHandleFromProfileId(profile_id, &s);
	
	char* script_path = concat(PATH_PROFILES, profile_id);
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
	
		while(1) {

			sleep(MAX_DELAY_BETWEEN_LOGS);

			// Save the lines from the buffer
			saveBuffer(output_file, shmid, &mutex);
		
		}
	
	}
	// Thread to save automatically when a number of bufferized lines is reached
	else {
	
		// Register stop handler once (must be in the parent process /!\)
		if(signalWithInfo(SIGINT, (*handlerStop)) < 0)
			printf("Can't catch SIGINT.\n");
	
		// Indicates to the signalManager that the program starts
		signalManager("START");
	
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
			
			//printf("%s", line);
			
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