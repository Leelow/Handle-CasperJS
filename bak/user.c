#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "command.h"

int main(int argc, char * argv[]) {
	
	/*** Get adduser program path ***/	
	char adduser_path[100];
	exec_command("which adduser", adduser_path);
	//printf("\"%s\"\n", adduser_path);
	
	/*** Check if user "spectre" already exists ***/
	char adduser_list[2048];
	exec_command("cut -d: -f1 /etc/passwd", adduser_list);
	printf("\"%s\"\n", adduser_list);
	
	// TODO User creation and set permission to folder
	
}