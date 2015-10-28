#ifndef SHARE_H_INCLUDED
#define SHARE_H_INCLUDED

#include <jansson.h> // JSON parser

#define MAX_STRING_LENGTH 256
#define MAX_LINE_BUFFER 150
#define MAX_DELAY_BETWEEN_LOGS 5

#define PATH_SETTINGS_FILE	"settings.json"
#define PATH_DAEMON   		"/home/leo/casperjs_daemon/"
#define PATH_PROFILES 		"/home/leo/casperjs_daemon/profiles/"
#define PATH_SETTINGS 		"/home/leo/casperjs_daemon/settings.json"

#define VERSION "0.1"

typedef struct { 

   int id_daemon;
   char* path_daemon;

} Settings;

/*** Convert a Settings struct to a json string ***/
// s : id of the shared memory segment containing the Buffer struct
char* settingsToJson(Settings* s);


Settings jsonToSettings(char* json);
int saveSettingsToFile(char* path, Settings* s);
int loadSettingsFromFile(char* path, Settings* s);

char* settingsToJson(Settings* s) {
	
	// Create json object
	json_t *obj = json_object();
	json_object_set_new(obj, "id_daemon",   json_integer(s->id_daemon));
	json_object_set_new(obj, "path_daemon", json_string(s->path_daemon));
	
	// Convert to a json string (pretty printed)
	return json_dumps(obj, 1);
	
}

Settings jsonToSettings(char* json) {
	
	// Returned object if error
	json_error_t err = {0}; 

	// Parse the object
	json_t* obj = json_loads(json, 0, &err);
	json_t* j_id_daemon = json_object_get(obj, "id_daemon");
	json_t* j_path_daemon = json_object_get(obj, "path_daemon");
	
	// Get the fields of the object
	int id_daemon = json_integer_value(j_id_daemon);
	const char* c_path_daemon = json_string_value(j_path_daemon);
	char* path_daemon = malloc(sizeof(c_path_daemon));
	memcpy(path_daemon, c_path_daemon, sizeof(c_path_daemon));
	
	// Free tje json object
	json_decref(obj);
	
	Settings s = {id_daemon, path_daemon};
	return s;
	
}

int saveSettingsToFile(char* path, Settings* s) {
	
	//TODO : check
	FILE *f = fopen(path, "w");
	fprintf(f, "%s", settingsToJson(s));
    fclose(f);	
}

int loadSettingsFromFile(char* path, Settings* s) {
	//TODO : check
	
	// Read all the contents of the file
	FILE *f = fopen(path, "r");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *json_str = malloc(fsize + 1);
	fread(json_str, fsize, 1, f);
	fclose(f);
	
	Settings s_file = jsonToSettings(json_str);
	memcpy(s, &s_file, sizeof(s_file));
	
}
	
#endif