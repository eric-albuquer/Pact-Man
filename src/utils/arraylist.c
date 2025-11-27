#include "arraylist.h"
#include <stdlib.h>
#include <string.h>

//===============================================================
//  CRESCER CAPACIDADE DO ARRAY
//===============================================================

static inline void growth(ArrayList* this) {
    this->capacity += this->capacity >> 1;
    this->data = realloc(this->data, this->capacity * sizeof(void*));
}

//===============================================================
//  ADICIONAR ELEMENTO NO FIM DO ARRAY
//===============================================================

static inline void push(ArrayList* this, void* obj) {
    if (this->length == this->capacity)
        growth(this);
    this->data[this->length++] = obj;
}

//===============================================================
//  REMOVER ELEMENTO DO FIM DO ARRAY
//===============================================================

static inline void* pop(ArrayList* this) {
    if (this->length == 0) return NULL;
    return this->data[--this->length];
}

//===============================================================
//  PEGAR ÍNDICE DO ELEMENTO
//===============================================================

static inline int indexOf(ArrayList* this, const void* data) {
    for (int i = 0; i < this->length; i++) {
        if (this->data[i] == data) return i;
    }
    return -1;
}

//===============================================================
//  TROCA DE ELEMENTOS DO ARRAY
//===============================================================

static inline void swap(void** a, void** b) {
    void* temp = *a;
    *a = *b;
    *b = temp;
}

//===============================================================
//  REMOVER ELEMENTO POR ÍNDICE MANTENDO A ORDEM
//===============================================================

static inline void* removeIdx(ArrayList* this, unsigned int idx) {
    if (idx >= this->length) return NULL;
    for (unsigned int i = idx; i < this->length - 1; i++) {
        swap(&this->data[i], &this->data[i + 1]);
    }
    this->length--;
    return this->data[this->length];
}

//===============================================================
//  FUNÇÃO DE INTERCALAR PARA ORDENAR
//===============================================================

static void merge(
    void** restrict arr,
    void** restrict aux,
    unsigned int start,
    unsigned int mid,
    unsigned int end,
    int (*cmp)(const void*, const void*)
) {
    void** left = arr + start;
    void** right = arr + mid;

    void** leftEnd = arr + mid;
    void** rightEnd = arr + end;

    void** out = aux + start;

    void* lv = *left;
    void* rv = *right;

    while (left < leftEnd && right < rightEnd) {
        if (cmp(lv, rv) < 0) {
            *out++ = lv;
            left++;
            if (left < leftEnd) lv = *left;
        } else {
            *out++ = rv;
            right++;
            if (right < rightEnd) rv = *right;
        }
    }

    memcpy(out, left, (leftEnd - left) * sizeof(void*));
    memcpy(out, right, (rightEnd - right) * sizeof(void*));

    memcpy(arr + start, aux + start, (end - start) * sizeof(void*));
}

//===============================================================
//  FUNÇÃO DE ORDENAÇÃO ESTÁVEL
//===============================================================

static void mergeSort(ArrayList* this, int (*cmp)(const void*, const void*)) {
    const unsigned int length = this->length;
    void** aux = malloc(sizeof(void*) * length);
    void** arr = this->data;
    for (unsigned int size = 1; size < length; size <<= 1) {
        const unsigned int step = size << 1;
        for (unsigned int start = 0; start < length; start += step) {
            unsigned int mid = start + size;
            unsigned int end = start + step;

            if (mid > length) mid = length;
            if (end > length) end = length;
            merge(arr, aux, start, mid, end, cmp);
        }
    }
    free(aux);
}

//===============================================================
//  FUNÇÃO DE ORDENAÇÃO NÃO ESTÁVEL MENOS MEMÓRIA
//===============================================================

static void quickSortRecursion(ArrayList* this, int start, int end, int (*cmp)(const void*, const void*)) {
    if (start >= end) return;
    swap(&this->data[(start + end) >> 1], &this->data[end]);
    void* pivot = this->data[end];
    int idx = start;

    for (int i = start; i < end; i++) {
        if (cmp(this->data[i], pivot) < 0) {
            swap(&this->data[i], &this->data[idx++]);
        }
    }
    swap(&this->data[end], &this->data[idx]);

    quickSortRecursion(this, start, idx - 1, cmp);
    quickSortRecursion(this, idx + 1, end, cmp);
}

static inline void quickSort(ArrayList* this, int (*cmp)(const void*, const void*)){
    quickSortRecursion(this, 0, this->length - 1, cmp);
}

//===============================================================
//  BISCA BINÁRIA
//===============================================================

static int bSearch(ArrayList* this, int start, int end, const void* data, int (*cmp)(const void*, const void*)) {
    if (start > end) return -1LL;
    int mid = (start + end) >> 1;
    void* midData = this->data[mid];
    int comp = cmp(midData, data);
    if (comp == 0) return mid;
    else if (comp > 0) return bSearch(this, start, mid, data, cmp);
    else if (comp < 0) return bSearch(this, mid + 1, end, data, cmp);
    return -1LL;
}

static inline int binarySearch(ArrayList* this, const void* data, int (*cmp)(const void*, const void*)) {
    return bSearch(this, 0, this->length - 1, data, cmp);
}

//===============================================================
//  LIMPAR O ARRAY
//===============================================================

static inline void clear(ArrayList* this) {
    this->length = 0;
}

//===============================================================
//  LIBERAR MEMÓRIA
//===============================================================

static inline void _free(ArrayList* this) {
    free(this);
}

//===============================================================
//  CONSTRUTOR
//===============================================================

ArrayList* new_ArrayList() {
    ArrayList* this = malloc(sizeof(ArrayList));
    this->length = 0;
    this->capacity = ARRAYLIST_START_CAPACITY;
    this->data = malloc(this->capacity * sizeof(void*));

    this->push = push;
    this->pop = pop;
    this->removeIdx = removeIdx;
    this->quickSort = quickSort;
    this->mergeSort = mergeSort;
    this->binarySearch = binarySearch;
    this->indexOf = indexOf;

    this->clear = clear;
    this->free = _free;
    return this;
}
