#include "arraylist.h"
#include <stdlib.h>

static void growth(ArrayList* this) {
    this->capacity += this->capacity >> 1;
    this->data = realloc(this->data, this->capacity * sizeof(void*));
}

static void push(ArrayList* this, void* obj) {
    if (this->length == this->capacity)
        growth(this);
    this->data[this->length++] = obj;
}

static void* pop(ArrayList* this){
    if (this->length == 0) return NULL;
    return this->data[--this->length];
}

static int indexOf(ArrayList* this, const void* data){
    for (int i = 0; i < this->length; i++){
        if (this->data[i] == data) return i;
    }
    return -1;
}

static void swap(void** a, void** b){
    void* temp = *a;
    *a = *b;
    *b = temp;
}

static void quickSort(ArrayList* this, int start, int end, int (*cmp)(const void*, const void*)){
    if (start >= end) return;
    swap(&this->data[(start + end) >> 1], &this->data[end]);
    void* pivot = this->data[end];
    int idx = start;

    for (int i = start; i < end; i++){
        if (cmp(this->data[i], pivot) < 0){
            swap(&this->data[i], &this->data[idx++]);
        }
    }
    swap(&this->data[end], &this->data[idx]);
    
    quickSort(this, start, idx - 1, cmp);
    quickSort(this, idx + 1, end, cmp);
}

static void sort(ArrayList* this, int (*cmp)(const void*, const void*)){
    quickSort(this, 0, this->length - 1, cmp);
}

static int bSearch(ArrayList* this, int start, int end, const void* data, int (*cmp)(const void*, const void*)){
    if (start > end) return -1LL;
    int mid = (start + end) >> 1;
    void* midData = this->data[mid];
    int comp = cmp(midData, data);
    if (comp == 0) return mid;
    else if (comp > 0) return bSearch(this, start, mid, data, cmp);
    else if (comp < 0) return bSearch(this, mid + 1, end, data, cmp);
    return -1LL;
}

static int binarySearch(ArrayList* this, const void* data, int (*cmp)(const void*, const void*)){
    return bSearch(this, 0, this->length - 1, data, cmp);
}

static void _free(ArrayList* this){
    free(this);
}

ArrayList* new_ArrayList() {
    ArrayList* arr = malloc(sizeof(ArrayList));
    arr->length = 0;
    arr->capacity = ARRAYLIST_START_CAPACITY;
    arr->data = malloc(arr->capacity * sizeof(void*));
    
    arr->push = push;
    arr->pop = pop; 
    arr->sort = sort;
    arr->binarySearch = binarySearch;
    arr->indexOf = indexOf;

    arr->free = _free;
    return arr;
}
