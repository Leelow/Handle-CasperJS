#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "command.h"

int main(int argc, char * argv[]) {
	
	char result[25];
	get_command_path("casperjs", result);
	printf("\"%s\"", result);
	
	
}