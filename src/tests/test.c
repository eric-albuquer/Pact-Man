#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "linkedlist.h"
#include "math.h"

typedef struct Input {
    int dx;
    int dy;
} Input;

int main() {
    LinkedList* list = new_LinkedList();
    for (int i = 0; i < 10; i++){
        Input* inp = malloc(sizeof(Input));
        inp->dx = i;
        inp->dy = -i;
        list->addFirst(list, inp);
    }

    while(list->length > 0){
        Input* inp = list->removeLast(list);
        printf("%d, %d\n", inp->dx, inp->dy);
        free(inp);
    }
    
    return 0;
}