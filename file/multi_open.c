#include "file.h"
#include <errno.h>
#include <fcntl.h>

pthread_cond_t open_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t open_mutex = PTHREAD_MUTEX_INITIALIZER;

int multi_open_file(const char *path, int flags, mode_t mode) {
    int code;

    pthread_mutex_lock(&open_mutex);
    do {
        code = open(path, flags, mode);
        if (code < 0 && errno == EMFILE) {
            pthread_cond_wait(&open_cond, &open_mutex);
        }
    } while (code < 0 && errno == EMFILE);
    
    pthread_mutex_unlock(&open_mutex);
    return code;
}

DIR* multi_open_dir(const char *path) { 
    DIR *dir;

    pthread_mutex_lock(&open_mutex);
    do  {
        dir = opendir(path);
        if (!dir && errno == EMFILE) {
            pthread_cond_wait(&open_cond, &open_mutex);
        }
    } while (!dir && errno == EMFILE);

    pthread_mutex_unlock(&open_mutex);
    return dir;
}
