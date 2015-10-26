#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#define SPECTRE_VERSION "0.1a"

#define MAX_STRING_LENGTH 256 /* Correspond à la taille de la chaîne à lire */

#define TEMP_PATH "/home/leo/handlerCasperJS/spectre/"
#define SCRIPT_DIR "scripts/"

int main(int argc, char * argv[]) {
	
	// char* temp_path = TEMP_PATH;
	// char* script_dir = SCRIPT_DIR;
	
	// /*** Check if spectre is already install  ***/
	
	// // Create the main dir if necessary
	// if (dir_exist(TEMP_PATH) == -1)
		// mkdir(TEMP_PATH, 0777);
	
	// // Create the script dir if necessary
	// char script_dir[50];
	// script_dir = concat(temp_path, script_dir);
	// //strcpy(script_dir, temp_path);
	// //strcat(script_dir, script_dir);
	// printf("\"%s\"\n", script_dir);
	
	// if (dir_exist(script_dir) == -1)
		// mkdir(script_dir, 0777);
	
	// // ARG --version
	// if(argc > 1 && strcmp("--version", argv[1]) == 0)
		// printf("%s\n", SPECTRE_VERSION);
	
	// // ARG --create id file <engine> <info>
	// if(argc > 1 && strcmp("--create", argv[1]) == 0) {
		// if(argc < 4) printf("Too few arguments : spectre --create id file <engine> <info>\n");
		// if(argc > 6) printf("Too much arguments : spectre --create id file <engine> <info>\n");
	
		// char* id = argv[2];
		// char* file = argv[3];
		
		// // Check if the id is already taken or not
		// //if(dir_exist(concat(TEMP_PATH, 
		
		// printf("id : \"%s\"\n", id);
		// printf("file : \"%s\"\n", file);
	
	// }
	
	// Capture casperJS output (PAS EFFACER)
	// int fp2[2];
	// pipe(fp2);
  
    // close(fp2[0]); // on ferme en lecture
    // dup2(fp2[1], STDIN_FILENO);
    // execlp("/usr/local/bin/casperjs", "casperjs", argv[1], NULL);
	
	
	char line[MAX_STRING_LENGTH];
    FILE *fpin;

	
    fpin = popen("casperjs \"/home/leo/test.js\"", "r");;
    while(fgets(line, MAX_STRING_LENGTH, fpin) != NULL) {
		
		printf("%s", line);
		
		
	}
	
}

int dir_exist(char* path) {
	struct stat st = {0};
	return stat(TEMP_PATH, &st);
}

int file_exist(char* path) {
	return dir_exist(path);
}

char* concat(char *s1, char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
	printf("%s\n", s2);
    return result;
}