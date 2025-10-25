#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#define ARRAYLIST_START_CAPACITY 64

#include "utils.h"

typedef struct ArrayList {
    uInt64 length;
    uInt64 capacity;
    void** data; 
    
    void (*push)(struct ArrayList*, void*);
    void* (*pop)(struct ArrayList*);
    Int64 (*indexOf)(struct ArrayList*, const void*);
    void (*sort)(struct ArrayList*, int(*cmp)(const void*, const void*));
    Int64 (*binarySearch)(struct ArrayList*, const void*, int(*cmp)(const void*, const void*));

    void (*free)(struct ArrayList*);
} ArrayList;

ArrayList* new_ArrayList();

#endif