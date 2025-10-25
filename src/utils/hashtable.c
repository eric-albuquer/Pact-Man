#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

#define FNV64_OFFSET 14695981039346656037ull
#define FNV64_PRIME 1099511628211ull

static HashNode* new_Node(HashNode* prev, HashNode* next, HashNode* prevKey,
                          HashNode* nextKey, char* key, void* data) {
    HashNode* node = malloc(sizeof(HashNode));
    node->next = next;
    node->prev = prev;
    node->nextKey = nextKey;
    node->prevKey = prevKey;
    strcpy(node->key, key);
    node->data = data;
    return node;
}

static Bucket* new_Bucket() { return calloc(1, sizeof(Bucket)); }

static HashNode* hasBucket(Bucket* b, char* key) {
    HashNode* cur = b->head;
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

static uInt64 fnv1a(const char* str) {
    uInt64 hash = FNV64_OFFSET;
    unsigned char c;

    while ((c = (unsigned char)*str++)) {
        hash ^= c;
        hash *= FNV64_PRIME;
    }

    return hash;
}

static void set(HashTable* this, char* key, void* data) {
    uInt64 idx = fnv1a(key) % this->length;
    Bucket* b = this->buckets[idx];
    Bucket* keys = this->keys;
    HashNode* node = hasBucket(b, key);
    if (node != NULL) {
        free(node->data);
        node->data = data;
        return;
    }
    node = new_Node(b->tail, NULL, keys->tail, NULL, key, data);
    if (b->tail == NULL)
        b->head = node;
    else
        b->tail->next = node;
    b->tail = node;

    if (keys->tail == NULL)
        keys->head = node;
    else
        keys->tail->nextKey = node;
    keys->tail = node;
}

static void* get(HashTable* this, char* key) {
    uInt64 idx = fnv1a(key) % this->length;
    Bucket* b = this->buckets[idx];
    HashNode* node = hasBucket(b, key);
    return node != NULL ? node->data : NULL;
}

bool has(HashTable* this, char* key) {
    uInt64 idx = fnv1a(key) % this->length;
    Bucket* b = this->buckets[idx];
    return hasBucket(b, key) != NULL;
}

static void* delete(HashTable* this, char* key) {
    uInt64 idx = fnv1a(key) % this->length;
    Bucket* b = this->buckets[idx];
    Bucket* keys = this->keys;
    HashNode* node = hasBucket(b, key);
    if (node == NULL) return NULL;

    if (node->prev)
        node->prev->next = node->next;
    else
        b->head = node->next;
    if (node->next)
        node->next->prev = node->prev;
    else
        b->tail = node->prev;

    if (node->prevKey)
        node->prevKey->nextKey = node->nextKey;
    else
        keys->head = node->nextKey;
    if (node->nextKey)
        node->nextKey->prevKey = node->prevKey;
    else
        keys->tail = node->prevKey;

    void* data = node->data;
    free(node);
    return data;
}

static void _free(HashTable* this) {
    HashNode* cur = this->keys->head;
    HashNode* temp;
    while (cur != NULL) {
        temp = cur;
        cur = cur->nextKey;
        free(temp);
    }
    for (uInt64 i = 0; i < this->length; i++) {
        free(this->buckets[i]);
    }
    free(this->buckets);
    free(this->keys);
    free(this);
}

HashTable* new_HashTable(uInt64 length) {
    HashTable* h = malloc(sizeof(HashTable));
    h->length = length;
    h->keys = new_Bucket();
    h->buckets = malloc(sizeof(Bucket) * length);
    for (uInt64 i = 0; i < length; i++) {
        h->buckets[i] = new_Bucket();
    }

    h->set = set;
    h->get = get;
    h->has = has;
    h->delete = delete;
    
    h->free = _free;
    return h;
}