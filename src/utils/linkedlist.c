#include "linkedlist.h"
#include <stdlib.h>

static Node* new_Node(Node* prev, Node* next, void* data){
    Node* node = malloc(sizeof(Node));
    node->next = next;
    node->prev = prev;
    node->data = data;
    return node;
}

static void clear(LinkedList* this){
    Node* cur = this->head;
    Node* temp;
    while(cur != NULL){
        temp = cur;
        cur = cur->next;
        free(temp->data);
        free(temp);
    }
    this->head = this->tail = NULL;
    this->length = 0;
}

static void _free(LinkedList* this){
    Node* cur = this->head;
    Node* temp;
    while(cur != NULL){
        temp = cur;
        cur = cur->next;
        free(temp->data);
        free(temp);
    }
    free(this);
}

static void addFirst(LinkedList* this, void* data){
    Node* node = new_Node(NULL, this->head, data);
    if (this->head) this->head->prev = node;
    else this->tail = node;
    this->head = node;
    this->length++;
}

static void addLast(LinkedList* this, void* data){
    Node* node = new_Node(this->tail, NULL, data);
    if (this->tail) this->tail->next = node;
    else this->head = node;
    this->tail = node;
    this->length++;
}

static void* removeFirst(LinkedList* this){
    if (this->length == 0) return NULL;
    Node* first = this->head;
    this->head = first->next;
    if (this->head == NULL) this->tail = NULL;
    else this->head->prev = NULL;
    this->length--;
    void* data = first->data;
    free(first);
    return data;
}

static void* removeLast(LinkedList* this){
    if (this->length == 0) return NULL;
    Node* last = this->tail;
    this->tail = last->prev;
    if (this->tail == NULL) this->head = NULL;
    else this->tail->next = NULL;
    this->length--;
    void* data = last->data;
    free(last);
    return data;
}

LinkedList* new_LinkedList(){
    LinkedList* this = malloc(sizeof(LinkedList));
    this->head = this->tail = NULL;
    this->length = 0;

    this->clear = clear;
    this->free = _free;
    this->addFirst = addFirst;
    this->addLast = addLast;
    this->removeFirst = removeFirst;
    this->removeLast = removeLast;
    return this;
}