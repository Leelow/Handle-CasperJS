#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "command.h"

/* Get the full path of a command */
void get_command_path(char* command_name, char* path)
{
	
	FILE *fp;
	
	// Exec the command
	char command[MAX_STRING_LENGTH];
	strcpy(command, "which ");
	strcat(command, command_name);
	fp = popen(command, "r");
	
	// Check if everything works
	if (fp == NULL) {
		printf("Failed to execute command \"%s\".\n", command);
		exit(1);
	}

	char output_t[MAX_STRING_LENGTH];
	
	// Get the command result
	fgets(output_t, sizeof(output_t) - 1, fp);
	
	// Remove the "\n" dues to fgets behavior
	size_t size = strlen(output_t);
	if (output_t[size - 1] == '\n') output_t[size - 1] = '\0';
	
	// Copy the string to the output
	strcpy(path, output_t);
	
}