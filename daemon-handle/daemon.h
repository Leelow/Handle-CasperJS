#include <sys/stat.h>

int dir_exist(char* path);
int file_exist(char* path);

int dir_exist(char* path) {
	struct stat st = {0};
	return stat(path, &st);
}

int file_exist(char* path) {
	return dir_exist(path);
}