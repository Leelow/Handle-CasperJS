#ifndef CREATEPROFILE_H_INCLUDED
#define CREATEPROFILE_H_INCLUDED

/*** Create a new profile with a name and a file. Return the id of the new profile ***/
// name               : the name of the profile
// file               : path of the script file
// web_browser_engine : name of the web browser engine
char* create_profile(char* name, char* file, char* web_browser_engine) {
	
	// TODO : Protect name with "/"
	
	// TODO : Check if the name is already taken or not ???
	
	// Check if the given file's path is correct
	if(file_exist(file) == -1) {
		printf("The file \"%s\" dosen't exist.\n", file);
		exit(-1);
	}
	
	// Generate an unique profile_id thanks to the timestamp
	int timestamp = (int) time(NULL);
	char timestamp_str[11];
	sprintf(timestamp_str, "%d", timestamp);
	
	char* profile_id = concat(name, "_");
	strcat(profile_id, timestamp_str);

	// Create the dir corresponding to the new profile
	char* profile_path = concat(PATH_PROFILES, profile_id);
	strcat(profile_path, "/");
	if (dir_exist(profile_path) == -1)
		mkdir(profile_path, 0777);
	
	// Create the dir for the profile's logs file
	char* profile_logs_path = concat(profile_path, "logs");
	strcat(profile_logs_path, "/");
	if (dir_exist(profile_logs_path) == -1)
		mkdir(profile_logs_path, 0777);
	
	// Save the handle's settings in the settings file
	SettingsHandle settings = {profile_id, timestamp, web_browser_engine};
	char* settings_file_path = concat(profile_path, "settings.json");
	saveSettingsHandleToFile(settings_file_path, &settings);
	
	// Get the absolute path for the copied file
	char* file_path = concat(profile_path, file);
	
	// Copy the file to the profile path
	copy_file(file_path, file);
		
	// Return the id
	return profile_id;
	
}
#endif