#ifndef FILE_H
#define FILE_H
#include <pthread.h>
#include <sys/types.h>
#include <dirent.h>

extern pthread_cond_t open_cond;
extern pthread_mutex_t open_mutex;

int cp_dir(const char* path_from, const char* path_to);
int cp_file(const char* path_from, const char* path_to);
int cp_r(const char* path_from, const char* path_to);
void multi_close_file(int fd);
int multi_open_file(const char* path, int flags, mode_t mode);
void multi_close_dir(DIR* dir);
DIR* multi_open_dir(const char* path);
int multi_read_dir(DIR* dir, struct dirent* entry, struct dirent** result);

#endif
