#ifndef SETTINGSHANDLE_H_INCLUDED
#define SETTINGSHANDLE_H_INCLUDED

#include <jansson.h> // JSON parser

/*******************************************************************/
/*                                                                 */
/*						Handle Settings                            */
/*                                                                 */
/*******************************************************************/

typedef struct { 

   char* id;
   int timestamp_creation;
   char* web_browser_engine;

} SettingsHandle;

/*** Convert a SettingsHandle struct to a json string ***/
// s : pointer to the SettingsHandle to convert
char* settingsHandleToJson(SettingsHandle* s);

/*** Convert a json string to a SettingsHandle struct ***/
// json_str : json string to convert
SettingsHandle jsonToSettingsHandle(char* json_str);

/*** Save a SettingsHandle into a file ***/
// path : path to the output file
// s    : pointer to the SettingsHandle to save
int saveSettingsHandleToFile(char* path, SettingsHandle* s);

/*** Load a SettingsHandle from a file ***/
// path : path to the input file
// s    : pointer to the dest SettingsHandle
int loadSettingsHandleFromFile(char* path, SettingsHandle* s);

char* settingsHandleToJson(SettingsHandle* s) {
	
	// Create json object
	json_t *obj = json_object();
	json_object_set_new(obj, "id",                 json_string(s->id));
	json_object_set_new(obj, "timestamp_creation", json_integer(s->timestamp_creation));
	json_object_set_new(obj, "web_browser_engine", json_string(s->web_browser_engine));
	
	// Convert to a json string (pretty printed)
	return json_dumps(obj, 1);
	
}

SettingsHandle jsonToSettingsHandle(char* json_str) {
	
	// Returned object if error
	json_error_t err = {0}; 

	// Parse the object
	json_t* obj = json_loads(json_str, 0, &err);
	json_t* j_id = json_object_get(obj, "id");
	json_t* j_timestamp_creation = json_object_get(obj, "timestamp_creation");
	json_t* j_web_browser_engine = json_object_get(obj, "web_browser_engine");
	
	// Get the fields of the object
	const char* c_id = json_string_value(j_id);	
	char* id = malloc(sizeof(c_id));
	memcpy(id, c_id, sizeof(c_id));
	
	int timestamp_creation = json_integer_value(j_timestamp_creation);
	
	const char* c_web_browser_engine = json_string_value(j_web_browser_engine);	
	char* web_browser_engine = malloc(sizeof(c_web_browser_engine));
	memcpy(web_browser_engine, c_web_browser_engine, sizeof(c_web_browser_engine));
	
	// Free tje json object
	json_decref(obj);
	
	SettingsHandle s = {id, timestamp_creation, web_browser_engine};
	return s;
	
}

int saveSettingsHandleToFile(char* path, SettingsHandle* s) {
	
	//TODO : check
	FILE *f = fopen(path, "w");
	fprintf(f, "%s", settingsHandleToJson(s));
    fclose(f);	
}

int loadSettingsHandleFromFile(char* path, SettingsHandle* s) {
	//TODO : check
	
	// Read all the contents of the file
	FILE *f = fopen(path, "r");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *json_str = malloc(fsize + 1);
	fread(json_str, fsize, 1, f);
	fclose(f);
	
	SettingsHandle s_file = jsonToSettingsHandle(json_str);
	memcpy(s, &s_file, sizeof(s_file));
	
}

#endif