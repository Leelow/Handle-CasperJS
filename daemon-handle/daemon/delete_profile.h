#ifndef DELETEPROFILE_H_INCLUDED
#define DELETEPROFILE_H_INCLUDED

/*** Delete a profile thanks to its profile_id ***/
// profile_id : id of the profile to delete
int delete_profile(char* profile_id) {
	
	// Get the profile dir path
	char* profile_dir = concat(PATH_PROFILES, profile_id);
	strcat(profile_dir, "/");
	
	// Delete the profile dir and all it's content
	del_dir(profile_dir);
	
}

#endif