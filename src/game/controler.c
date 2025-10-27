#include "controler.h"

#include <stdlib.h>

static void reset(Controler* this) {
    Node* cur = this->inputBuffer->head;
    Node* temp;
    while (cur != NULL) {
        temp = cur;
        cur = cur->next;
        free(temp->data);
        free(temp);
    }
    this->inputBuffer->head = this->inputBuffer->tail = NULL;
    this->inputBuffer->length = 0;
}

static void addInput(Controler* this, int dx, int dy) {
    LinkedList* inputBuffer = this->inputBuffer;
    Input* last = inputBuffer->head ? inputBuffer->head->data : NULL;
    if (last == NULL || (last->dx != dx || last->dy != dy)) {
        Input* inp = malloc(sizeof(Input));
        inp->dx = dx;
        inp->dy = dy;
        inputBuffer->addFirst(inputBuffer, inp);
    }
}

static void getInputs(Controler* this) {
    if (IsKeyDown(KEY_W)){
        addInput(this, 0, -1);
    }
    if (IsKeyDown(KEY_S)){
        addInput(this, 0, 1);
    }
    if (IsKeyDown(KEY_A)){
        addInput(this, -1, 0);
    }
    else if (IsKeyDown(KEY_D)){
        addInput(this, 1, 0);
    }
}

static void _free(Controler* this) {
    reset(this);
    free(this);
}

Controler* new_Controler() {
    Controler* this = malloc(sizeof(Controler));
    this->inputBuffer = new_LinkedList();

    this->getInputs = getInputs;
    this->reset = reset;
    this->free = _free;
    return this;
}