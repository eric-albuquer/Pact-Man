#include "controler.h"
#include <stdlib.h>

static void reset(Controler* this){
    Node* cur = this->ioBuffer->head;
    Node* temp;
    while(cur != NULL){
        temp = cur;
        cur = cur->next;
        free(temp->data);
        free(temp);
    }
    this->ioBuffer->head = this->ioBuffer->tail = NULL;
    this->ioBuffer->length = 0;
}

static void getInputs(Controler* this) {
    Input *inp = calloc(1, sizeof(Input));
    LinkedList* ioBuffer = this->ioBuffer;
    if (IsKeyDown(KEY_W))
        inp->dy = -1;
    else if (IsKeyDown(KEY_S))
        inp->dy = 1;
    else if (IsKeyDown(KEY_A))
        inp->dx = -1;
    else if (IsKeyDown(KEY_D))
        inp->dx = 1;

    if (inp->dx != 0 || inp->dy != 0){
        ioBuffer->addFirst(ioBuffer, inp);
    }
        
}

static void _free(Controler* this) { 
    reset(this);
    free(this);
}

Controler* new_Controler() {
    Controler* this = malloc(sizeof(Controler));
    this->ioBuffer = new_LinkedList();
    
    this->getInputs = getInputs;
    this->reset = reset;
    this->free = _free;
    return this;
}