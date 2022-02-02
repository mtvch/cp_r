#include "file.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#define BUFF_SIZE 512

int cp_file(const char* path_from, const char* path_to) {
    struct stat from_file_stats;
    if (stat(path_from, &from_file_stats) < 0) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error reading stats in 'cp_file'");
        return -1;
    }

    int from_file_mode =  from_file_stats.st_mode & 0777;

    int fd_from = multi_open_file(path_from, O_RDONLY, 0);
    if (fd_from < 0) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error opening file");
        return -1;
    }

    int fd_to = multi_open_file(path_to, O_RDWR | O_CREAT, from_file_mode);
    if (fd_to < 0) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error opening file");
        multi_close_file(fd_from);
        return -1;
    }


    char buff[BUFF_SIZE];
    int read_return;
    while ((read_return = read(fd_from, buff, BUFF_SIZE)) > 0) {
        if (write(fd_to, buff, read_return) < 0) {
            fprintf(stderr, "Can't copy %s\n", path_from);
            perror("Error writing to file");
            multi_close_file(fd_from);
            multi_close_file(fd_to);
            remove(path_to);
            return -1;
        }
    }

    if (read_return < 0) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error reading from file");
        remove(path_to);
    }

    multi_close_file(fd_from);
    multi_close_file(fd_to);

    return read_return;
}
