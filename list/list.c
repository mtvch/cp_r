#include "list.h"
#include <stdlib.h>

void free_list(List *list) {
    if (!list) {
        return;
    }
    List* next = list->tl;
    if (list->hd) {
        free(list->hd);
    }
    free(list);
    free_list(next);
}

List* create_list() {
    List *list = malloc(sizeof(List));
    if (!list) {
        return NULL;
    }
    list->hd = NULL;
    list->tl = NULL;
    return list;
}

void cons(List* a, List *b) {
    if (a->tl == NULL) {
        a->tl = b;
        return;
    }
    cons(a->tl, b);
}
