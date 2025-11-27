#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#define ARRAYLIST_START_CAPACITY 64

typedef struct ArrayList {
    unsigned int length;
    unsigned int capacity;
    void** data; 
    
    void (*push)(struct ArrayList*, void*);
    void* (*pop)(struct ArrayList*);

    void* (*removeIdx)(struct ArrayList*, unsigned int idx);
    int (*indexOf)(struct ArrayList*, const void*);
    void (*quickSort)(struct ArrayList*, int(*cmp)(const void*, const void*));
    void (*mergeSort)(struct ArrayList*, int(*cmp)(const void*, const void*));
    int (*binarySearch)(struct ArrayList*, const void*, int(*cmp)(const void*, const void*));

    void (*clear)(struct ArrayList*);
    void (*free)(struct ArrayList*);
} ArrayList;

ArrayList* new_ArrayList();

#endif