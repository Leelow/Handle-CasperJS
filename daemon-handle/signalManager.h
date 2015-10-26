#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "custom_signal.h"

#define MAX_STRING_LENGTH 256
#define MAX_HANDLED_PROGRAM 10
#define MAX_HANDLED_PROGRAM_MESSAGES 10

typedef enum MessageType {
	ERROR_FILE, ERROR_SYNTAX, INFOS, DEBUG, ERROR, END, EMPTY
} MessageType;

typedef struct Message {
	
	// Id of the message's sender
	int sender_pid;
	
	// Timestamp of the message
	int timestamp;
	
	// Type of the message
	MessageType type;
	
	// Plain text of the message
	char text[MAX_STRING_LENGTH];
	
} Message;

typedef struct MessageManager {

	// Size of the message handle
	int size;
	
	// Messages of the handle
	Message messages[MAX_HANDLED_PROGRAM_MESSAGES];

} MessageManager;

typedef struct HandleInfos {
	
	// Name of the handle
	char name[MAX_STRING_LENGTH];
	
	// Timestamp of the creation
	// Timestamp of the last start
	// Timestamp of the last message
	int last_msg;
	
	// Pid of the handle, -1 if it's not started
	//int pid;
	
	// Message manager
	MessageManager msg_mng;
	
} HandleInfos;

typedef struct HandleManager { // to delete
	
	// Number of handle
	int size;
	
	// Array containing asso 
	int ass[MAX_HANDLED_PROGRAM];
	
	// Array containing handles infos
	HandleInfos infos[MAX_HANDLED_PROGRAM];
	
	// Mutex to protect operations
	sem_t mutex;
	
} HandleManager;

void addMessage(int hdl_index, HandleManager* hdl_mng, Message msg);
int getIndexFromPid(int hdl_pid, HandleManager* hdl_mng);
int getPidFromIndex(int hdl_index, HandleManager* hdl_mng);

void initiliazeHandleManager(HandleManager* hdl_mng_out) {
	
	HandleManager hdl_mng;
	
	// Initialize the mutex (prepare it to be in a memory shared segment)
	sem_init(&hdl_mng.mutex, 1, 1);
	
	hdl_mng_out = &hdl_mng;
	
}

void getLastMessageInHandleManager(HandleManager* hdl_mng, Message* msg_output) {

	// If there is no message
	if(hdl_mng->size == 0) {
		
		MessageType t = EMPTY;
		Message msg = {-1, -1, t, "-1"};
		memcpy(msg_output, &msg, sizeof(msg));
		return;

	}

	sem_wait(&(hdl_mng->mutex));
	
	int last_timestamp = -1;
	
	int i;
	for(i = 0; i < hdl_mng->size; i++) {
		
		// Store the message if it's the last
		int curr_last_timestamp = hdl_mng->infos[i].last_msg;
		if(curr_last_timestamp >= last_timestamp) {
			
			int index = hdl_mng->infos[i].msg_mng.size - 1;

			Message msg = hdl_mng->infos[i].msg_mng.messages[index];
			memcpy(msg_output, &msg, sizeof(msg));
			
		}
		
	}

	sem_post(&(hdl_mng->mutex));
	
}

int getMessageNumberInHandleManager(HandleManager* hdl_mng) {
	
	int count = 0;
	
	int i;
	for(i = 0; i < hdl_mng->size; i++) {
		
		count += hdl_mng->infos[i].msg_mng.size;
		
	}
	
	return count;
	
}

// Send a message to the daemon (must be the same thread used by the messages' receiver)
void sendMessageToDaemon(int daemon_pid, Message msg, HandleManager* hdl_mng) {

	sem_trywait(&(hdl_mng->mutex));
	
	// Get the corresponding id
	int hdl_id = getIndexFromPid(getpid(), hdl_mng);
	
	// If the handle isn't registred yet and there is possibility to register a new handle
	if(hdl_id == -1 && hdl_mng->size < MAX_HANDLED_PROGRAM) {
		hdl_id = hdl_mng->size;
		hdl_mng->ass[hdl_id] = getpid();
		hdl_mng->size++;
	}
	
	// Add message in the handle message's list
	addMessage(hdl_id, hdl_mng, msg);
	
	// Send a signal to the daemon indicating there is a new message
	kill(daemon_pid, SIGUSR1);
	
	// Indicate the message has been sent
	sem_post(&(hdl_mng->mutex));
	
}

// Get the corresponding id from a pid using an handleManager. Return -1 if there is no correspondance.
int getIndexFromPid(int hdl_pid, HandleManager* hdl_mng) {
	
	int i;
	for(i = 0; i < hdl_mng->size; i++) {
		if(hdl_mng->ass[i] == hdl_pid)
			return i;
	}
	return -1;
	
}

// Get the corresponding pid from an id using an handleManager. Return -1 if there is no correspondace.
int getPidFromIndex(int hdl_index, HandleManager* hdl_mng) {

	if(hdl_index < hdl_mng->size)
		return hdl_mng->ass[hdl_index];

	return -1;
	
}

// Add message in the handle message's list
void addMessage(int hdl_index, HandleManager* hdl_mng, Message msg) {
	
	// Update last message timestamp
	hdl_mng->infos[hdl_index].last_msg = msg.timestamp;
	
	// Add message
	int size = hdl_mng->infos[hdl_index].msg_mng.size;
	memcpy(&(hdl_mng->infos[hdl_index].msg_mng.messages[size]), &msg, sizeof(msg));
	hdl_mng->infos[hdl_index].msg_mng.size++;

}

HandleManager* accessToHandleManager(int shmid) {
	
	HandleManager* hdl_mng = (HandleManager*) shmat(shmid, NULL, 0);
	if(hdl_mng == NULL) {
		printf("Can't attach the shared memory segment.\n");
		exit(-1);
	}
	return hdl_mng;
	
}

void dissociateHandleManager(HandleManager* hdl_mng) {
	if(shmdt(hdl_mng) == -1) {
		printf("Can't dissociate the shared memory segment.\n") ;
		exit(-1) ;
	}
}

void freeHandleManager(int shmid) {	
	shmctl(shmid, IPC_RMID, NULL);		
}