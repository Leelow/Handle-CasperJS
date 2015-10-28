#ifndef CUSTOMSIGNAL_H_INCLUDED
#define CUSTOMSIGNAL_H_INCLUDED

#include <signal.h>
#include <unistd.h>

typedef void InfoHandler(int, siginfo_t *, void *);

int signalWithInfo(int signum, InfoHandler* handler) {
	
	struct sigaction action, old_action;

	memset(&action, 0, sizeof(struct sigaction));
	action.sa_sigaction = handler;  
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_RESTART | SA_SIGINFO;

	return sigaction(signum, &action, &old_action);

}

#endif