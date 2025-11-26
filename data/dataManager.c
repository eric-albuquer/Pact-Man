#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "arraylist.h"
#include <string.h>

//===============================================================
//  TIPOS DE OPERAÇÕES
//===============================================================

typedef enum {
    OPERATION_DELETE,
    OPERATION_INSERT,

    OPERATION_EXIT,

    OPERATION_COUNT,
} Operations;

//===============================================================
//  DADOS NA MEMÓRIA RAM
//===============================================================

static ArrayList* scores = NULL;

//===============================================================
//  FUNÇÕES AUXILIARES
//===============================================================

static void toUpperCase(char* str){
    while(*str){
        if (*str >= 'a' && *str <= 'z'){
            *str -= 'a' - 'A';
        }
        str++;
    }
}

static int cmpTime(const void* a, const void* b) {
    const Score* sa = a;
    const Score* sb = b;

    if (sa->totalTime < sb->totalTime) return -1;
    if (sa->totalTime > sb->totalTime) return 1;
    return 0;
}

//===============================================================
//  FUNÇÕES DO CRUD
//===============================================================

static void delete() {
    unsigned int idx;
    printf("Escolha um indice para remover: ");
    scanf("%u", &idx);
    Score* removedScore = scores->removeIdx(scores, idx);
    free(removedScore);
}

static void insert() {
    Score* score = malloc(sizeof(Score));

    printf("Nome: ");
    scanf("%s", score->name);
    toUpperCase(score->name);
    printf("Tempo: ");
    scanf("%f", &score->totalTime);
    printf("Moedas: ");
    scanf("%d", &score->totalCoins);
    printf("Fragmentos: ");
    scanf("%d", &score->totalFragments);

    scores->push(scores, score);
}

static void view(){
    if (scores->length == 0) {
        printf("Nenhum dado cadastrado!\n");
        return;
    }

    for (int i = 0; i < scores->length; i++){
        Score* score = scores->data[i];
        int totalSeconds = score->totalTime;
        printf("%d, Nome: %s, Tempo: %02d:%02d, Moedas: %d, Fragmentos: %d\n", i, score->name, totalSeconds / 60, totalSeconds % 60, score->totalCoins, score->totalFragments);
    }
}

//===============================================================
//  CARREGAR DADOS PARA A RAM
//===============================================================

static void loadData(const char* pathDatabase) {
    FILE* file = fopen(pathDatabase, "rb");
    if (file == NULL) return;
    scores = new_ArrayList();

    while (1) {
        Score* score = malloc(sizeof(Score));
        if (fread(score, sizeof(Score), 1, file) == 1) {
            scores->push(scores, score);
        } else {
            free(score);
            break;
        }
    }
    fclose(file);
}

//===============================================================
//  SALVAR DADOS E LIBERAR MEMÓRIA
//===============================================================

static void saveData(const char* pathDatabase) {
    if (scores->length == 0) return;

    scores->sort(scores, cmpTime);

    FILE* file = fopen(pathDatabase, "wb");
    for (int i = 0; i < scores->length; i++) {
        Score* score = scores->data[i];
        fwrite(score, sizeof(Score), 1, file);
        free(score);
    }
    fclose(file);
    scores->free(scores);
}

//===============================================================
//  FLUXO PRINCIPAL DO PROGRAMA
//===============================================================

int main() {
    const char* pathDatabase = "data/scores.bin";
    loadData(pathDatabase);
    unsigned int operation;
    
    do {
        view();
        printf("\nEscolha a operacao: (%d - Excluir), (%d - Incluir), (%d - Sair e salvar)\nOperacao: ", OPERATION_DELETE, OPERATION_INSERT, OPERATION_EXIT);
        scanf("%u", &operation);
        if (operation == OPERATION_DELETE) delete();
        else if (operation == OPERATION_INSERT) insert();
        else if (operation == OPERATION_EXIT) break;
        else printf("Operacao invalida!");
    } while (operation < OPERATION_COUNT);

    saveData(pathDatabase);
    return 0;
}