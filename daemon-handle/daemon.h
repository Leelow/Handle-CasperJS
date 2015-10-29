#ifndef DAEMON_H_INCLUDED
#define DAEMON_H_INCLUDED

#include "share/share.h"

/*** Handle manager informations ***/
HandleManager* hdl_mng;
int volatile shmid_hdl_mng;

/*** Commands inclusion ***/
#include "daemon/create_profile.h"
#include "daemon/delete_profile.h"
#include "daemon/infos_profile.h"
#include "daemon/start_profile.h"
#include "daemon/follow_profile.h"

#endif