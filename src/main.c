// src/main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libspotify/api.h>
#include <string.h>
#include <pthread.h>

#define DEBUG 1

void debug(const char *format, ...)
{
    if (!DEBUG)
        return;

    va_list argptr;
    va_start(argptr, format);
    vprintf(format, argptr);
    printf("\n");
}

extern const uint8_t g_appkey[];
extern const size_t g_appkey_size;
extern const char *username;
extern const char *password;

int g_logged_in = 0;
int g_running;
bool browsing = 0;
int playlistCount = 0; 
int loaded = 0;
pthread_t tid[2];
int counter;
pthread_mutex_t lock;
sp_playlist *playListArray[50];
int playlistCounter;

static sp_playlist_callbacks pl_callbacks;

static void on_search_complete(sp_search *search, void *userdata) {
    debug("callback: on_search_complete");
    sp_error error = sp_search_error(search);
    if (error != SP_ERROR_OK) {
        fprintf(stderr, "Error: %s\n", sp_error_message(error));
        exit(1);
    }

   /* int num_tracks = sp_search_num_tracks(search);
    if (num_tracks == 0) {
        printf("Sorry, couldn't find that track. =/\n");
        exit(0);
    }*/

    const char *query = sp_search_query(search);
    int numPlaylist = sp_search_num_playlists(search);
    // TODO: play this track
    printf("Found something! %s with %d playlists \n", query, numPlaylist);
}

void run_search(sp_session *session) {
    // ask the user for an artist and track
/*    char artist[1024];
    printf("Artist: ");
    fgets(artist, 1024, stdin);
    artist[strlen(artist)-1] = '\0';*/

    char user[1024];
    printf("User: ");
    fgets(user, 1024, stdin);
    user[strlen(user)-1] = '\0';

    // format the query, e.g. "artist:<artist> track:<track>"
    char q[4096];
    sprintf(q, "spotify:user:%s", user);

    // start the search
    sp_search_create(session, q, 0, 1, 0, 0, 0, 0, 0, 0, SP_SEARCH_STANDARD,
        &on_search_complete, session);
}

//******************************************************************************************
//*                     LIST HANDLING CALLBACKS                                            *
//******************************************************************************************

static void playlist_browse_try() {

    for (int z = 0; z < 50; z++) {
        if (playListArray[z] == NULL) return;
        
        sp_playlist *currentPlaylist = playListArray[z];

         int i, tracks;

       printf("\tList address: %p \n", currentPlaylist);

       if (currentPlaylist == NULL) return;

       if(!sp_playlist_is_loaded(currentPlaylist)) {
            printf("\tPlaylist for browsing not loaded\n");
            return;
        } else {
            printf("\tPlaylist for browsing loaded\n");
        }

        tracks = sp_playlist_num_tracks(currentPlaylist);
        for(i = 0; i < tracks; i++) {
            sp_track *t = sp_playlist_track(currentPlaylist, i);
            if (!sp_track_is_loaded(t)){
                printf("\tTracks not ready \n");
                return;
            }
        }

        printf("Playlist %s \n", sp_playlist_name(currentPlaylist));

        for(i = 0; i < tracks; i++) {
            sp_track *t = sp_playlist_track(currentPlaylist, i);
            printf("%d - song: %s \n", i+1, sp_track_name (t));
        }

        
        sp_playlist_remove_callbacks(currentPlaylist, &pl_callbacks, NULL);
    }
}

static void pl_tracks_added(sp_playlist *pl, sp_track * const * tracks,
    int num_tracks, int position, void *userdata)
{
    printf("\t%d tracks added\n", num_tracks);
    //playlist_browse_try(pl);
}

static void pl_tracks_removed(sp_playlist *pl, const int *tracks,
  int num_tracks, void *userdata)
{
    printf("\t%d tracks removed\n", num_tracks);
}

static void pl_tracks_moved(sp_playlist *pl, const int *tracks,
    int num_tracks, int new_position, void *userdata)
{
    printf("\t%d tracks moved\n", num_tracks);
}

static void pl_renamed(sp_playlist *pl, void *userdata)
{
    printf("\tList name: %s\n",  sp_playlist_name(pl));
}

static void pl_state_change(sp_playlist *pl, void *userdata)
{
   printf("\tChanged statate: %s\n",  sp_playlist_name(pl));
   playlist_browse_try();
}

static sp_playlist_callbacks pl_callbacks = {
    pl_tracks_added,
    pl_tracks_removed,
    pl_tracks_moved,
    pl_renamed,
    pl_state_change,
};


void browse_playlist(sp_playlist *pl)
{
    //playlist_browse = pl;
    sp_playlist_add_callbacks(pl, &pl_callbacks, NULL);
    playlist_browse_try();
}

//******************************************************************************************
//*                   LIST CONTAINER HANDLING CALLBACKS                                    *
//******************************************************************************************


static void playlist_added(sp_playlistcontainer *pc, sp_playlist *pl,
 int position, void *userdata)
{
    printf("\t playlist loaded %s \n", sp_playlist_name(pl));
    playListArray[playlistCounter] = pl;
    playlistCounter++;
    browse_playlist(pl);
    /*if (loaded == 1) {
        printf("\t Sorry already loadded a list \n");
    } else {
        browse_playlist(pl);
        //sp_playlist_add_callbacks(pl, &pl_callbacks, NULL);
        loaded = 1;
    }*/
    //we can also remove the callbacks for the container
}

static void playlist_removed(sp_playlistcontainer *pc, sp_playlist *pl,
   int position, void *userdata)
{
    printf("\t playlist removed \n");
    playlistCount--;
    printf("lists loaded %d \n", playlistCount);
    sp_playlist_remove_callbacks(pl, &pl_callbacks, NULL);
    //sp_playlist_release(pl);
}


static void container_loaded(sp_playlistcontainer *pc, void *userdata)
{
    printf("\t container loaded \n");
}

static sp_playlistcontainer_callbacks pc_callbacks = {
    .playlist_added = &playlist_added,
    .playlist_removed = &playlist_removed,
    .container_loaded = &container_loaded,
};

//******************************************************************************************
//*                       MAIN PROGRAM                                                     *
//******************************************************************************************

static void on_login(sp_session *session, sp_error error)
{
    debug("callback: on_login");
    if (error != SP_ERROR_OK) {
        fprintf(stderr, "Error: unable to log in: %s\n", sp_error_message(error));
        exit(1);
    }

    g_logged_in = 1;

    /*run_search(session);*/

    /*sp_user *user = sp_session_user(session);
    const char *userName = sp_user_display_name (user);*/
    
    /*sp_playlist* playlist = sp_session_starred_for_user_create (session,"tonychenkt"); 
    browse_playlist(playlist);  */
    /*sp_playlist *container = sp_session_starred_create(session);*/
    // rwrofl, goofyahead
    sp_playlistcontainer *container = sp_session_publishedcontainer_for_user_create(session, "123070624");
    sp_playlistcontainer_add_callbacks(container,&pc_callbacks,NULL);

    /*const char *nameList = sp_playlist_name (container);*/
}

static void on_main_thread_notified(sp_session *session)
{
    debug("callback: on_main_thread_notified");
    pthread_mutex_unlock(&lock);
}

static int on_music_delivered(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames)
{
    debug("callback: on_music_delivered");
    return 0;
}

static void on_log(sp_session *session, const char *data)
{
    // this method is *very* verbose, so this data should really be written out to a log file
}

static void on_end_of_track(sp_session *session)
{
    debug("callback: on_end_of_track");
}

static void metadata_updated(sp_session *sess)
{
   debug("metadata updated");
   playlist_browse_try();
}

static sp_session_callbacks session_callbacks = {
    .logged_in = &on_login,
    .notify_main_thread = &on_main_thread_notified,
    .music_delivery = &on_music_delivered,
    .log_message = &on_log,
    .metadata_updated = &metadata_updated,
    .end_of_track = &on_end_of_track
};

static sp_session_config spconfig = {
    .api_version = SPOTIFY_API_VERSION,
    .cache_location = "tmp",
    .settings_location = "tmp",
    .application_key = g_appkey,
    .application_key_size = 0, // set in main()
    .user_agent = "spot",
    .callbacks = &session_callbacks,
    NULL
};

int main(void)
{
    if (pthread_mutex_init(&lock, NULL) != 0)
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

    sp_session_login(session, username, password, 0, NULL);
    
    g_running = 1;
    int next_timeout = 1;

    
    while (g_running) {
        sp_session_process_events(session, &next_timeout);
        printf("should wait for %d \n",next_timeout);
        usleep(next_timeout);
        pthread_mutex_lock(&lock);
    }

    printf("success!\n");
    return 0;
}
