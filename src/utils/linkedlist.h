#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "utils.h"

typedef struct Node {
    struct Node* next;
    struct Node* prev;
    void* data;
} Node;

typedef struct LinkedList {
    Node* head;
    Node* tail;
    uInt64 length;

    void (*addFirst)(struct LinkedList*, void*);
    void (*addLast)(struct LinkedList*, void*);
    void* (*removeFirst)(struct LinkedList*);
    void* (*removeLast)(struct LinkedList*);

    void (*free)(struct LinkedList*);
} LinkedList;

LinkedList* new_LinkedList();

#endif