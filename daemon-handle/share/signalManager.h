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
#include "message.h"

// #define MAX_STRING_LENGTH 256
#define MAX_HANDLED_PROGRAM 10
#define MAX_HANDLED_PROGRAM_MESSAGES 10

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
	
	// Array containing handles state (active = 1, unactive = -1);
	int state[MAX_HANDLED_PROGRAM];
	
	// Array containing daemon's pid
	int daemon_pid[MAX_HANDLED_PROGRAM];
	
	// Array containing handles pid
	int pid[MAX_HANDLED_PROGRAM];
	
	// Array containing handles profile id
	char profile_id[MAX_HANDLED_PROGRAM][MAX_STRING_LENGTH];
	
	// Message manager
	MessageManager msg_mng[MAX_HANDLED_PROGRAM];
	
	// Mutex to protect operations
	sem_t mutex[MAX_HANDLED_PROGRAM];
	
} HandleManager;

/*** Initialize handle manager's components ***/
// hdl_mng_out : handle manager
void initiliazeHandleManager(HandleManager* hdl_mng_out);

/*** Get the last message in the handle manager ***/
// hdl_mng    : handle manager
// handle_pid : handle's pid
// msg_output : output message
void getLastMessageInHandleManager(HandleManager* hdl_mng, int handle_pid, Message* msg_output);

/*** Send a message to the daemon ***/
// daemon_pid : daemon's pid
// msg        : message to send to the daemon
// profile_id : handle's profile id
// hdl_mng    : handle manager
void sendMessageToDaemon(int daemon_pid, Message msg, char* profile_id, HandleManager* hdl_mng);

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
// hdl_mng : handle manager
// Note    : return -1 if there is no correspondance.
int getIndexFromPid(int hdl_pid, HandleManager* hdl_mng);

/*** Get the handle's index in the handle manager thanks to its profile_id (internal function) ***/
// hdl_profile_id : handle's profile id in the handle manager
// hdl_mng        : handle manager
// Note           : return -1 if there is no correspondance.
int getIndexFromProfileId(char* hdl_profile_id, HandleManager* hdl_mng);

/*** Get the handle's pid in the handle manager thanks to its index (internal function) ***/
// hdl_index : handle's index in the handle manager
// hdl_mng   : handle manager
// Note      : return -1 if there is no correspondance.
int getPidFromIndex(int hdl_index, HandleManager* hdl_mng);


char* printHandleManagerInfos(HandleManager* hdl_mng);

void initiliazeHandleManager(HandleManager* hdl_mng_out) {
	
	HandleManager hdl_mng;
	
	hdl_mng.size = 0;
	
	// Initialize the mutex (prepare it to be in a memory shared segment)
	//sem_init(&hdl_mng.mutex, 1, 1);
	
	hdl_mng_out = &hdl_mng;
	
}

void getLastMessageInHandleManager(HandleManager* hdl_mng, int handle_pid, Message* msg_output) {

	// If there is no handle, there is no message (TO REMOVE ?)
	if(hdl_mng->size == 0) {
		
		MessageType t = EMPTY;
		Message msg = {-1, -1, t, "-1"};
		memcpy(msg_output, &msg, sizeof(msg));
		return;

	}

	int index_handle = getIndexFromPid(handle_pid, hdl_mng);

	sem_wait(&(hdl_mng->mutex[index_handle]));
	
	int index_msg = hdl_mng->msg_mng[index_handle].size - 1;
	Message msg = hdl_mng->msg_mng[index_handle].messages[index_msg];
	memcpy(msg_output, &msg, sizeof(msg));

	sem_post(&(hdl_mng->mutex[index_handle]));
	
}

void sendMessageToDaemon(int daemon_pid, Message msg, char* profile_id, HandleManager* hdl_mng) {

	// Get the corresponding id
	int hdl_id = getIndexFromPid(getpid(), hdl_mng);
	
	// If the handle isn't registred yet and there is possibility to register a new handle
	if(hdl_id == -1 && hdl_mng->size < MAX_HANDLED_PROGRAM) {
		
		hdl_id = hdl_mng->size;
		hdl_mng->pid[hdl_id] = getpid();
		strcpy(hdl_mng->profile_id[hdl_id], profile_id);
		hdl_mng->state[hdl_id] = 1;
		sem_init(&(hdl_mng->mutex[hdl_id]), 1, 1);
		hdl_mng->size++;
	
	}
	//printf("send1\n");
	sem_trywait(&(hdl_mng->mutex[hdl_id]));
	//printf("send2\n");
	// Add message in the handle message's list
	addMessage(hdl_id, hdl_mng, msg);
	
	// Send a signal to the daemon indicating there is a new message
	kill(daemon_pid, SIGUSR1);
	
	// Indicate the message has been sent
	sem_post(&(hdl_mng->mutex[hdl_id]));
	
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

int getIndexFromProfileId(char* hdl_profile_id, HandleManager* hdl_mng) {
	
	int i;
	for(i = 0; i < hdl_mng->size; i++) {
		//printf("%i : %s\n", i, hdl_mng->profile_id[i]);
		if(strcmp(hdl_mng->profile_id[i], hdl_profile_id) == 0)
			return i;
	}
	return -1;
	
}

int getPidFromIndex(int hdl_index, HandleManager* hdl_mng) {

	if(hdl_index < hdl_mng->size)
		return hdl_mng->pid[hdl_index];

	return -1;
	
}

/// TEMP

int getHandlePidFromProfileId(char* hdl_profile_id, HandleManager* hdl_mng) {
	
	int index = getIndexFromProfileId(hdl_profile_id, hdl_mng);
	if(index > -1)
		return getPidFromIndex(index, hdl_mng);

	return -1;
	
}

char* printHandleManagerInfos(HandleManager* hdl_mng) {

	// Create json object for the handle manager
	json_t* obj_hdl_mng = json_object();
	json_object_set_new(obj_hdl_mng, "last_msg", json_integer(hdl_mng->last_msg)); // useful ?
		
	json_t* obj_array = json_array();
		
	// Create json object for arrays
	int size = 0;
	int i;
	for(i = 0; i < hdl_mng->size; i++) {
		
		// Parse only active handle
		if(hdl_mng->state[i] > 0) {
			
			size++;
		
			json_t *obj_data = json_object();
			//json_object_set_new(obj_data, "state",      json_integer(hdl_mng->state[i]));
			json_object_set_new(obj_data, "daemon_pid", json_integer(hdl_mng->daemon_pid[i]));
			json_object_set_new(obj_data, "pid",        json_integer(hdl_mng->pid[i]));
			json_object_set_new(obj_data, "profile_id", json_string(hdl_mng->profile_id[i]));
			
			// Parse MessageManager
			json_t* obj_array_mm = json_array();
			
			int j;
			for(j = 0; j < hdl_mng->msg_mng[i].size; j++) {
				
				json_t *obj_data_mm = json_object();
				json_object_set_new(obj_data_mm, "sender_pid", json_integer(hdl_mng->msg_mng[i].messages[j].sender_pid));
				json_object_set_new(obj_data_mm, "timestamp",  json_integer(hdl_mng->msg_mng[i].messages[j].timestamp));
				json_object_set_new(obj_data_mm, "type",       json_string(messageTypeToString(&(hdl_mng->msg_mng[i].messages[j].type))));
				json_object_set_new(obj_data_mm, "text",       json_string(hdl_mng->msg_mng[i].messages[j].text));
				
				json_array_append(obj_array_mm, obj_data_mm);
				
			}
			
			json_object_set_new(obj_data, "message", obj_array_mm);
			
			json_array_append(obj_array, obj_data);
		
		}
		
	}
		
	json_object_set_new(obj_hdl_mng, "size", json_integer(size));
		
	json_object_set_new(obj_hdl_mng, "handle", obj_array);
	
	// Convert to a json string (pretty printed)
	return json_dumps(obj_hdl_mng, 1);

}

int getFollowingDaemonPid(int hdl_pid, HandleManager* hdl_mng) {
	
	int index = getIndexFromPid(hdl_pid, hdl_mng);
	
	if(index > -1)
		return hdl_mng->daemon_pid[index];
	
	return -1;
	
}

// Update the following daemon. If one already exists, stop it before.
void updateFollowingDaemonPid(int hdl_pid, char* profile_id, int _daemon_pid, HandleManager* hdl_mng) {
	
	int index = getIndexFromPid(hdl_pid, hdl_mng);
	
	// If the handle isn't registred yet and there is possibility to register a new handle
	if(index == -1 && hdl_mng->size < MAX_HANDLED_PROGRAM) {
		
		index = hdl_mng->size;
		hdl_mng->pid[index] = getpid();
		strcpy(hdl_mng->profile_id[index], profile_id);
		hdl_mng->state[index] = 1;
		hdl_mng->daemon_pid[index] = -1;
		sem_init(&(hdl_mng->mutex[index]), 1, 1);
		hdl_mng->size++;
	
	}
	
	int following_daemon_pid = hdl_mng->daemon_pid[index];
	
	// If it exists, stop it
	if(following_daemon_pid > 0)
		kill(following_daemon_pid, SIGUSR2);	

	// Update the following daemon
	hdl_mng->daemon_pid[index] = _daemon_pid;
	
}

void stopSendSignalToFollowingDaemon(int hdl_pid, HandleManager* hdl_mng) {

	int index = getIndexFromPid(hdl_pid, hdl_mng);
	if(index > -1)
		hdl_mng->daemon_pid[index] = -1;

}

void handleIsStopped(int hdl_pid, HandleManager* hdl_mng) {
	
	int index = getIndexFromPid(hdl_pid, hdl_mng);
	hdl_mng->state[index] = -1;
	
}

/// END TEMP

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