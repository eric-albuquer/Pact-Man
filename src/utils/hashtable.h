#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "utils.h"

typedef struct HashNode {
    struct HashNode* next;
    struct HashNode* prev;
    struct HashNode* nextKey;
    struct HashNode* prevKey;
    char key[100];
    void* data;
} HashNode;

typedef struct {
    HashNode* head;
    HashNode* tail;
} Bucket;

typedef struct HashTable {
    uInt64 length;
    Bucket* keys;
    Bucket** buckets;

    void (*set)(struct HashTable*, char*, void*);
    void* (*get)(struct HashTable*, char*);
    bool (*has)(struct HashTable*, char*);
    void (*delete)(struct HashTable*, char*);

    void (*clear)(struct HashTable*);
    void (*free)(struct HashTable*);
} HashTable;

HashTable* new_HashTable(uInt64 length);

#endif