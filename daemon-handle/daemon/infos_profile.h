#ifndef INFOSPROFILE_H_INCLUDED
#define INFOSPROFILE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

/*** Show infos about a profile thanks to its profile_id ***/
// profile_id : id of the profile to get infos
// str_output : pointer to the ouput string
char* infos_profile(char* profile_id) {
	
	// Get the profile's settings file path
	char* profile_settings_file_path = concat(PATH_PROFILES, profile_id);
	profile_settings_file_path = concat(profile_settings_file_path, "/");
	profile_settings_file_path = concat(profile_settings_file_path, PROFILE_SETTINGS_FILE);
	
	FILE* f = fopen(profile_settings_file_path, "r");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);

	fseek(f, 0, SEEK_SET);

	char *json_str = malloc(fsize + 1);
	fread(json_str, fsize, 1, f);
	fclose(f);
	
	return json_str;
	 
}

#endif