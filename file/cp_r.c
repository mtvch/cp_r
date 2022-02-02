#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ftw.h>
#include <string.h>
#include "../list/list.h"
#include "../path/path.h"
#include "file.h"
#include <errno.h>


struct params {
    char* path_from;
    char* path_to;
};

void free_params_paths(List *params_list) {
    List* args_list = params_list;
    while (args_list != NULL && args_list->hd != NULL) {
        struct params* args = (struct params*)args_list->hd;
        free(args->path_from);
        free(args->path_to);
        args_list = args_list->tl;
    }
}

void* cp_r_multi(void* params) {
    struct params* params_struct = (struct params*)params;
    cp_r(params_struct->path_from, params_struct->path_to);
}

void* cp_file_multi(void* params) {
    struct params* params_struct = (struct params*)params;
    cp_file(params_struct->path_from, params_struct->path_to);
}

int create_thread(char* path_from, char* path_to, const char* name, List** threads_list, List** args_list, void* (*fn)(void*)) {
    pthread_t* th = malloc(sizeof(pthread_t));
    if (!th) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error allocating thread");
        return -1;
    }

    struct params* params = malloc(sizeof(struct params));
    if (!params) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error creating params");
        free(th);
        return -1;
    }
    params->path_from = path_from;
    params->path_to = path_to;

    List* args_head = create_list();
    if (!args_head) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error creating args_head");
        free(th);
        free(params);
        return -2;
    }
    args_head->hd = params;
    cons(args_head, *args_list);
    *args_list = args_head;

    List *threads_head = create_list();
    
    if (!threads_head) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error creating threads_head");
        free(th);
        return -1;
    }
    threads_head->hd = th;
    cons(threads_head, *threads_list);
    *threads_list = threads_head;

    if (pthread_create(th, NULL, fn, params) < 0) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error creating dir thread");
        return -1;
    }
    
    return 0;
}

int process_file(char* path_from, char* path_to, const char* name, List **threads_list, List **args_list) {
    struct stat stats;
    if (stat(path_from, &stats) < 0) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error reading stats in 'process_file'");
        return -1;
    }

    int code = 0;

    switch (stats.st_mode & S_IFMT) {
        case S_IFDIR:
            code = create_thread(path_from, path_to, name, threads_list, args_list, &cp_r_multi);
            break;
        case S_IFREG:
            code = create_thread(path_from, path_to, name, threads_list, args_list, &cp_file_multi);
            break;
        default:
            break;
    }

    return code;
}

int iter_over_dir_stream(DIR* dir_stream, const char* path_from, const char* path_to, List** threads_list, List** args_list) {
    int errno_on_start = errno;
    struct dirent* entry;
    while (entry = readdir(dir_stream)) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }
        char* new_path_from = malloc(sizeof(char) * (strlen(path_from) + strlen(entry->d_name) + 2));
        if (!new_path_from) {
            fprintf(stderr, "Can't copy %s from %s\n", entry->d_name, path_from);
            perror("Error allocating memory for `path`");
            return -1;
        }
        join(path_from, entry->d_name, new_path_from);

        if (!strcmp(path_to, new_path_from)) {
            free(new_path_from);
            continue;
        }

        char* new_path_to = malloc(sizeof(char) * (strlen(path_to) + strlen(entry->d_name) + 2));
        if (!path_to) {
            fprintf(stderr, "Can't copy %s from %s\n", entry->d_name, path_from);
            perror("Error allocating memory for 'path_to'");
            free(new_path_from);
            return -1;
        }
        join(path_to, entry->d_name, new_path_to);

        int code = process_file(new_path_from, new_path_to, entry->d_name, threads_list, args_list);

        if (code == -2) {
            free(new_path_from);
            free(new_path_to);
        }

        if (code < 0) {
            return -1;
        }
    }

    if (errno != errno_on_start) {
        fprintf(stderr, "Error copying contents of %s\n", path_from);
        perror("readdir error");
        return -1;
    }

    return 0;
}

int cp_r(const char* path_from, const char* path_to) {
     if (cp_dir(path_from, path_to) < 0) {
        return -1;
    }

    DIR* path_from_stream = multi_open_dir(path_from);
    if (!path_from_stream) {
        fprintf(stderr, "Can't copy contents of %s\n", path_from);
        perror("Error getting path_from_stream with multi_open_dir");
        return -1;
    }

    List *threads_list = create_list();
    if (!threads_list) {
        fprintf(stderr, "Can't copy contents of %s\n", path_from);
        perror("Error creating threads list");
        multi_close_dir(path_from_stream);
        return -1;
    }

    List *args_list = create_list();
    if (!args_list) {
        fprintf(stderr, "Can't copy contents of %s\n", path_from);
        perror("Error creating args list");
        free_list(threads_list);
        multi_close_dir(path_from_stream);
        return -1;
    }

    int code = iter_over_dir_stream(path_from_stream, path_from, path_to, &threads_list, &args_list);

    multi_close_dir(path_from_stream);

    List* list = threads_list;
    while (list != NULL && list->hd != NULL) {
        if (pthread_join(*((pthread_t*)list->hd), NULL) < 0) {
            perror("pthread_join error");
        }
        list = list->tl;
    }

    free_list(threads_list);
    free_params_paths(args_list);
    free_list(args_list);
    return code;
}
