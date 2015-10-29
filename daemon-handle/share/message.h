#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <jansson.h> // JSON parser

/*******************************************************************/
/*                                                                 */
/*						     Message                               */
/*                                                                 */
/*******************************************************************/


/*** Convert a messageType into a string ***/
// mt : pointer to the messageType to convert
char* messageTypeToString(MessageType* mt);

/*** Convert a message into a json string ***/
// m : message to convert
char* messageToJson(Message* m)

typedef enum MessageType {
	
	ERROR_FILE, ERROR_SYNTAX, INFOS, DEBUG, ERROR, START, END, EMPTY
	
} MessageType;

typedef struct Message {
	
	// Id of the message's sender
	int sender_pid;
	
	// Timestamp of the message
	int timestamp;
	
	// Type of the message
	MessageType type;
	
	// Plain text of the message
	char text[MAX_STRING_LENGTH];
	
} Message;

char* messageTypeToString(MessageType* mt) {
	
	char* mt_str;
	
	switch((int) *mt) {
		case ERROR_FILE:
			mt_str = "ERROR_FILE";
		break;
		case ERROR_SYNTAX:
			mt_str = "ERROR_SYNTAX";
		break;
		case INFOS:
			mt_str = "INFOS";
		break;
		case DEBUG:
			mt_str = "DEBUG";
		break;
		case ERROR:
			mt_str = "ERROR";
		break;
		case START:
			mt_str = "START";
		break;
		case END:
			mt_str = "END";
		break;
		case EMPTY:
			mt_str = "EMPTY";
		break;
		default:
			mt_str = "UNKNOWN";
		break;
	}
	
	return mt_str;
	
}

char* messageToJson(Message* m) {
	
	// Create json object
	json_t *obj = json_object();
	json_object_set_new(obj, "sender_pid", json_integer(m->sender_pid));
	json_object_set_new(obj, "timestamp",  json_integer(m->timestamp));
	json_object_set_new(obj, "type", 	   json_string(messageTypeToString(&m->type)));
	json_object_set_new(obj, "text",       json_string(m->text));
	
	// Convert to a json string (pretty printed)
	return json_dumps(obj, 1);
	
}

#endif