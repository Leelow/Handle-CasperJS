# Handle-CasperJS

__Daemon__

There are currently eight available commands :

    create      name file                 | Create the filetree for the new profile. Return the new profile_id.
    status                                | Print status of the daemon including all handle's infos.

    schedule    profile_id cron           | TODO
    delete      profile_id                | Delete the filetree corresponding to the profile.
    update      profile_id file           | TODO
    start       profile_id (--follow)     | Start a profile. If --follow is indicated, print all received signals.
    stop        profile_id                | Stop a profile.
    follow      profile_id                | TODO
    infos       profile_id                | Print infos of the corresponding profile.
