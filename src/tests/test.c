#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "linkedlist.h"
#include "math.h"

int main() {
    // Criando os chunks
    int rows = 10000;
    int cols = 10000;
    int chunkSide = 30;
    int chunkArea = chunkSide * chunkSide;
    HashTable* t = new_HashTable((rows * cols) / chunkArea);

    int chunkRows = ceil(rows / (float)chunkSide);
    int chunkCols = ceil(cols / (float)chunkSide);
    for (int i = 0; i < chunkRows; i++){
        for (int j = 0; j < chunkCols; j++){
            LinkedList* list = new_LinkedList();
            char key[100];
            sprintf(key, "%d,%d", i, j);
            t->set(t, key, list);
        }
    }

    // Pegando enimigos em áreas vizinhas
    float x = 556.7;
    float y = 778.1;

    int xCord = x;
    int yCord = y;

    // Cordenadas do grupo
    int chunkX = x / chunkSide;
    int chunkY = y / chunkSide;

    // Pegando chunks adjacentes
    for (int i = -1; i <= 1; i++){
        for (int j = -1; j <= 1; j++){
            char key[100];
            sprintf(key, "%d,%d", chunkY + i, chunkX + j);
            LinkedList* list = t->get(t, key);
            
            Node* cur = list->head;
            while(cur != NULL){
                // Atualizar inimigo
                cur = cur->next;
            }
        }
    }
    
    return 0;
}