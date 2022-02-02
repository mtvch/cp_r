#ifndef LIST_H
#define LIST_H

#include <pthread.h>

struct List {
    void *hd;
    struct List *tl;
};

typedef struct List List;

void free_list(List *list);

void cons(List *a, List *b);

void reverse(List** list);

List* create_list();


#endif
