#include "file.h"
#include <unistd.h>

void multi_close_file(int fd) {
    close(fd);
    pthread_cond_signal(&open_cond);
}

void multi_close_dir(DIR* fd) {
    closedir(fd);
    pthread_cond_signal(&open_cond);
}
