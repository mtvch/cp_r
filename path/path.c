#include "path.h"
#include <string.h>

void join(const char* path_1, const char* path_2, char *result) {
    strcpy(result, path_1);
    if (path_1[strlen(path_1) - 1] == '/') {
        strcat(result, path_2);
    }
    strcat(strcat(result, "/"), path_2);
}
