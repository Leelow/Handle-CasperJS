#ifndef SHARE_H_INCLUDED
#define SHARE_H_INCLUDED

#define MAX_STRING_LENGTH 256
#define MAX_LINE_BUFFER 150
#define MAX_DELAY_BETWEEN_LOGS 5

#define PATH_SETTINGS_FILE		"settings.json"
#define PATH_DAEMON   			"/home/leo/casperjs_daemon/"
#define PATH_PROFILES 			"/home/leo/casperjs_daemon/profiles/"
#define PATH_SETTINGS 			"/home/leo/casperjs_daemon/settings.json"

#define PROFILE_SETTINGS_FILE	"settings.json"
#define PROFILE_SCRIPT_FILE	    "script.js"

#define VERSION "0.1"

#include "filesystem.h"

#include "settingsDaemon.h"
#include "settingsHandle.h"

#include "customSignal.h"
#include "signalManager.h"
	
#endif