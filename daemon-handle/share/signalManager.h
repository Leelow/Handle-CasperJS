#ifndef SIGNALMANAGER_H_INCLUDED
#define SIGNALMANAGER_H_INCLUDED

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

#include "customSignal.h"

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

typedef struct HandleManager {
	
	// Number of handle
	int size;
	
	// Id of the handle
	char id[MAX_STRING_LENGTH];
	
	// Timestamp of the last message
	int last_msg;
	
	// Array containing handle pid
	int pid[MAX_HANDLED_PROGRAM];
	
	// Message manager
	MessageManager msg_mng[MAX_HANDLED_PROGRAM];
	
	// Mutex to protect operations
	sem_t mutex;
	
} HandleManager;

/*** Initialize handle manager's components ***/
// hdl_mng_out : handle manager
void initiliazeHandleManager(HandleManager* hdl_mng_out);

/*** Get the last message in the handle manager ***/
// hdl_mng    : handle manager
// msg_output : output message
void getLastMessageInHandleManager(HandleManager* hdl_mng, Message* msg_output);

/*** Send a message to the daemon ***/
// daemon_pid : daemon's pid
// msg        : message to send to the daemon
// hdl_mng    : handle manager
void sendMessageToDaemon(int daemon_pid, Message msg, HandleManager* hdl_mng);

/*** Add a message to the handle manager for an handle (internal function) ***/
// hdl_index : handle's index in the handle manager
// hdl_mng   : handle manager
// msg       : message to add
void addMessage(int hdl_index, HandleManager* hdl_mng, Message msg);

/*** Get the number of message in the handle manager ***/
// hdl_mng : handle manager
int getMessageNumberInHandleManager(HandleManager* hdl_mng);

/*** Get the handle's index in the handle manager thanks to its pid (internal function) ***/
// hdl_pid : handle's pid in the handle manager
// hdl_mng   : handle manager
// Note : return -1 if there is no correspondance.
int getIndexFromPid(int hdl_pid, HandleManager* hdl_mng);

/*** Get the handle's pid in the handle manager thanks to its index (internal function) ***/
// hdl_index : handle's index in the handle manager
// hdl_mng   : handle manager
// Note : return -1 if there is no correspondance.
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
		int curr_last_timestamp = hdl_mng[i].last_msg;
		if(curr_last_timestamp >= last_timestamp) {
			
			int index = hdl_mng->msg_mng[i].size - 1;

			Message msg = hdl_mng->msg_mng[i].messages[index];
			memcpy(msg_output, &msg, sizeof(msg));
			
		}
		
	}

	sem_post(&(hdl_mng->mutex));
	
}

void sendMessageToDaemon(int daemon_pid, Message msg, HandleManager* hdl_mng) {

	sem_trywait(&(hdl_mng->mutex));
	
	// Get the corresponding id
	int hdl_id = getIndexFromPid(getpid(), hdl_mng);
	
	// If the handle isn't registred yet and there is possibility to register a new handle
	if(hdl_id == -1 && hdl_mng->size < MAX_HANDLED_PROGRAM) {
		hdl_id = hdl_mng->size;
		hdl_mng->pid[hdl_id] = getpid();
		hdl_mng->size++;
	}
	
	// Add message in the handle message's list
	addMessage(hdl_id, hdl_mng, msg);
	
	// Send a signal to the daemon indicating there is a new message
	kill(daemon_pid, SIGUSR1);
	
	// Indicate the message has been sent
	sem_post(&(hdl_mng->mutex));
	
}

void addMessage(int hdl_index, HandleManager* hdl_mng, Message msg) {
	
	// Update last message timestamp
	hdl_mng[hdl_index].last_msg = msg.timestamp;
	
	// Add message
	int size = hdl_mng->msg_mng[hdl_index].size;
	memcpy(&(hdl_mng->msg_mng[hdl_index].messages[size]), &msg, sizeof(msg));
	hdl_mng->msg_mng[hdl_index].size++;

}

int getMessageNumberInHandleManager(HandleManager* hdl_mng) {
	
	int count = 0;
	
	int i;
	for(i = 0; i < hdl_mng->size; i++) {
		
		count += hdl_mng->msg_mng[i].size;
		
	}
	
	return count;
	
}

int getIndexFromPid(int hdl_pid, HandleManager* hdl_mng) {
	
	int i;
	for(i = 0; i < hdl_mng->size; i++) {
		if(hdl_mng->pid[i] == hdl_pid)
			return i;
	}
	return -1;
	
}

int getPidFromIndex(int hdl_index, HandleManager* hdl_mng) {

	if(hdl_index < hdl_mng->size)
		return hdl_mng->pid[hdl_index];

	return -1;
	
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

#endif