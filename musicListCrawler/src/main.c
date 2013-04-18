// src/main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libspotify/api.h>
#include <string.h>
#include <pthread.h>

#define DEBUG 0

void debug(const char *format, ...)
{
    if (!DEBUG) return;

    va_list argptr;
    va_start(argptr, format);
    vprintf(format, argptr);
    printf("\n");
}

extern const uint8_t g_appkey[];
extern const size_t g_appkey_size;
extern const char *username;
extern const char *password;

char *currentUser;
int g_logged_in = 0;
int g_running;
bool browsing = 0;
int loaded = 0;
pthread_t tid[2];
int counter;
pthread_mutex_t lock;
pthread_mutex_t arrayLock;
sp_playlist *playListArray[50];
bool checked[50];
int playlistCounter = 0;
int processedCounter = 0;

sp_playlist_callbacks pl_callbacks;

void on_search_complete(sp_search *search, void *userdata) {
    debug("callback: on_search_complete");
    sp_error error = sp_search_error(search);
    if (error != SP_ERROR_OK) {
        if (DEBUG) fprintf(stderr, "Error: %s\n", sp_error_message(error));
        exit(1);
    }

    const char *query = sp_search_query(search);
    int numPlaylist = sp_search_num_playlists(search);
    // TODO: play this track
    if (DEBUG) printf("Found something! %s with %d playlists \n", query, numPlaylist);
}

void run_search(sp_session *session) {

    char user[1024];
    if (DEBUG) printf("User: ");
    fgets(user, 1024, stdin);
    user[strlen(user)-1] = '\0';

    // format the query, e.g. "artist:<artist> track:<track>"
    char q[4096];
    if (DEBUG) sprintf(q, "spotify:user:%s", user);

    // start the search
    sp_search_create(session, q, 0, 1, 0, 0, 0, 0, 0, 0, SP_SEARCH_STANDARD,
        &on_search_complete, session);
}

//******************************************************************************************
//*                     LIST HANDLING CALLBACKS                                            *
//******************************************************************************************

void playlist_browse_try() {
    //pthread_mutex_lock(&arrayLock);
    if (browsing == 1) {
        if (DEBUG) printf("sorry \n");
        return;
    } 
    browsing = 1;
    for (int z = 0; z < 50; z++) {
        if (DEBUG) printf("%d \n", z);
        if (playListArray[z] == NULL) {
            browsing = 0;
            return;
        } 
        
    sp_playlist *currentPlaylist = playListArray[z];

       int i, tracks;

       if (currentPlaylist == NULL) {
            browsing = 0;
            return;
        } 

       if(!sp_playlist_is_loaded(currentPlaylist)) {
            if (DEBUG) printf("\tPlaylist for browsing not loaded\n");
            browsing = 0;
            return;
        } else {
            if (DEBUG) printf("\tPlaylist for browsing loaded %s \n", sp_playlist_name(currentPlaylist));
        }

        tracks = sp_playlist_num_tracks(currentPlaylist);
        for(i = 0; i < tracks; i++) {
            sp_track *t = sp_playlist_track(currentPlaylist, i);
            if (!sp_track_is_loaded(t)){
                if (DEBUG) printf("\tTracks not ready \n");
                browsing = 0;
                return;
            }
        }

        if (DEBUG) printf("Playlist %s \n", sp_playlist_name(currentPlaylist));
        
        if (checked[z] != 1) {
            for(i = 0; i < tracks; i++) {
                sp_track *t = sp_playlist_track(currentPlaylist, i);
                printf("%d - song: %s \n", i+1, sp_track_name (t));
                if (i == tracks-1) {
                    checked[z] = 1;
                    printf ("last song was printed ------------------ \n");
                    processedCounter++;
                } 
            }
        }

        //sp_playlist_remove_callbacks(currentPlaylist, &pl_callbacks, NULL);
        //sp_playlist_release(currentPlaylist);*/
        
        printf("process count %d : %d \n", playlistCounter, processedCounter);
        if (playlistCounter == processedCounter){
            printf("Exiting process correctly");
            g_running = 0;
        } 
    }
    browsing = 0;
}

void pl_tracks_added(sp_playlist *pl, sp_track * const * tracks,
    int num_tracks, int position, void *userdata)
{
    if (DEBUG) printf("\t%d tracks added\n", num_tracks);
}

void pl_tracks_removed(sp_playlist *pl, const int *tracks,
  int num_tracks, void *userdata)
{
    if (DEBUG) printf("\t%d tracks removed\n", num_tracks);
}

void pl_tracks_moved(sp_playlist *pl, const int *tracks,
    int num_tracks, int new_position, void *userdata)
{
    if (DEBUG) printf("\t%d tracks moved\n", num_tracks);
}

void pl_renamed(sp_playlist *pl, void *userdata)
{
    if (DEBUG) printf("\tList name: %s\n",  sp_playlist_name(pl));
}

void pl_state_change(sp_playlist *pl, void *userdata)
{
   if (DEBUG) printf("\tChanged statate: %s\n",  sp_playlist_name(pl));
   //playlist_browse_try();
}

sp_playlist_callbacks pl_callbacks = {
    pl_tracks_added,
    pl_tracks_removed,
    pl_tracks_moved,
    pl_renamed,
    pl_state_change,
};


void browse_playlist(sp_playlist *pl)
{
    sp_playlist_add_callbacks(pl, &pl_callbacks, NULL);
    //playlist_browse_try();
}

//******************************************************************************************
//*                   LIST CONTAINER HANDLING CALLBACKS                                    *
//******************************************************************************************


void playlist_added(sp_playlistcontainer *pc, sp_playlist *pl,
 int position, void *userdata)
{
    char *radioList = "Liked from Radio";
    const char *name = sp_playlist_name(pl);
    //if (strlen(name) < 1) return;
    if (playlistCounter >= 50) return;
    if (!strcmp(name, radioList)) return;
    printf("\t playlist loaded %s, number: %d \n", name, playlistCounter);
    playListArray[playlistCounter] = pl; //its overriding other playlists!!!
    playlistCounter++;
    browse_playlist(pl);
}

void playlist_removed(sp_playlistcontainer *pc, sp_playlist *pl,
   int position, void *userdata)
{
    if (DEBUG) printf("\t playlist removed \n");
    if (DEBUG) printf("lists loaded %d \n", playlistCounter);
}


void container_loaded(sp_playlistcontainer *pc, void *userdata)
{
    if (DEBUG) printf("\t container loaded \n");
}

sp_playlistcontainer_callbacks pc_callbacks = {
    .playlist_added = &playlist_added,
    .playlist_removed = &playlist_removed,
    .container_loaded = &container_loaded,
};

//******************************************************************************************
//*                       MAIN PROGRAM                                                     *
//******************************************************************************************

void on_login(sp_session *session, sp_error error)
{
    debug("callback: on_login");
    if (error != SP_ERROR_OK) {
        if (DEBUG) fprintf(stderr, "Error: unable to log in: %s\n", sp_error_message(error));
        exit(1);
    }

    g_logged_in = 1;

    // rwrofl, goofyahead
    sp_playlistcontainer *container = sp_session_publishedcontainer_for_user_create(session, currentUser);
    sp_playlistcontainer_add_callbacks(container,&pc_callbacks,NULL);
}

void on_main_thread_notified(sp_session *session)
{
    debug("callback: on_main_thread_notified");
    pthread_mutex_unlock(&lock);
}

int on_music_delivered(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames)
{
    debug("callback: on_music_delivered");
    return 0;
}

void on_log(sp_session *session, const char *data)
{
    // this method is *very* verbose, so this data should really be written out to a log file
}

void on_end_of_track(sp_session *session)
{
    debug("callback: on_end_of_track");
}

void metadata_updated(sp_session *sess)
{
   debug("metadata updated");
   playlist_browse_try();
}

sp_session_callbacks session_callbacks = {
    .logged_in = &on_login,
    .notify_main_thread = &on_main_thread_notified,
    .music_delivery = &on_music_delivered,
    .log_message = &on_log,
    .metadata_updated = &metadata_updated,
    .end_of_track = &on_end_of_track
};

sp_session_config spconfig = {
    .api_version = SPOTIFY_API_VERSION,
    .cache_location = "tmp",
    .settings_location = "tmp",
    .application_key = g_appkey,
    .application_key_size = 0, // set in main()
    .user_agent = "spot",
    .callbacks = &session_callbacks,
    NULL
};

int main(int argc, char **argv){
    if (argv[1] == NULL){
        printf("Need to pass a spotify id\n");
        return 1;
    } else {
        currentUser = argv[1];
    }

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    if (pthread_mutex_init(&arrayLock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    sp_error error;
    sp_session *session;

    // create the spotify session
    spconfig.application_key_size = g_appkey_size;
    error = sp_session_create(&spconfig, &session);
    
    if (error != SP_ERROR_OK) {
        fprintf(stderr, "Error: unable to create spotify session: %s\n", sp_error_message(error));
        return 1;
    }

    for (int r=0 ; r < 50 ; r++) {
        playListArray[r] = NULL;
    }
    
    sp_session_login(session, username, password, 0, NULL);
    
    g_running = 1;
    int next_timeout = 1;

    
    while (g_running) {
        sp_session_process_events(session, &next_timeout);
        if (DEBUG) printf("should wait for %d \n",next_timeout);
        usleep(next_timeout * 100);
        pthread_mutex_lock(&lock);
    }

    printf("success!\n");
    return 0;
}
