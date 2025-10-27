#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct Node {
    struct Node* next;
    struct Node* prev;
    void* data;
} Node;

typedef struct LinkedList {
    Node* head;
    Node* tail;
    unsigned int length;

    void (*addFirst)(struct LinkedList*, void*);
    void (*addLast)(struct LinkedList*, void*);
    void* (*removeFirst)(struct LinkedList*);
    void* (*removeLast)(struct LinkedList*);

    void* (*removeNode)(struct LinkedList*, Node*);

    void (*free)(struct LinkedList*);
} LinkedList;

LinkedList* new_LinkedList();

#endif