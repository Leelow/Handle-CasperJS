#ifndef HANDLE_H_INCLUDED
#define HANDLE_H_INCLUDED

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "share.h"

/*** Buffer struct which is stored in a memory shared segment ***/
struct Buffer {
	
	// Number of lines in the buffer
	int nLine;
	
	// String array containing buffer lines
	char lines[MAX_LINE_BUFFER][MAX_STRING_LENGTH];
	
};
typedef struct Buffer Buffer;

/*** Get the buffer struct from a memory shared segment ***/
// shmid : id of the shared memory segment containing the Buffer struct
Buffer* accessToBuffer(int shmid);

/*** Get the number of line in the buffer ***/
// shmid : id of the shared memory segment containing the Buffer struct
// mutex : pointer to the mutex protecting the save operation
// line  : line to save
void addLineToBuffer(int shmid, sem_t* mutex, char line[MAX_STRING_LENGTH]);

/*** Get the number of line in the buffer ***/
// shmid : id of the shared memory segment containing the Buffer struct
int getNumberOfLineInBuffer(int shmid);

/*** Save the buffer in the log file ***/
// file  : pat of the file to save the buffer
// shmid : id of the shared memory segment containing the Buffer struct
// mutex : pointer to the mutex protecting the save operation
void saveBuffer(char file[MAX_STRING_LENGTH], int shmid, sem_t* mutex);

/*** Dissociate the shared memory segment containing the buffer from the current thread ***/
// buff : pointer to the buffer to dissociate
void dissociateBuffer(Buffer* buff);

/*** Free the shared memory segment containing the buffer if it's not register to any thread ***/
// shmid : id of the shared memory segment containing the Buffer struct
void freeBuffer(int shmid);

/*** Return the current system date in the format [dd-mm-yyyy hh:mm:ss] ***/
char* getSystemDate();




Buffer* accessToBuffer(int shmid) {
	Buffer* buff = (Buffer*) shmat(shmid, NULL, 0);
	if(buff == NULL) {
		printf("Can't attach the shared memory segment.\n");
		exit(-1);
	}
	return buff;
}

void addLineToBuffer(int shmid, sem_t* mutex, char line[MAX_STRING_LENGTH]) {
	Buffer* buff = accessToBuffer(shmid);
	
	sem_wait(mutex);
	
	// Check the index isn't out of bound
	if(buff->nLine < MAX_LINE_BUFFER)
		strcpy(buff->lines[buff->nLine++], line);
	
	sem_post(mutex);
	
	dissociateBuffer(buff);	
}


int getNumberOfLineInBuffer(int shmid) {
	Buffer* buff = accessToBuffer(shmid);
	
	int nLine = buff->nLine;
	
	dissociateBuffer(buff);
	
	return nLine;
}

void saveBuffer(char file[MAX_STRING_LENGTH], int shmid, sem_t* mutex) {	
	Buffer* buff = accessToBuffer(shmid);
	
	sem_wait(mutex);
	
	FILE *f = fopen(file, "a");
	int i = 0;
	while(i < buff->nLine)
		fprintf(f, "%s", buff->lines[i++]);

	buff->nLine = 0;
	fclose(f);
	
	sem_post(mutex);
		
	dissociateBuffer(buff);	
}

void dissociateBuffer(Buffer* buff) {
	if(shmdt(buff) == -1) {
		printf("Can't dissociate the shared memory segment.\n") ;
		exit(-1) ;
	}
}

void freeBuffer(int shmid) {	
	shmctl(shmid, IPC_RMID, NULL);		
}

char* getSystemDate() {	
	// Get local time
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	char* date;
	size_t size;
	
	// Get the size of the date string
	size = snprintf(NULL, 0, "[%d-%02d-%02d %02d:%02d:%02d]", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	date = (char *) malloc(size + 1);
	if(date == NULL)
		return "[0-0-0 0:0:0] ";
	else {
		snprintf(date, size + 1, "[%d-%02d-%02d %02d:%02d:%02d]", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
		return date;
	}
}

#endif