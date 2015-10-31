#ifndef SETTINGSDAEMON_H_INCLUDED
#define SETTINGSDAEMON_H_INCLUDED

#include <jansson.h> // JSON parser

/*******************************************************************/
/*                                                                 */
/*						Daemon Settings                            */
/*                                                                 */
/*******************************************************************/

typedef struct { 

   int id_daemon;
   char* path_daemon;

} SettingsDaemon;

/*** Convert a SettingsDaemon struct to a json string ***/
// s : pointer to the SettingsDaemon to convert
char* settingsDaemonToJson(SettingsDaemon* s);

/*** Convert a json string to a SettingsDaemon struct ***/
// json_str : json string to convert
SettingsDaemon jsonToSettingsDaemon(char* json_str);

/*** Save a SettingsDaemon into a file ***/
// path : path to the output file
// s    : pointer to the SettingsDaemon to save
int saveSettingsDaemonToFile(char* path, SettingsDaemon* s);

/*** Load a SettingsDaemon from a file ***/
// path : path to the input file
// s    : pointer to the dest SettingsDaemon
int loadSettingsDaemonFromFile(char* path, SettingsDaemon* s);

char* settingsDaemonToJson(SettingsDaemon* s) {
	
	// Create json object
	json_t *obj = json_object();
	json_object_set_new(obj, "id_daemon",   json_integer(s->id_daemon));
	json_object_set_new(obj, "path_daemon", json_string(s->path_daemon));
	
	// Convert to a json string (pretty printed)
	return json_dumps(obj, 1);
	
}

SettingsDaemon jsonToSettingsDaemon(char* json_str) {
	
	// Returned object if error
	json_error_t err = {0}; 

	// Parse the object
	json_t* obj = json_loads(json_str, 0, &err);
	json_t* j_id_daemon = json_object_get(obj, "id_daemon");
	json_t* j_path_daemon = json_object_get(obj, "path_daemon");
	
	// Get the fields of the object
	int id_daemon = json_integer_value(j_id_daemon);
	const char* c_path_daemon = json_string_value(j_path_daemon);
	char* path_daemon = malloc(sizeof(c_path_daemon));
	memcpy(path_daemon, c_path_daemon, sizeof(c_path_daemon));
	
	// Free tje json object
	json_decref(obj);
	
	SettingsDaemon s = {id_daemon, path_daemon};
	return s;
	
}

int saveSettingsDaemonToFile(char* path, SettingsDaemon* s) {
	
	//TODO : check
	FILE *f = fopen(path, "w");
	fprintf(f, "%s", settingsDaemonToJson(s));
    fclose(f);	
}

int loadSettingsDaemonFromFile(char* path, SettingsDaemon* s) {
	//TODO : check
	
	// Read all the contents of the file
	FILE *f = fopen(path, "r");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *json_str = malloc(fsize + 1);
	fread(json_str, fsize, 1, f);
	fclose(f);
	
	SettingsDaemon s_file = jsonToSettingsDaemon(json_str);
	memcpy(s, &s_file, sizeof(s_file));
	
}

#endif