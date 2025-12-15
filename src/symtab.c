#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/symtab.h"

// número primo ajuda na distribuição
#define SIZE 211

/* Potência de 2 para o deslocamento (shift) na função hash */
#define SHIFT 4

/* Função Hash: Transforma o nome (string) em um índice numérico */
static int hash(char * key) {
    int temp = 0;
    int i = 0;
    while (key[i] != '\0') {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

/* Estrutura da Lista de Linhas (para saber todas as linhas onde a variável aparece) */
typedef struct LineListRec { 
    int lineno;
    struct LineListRec * next;
} * LineList;

/* Estrutura do "Bucket" (o registro da variável na tabela) */
typedef struct BucketListRec { 
    char * name;
    LineList lines;
    int memloc; /* Localização na memória */
    ExpType type;
    struct BucketListRec * next;
} * BucketList;

/* A Tabela Hash em si */
static BucketList hashTable[SIZE];

/* Insere na tabela */
void st_insert(char * name, int lineno, int loc, ExpType type) {
    int h = hash(name);
    BucketList l = hashTable[h];
    
    /* Procura se já existe */
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    
    /* Se não achou, cria um novo registro */
    if (l == NULL) {
        l = (BucketList) malloc(sizeof(struct BucketListRec));
        l->name = name;
        l->lines = (LineList) malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->memloc = loc;
        l->type = type;
        l->lines->next = NULL;
        l->next = hashTable[h];
        hashTable[h] = l; /* Insere no início da lista (LIFO) */
    } 
    else { /* Se já existe, apenas adiciona a nova linha na lista de referências */
        LineList t = l->lines;
        while (t->next != NULL) t = t->next;
        t->next = (LineList) malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
}

/* Busca na tabela */
int st_lookup(char * name) {
    int h = hash(name);
    BucketList l = hashTable[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    if (l == NULL) return -1;
    else return l->memloc;
}

/* Retorna o tipo da variável/função */
ExpType st_lookup_type(char * name) {
    int h = hash(name);
    BucketList l = hashTable[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    if (l == NULL) return Void; /* Se não achar, assume Void (ou erro) */
    else return l->type;
}

/* Imprime a tabela (Requisito do PDF) */
void printSymTab(FILE * listing) {
    int i;
    fprintf(listing, "Escopo/Nome    Tipo    Loc   Numeros de Linha\n");
    fprintf(listing, "-------------  ------  ----  ----------------\n");
    
    for (i = 0; i < SIZE; ++i) {
        if (hashTable[i] != NULL) {
            BucketList l = hashTable[i];
            while (l != NULL) {
                fprintf(listing, "%-14s ", l->name);

                switch(l->type) {
                    case Integer: fprintf(listing, "%-7s ", "int"); break;
                    case Void:    fprintf(listing, "%-7s ", "void"); break;
                    case Boolean: fprintf(listing, "%-7s ", "bool"); break;
                    default:      fprintf(listing, "%-7s ", "unknown"); break;
                }

                fprintf(listing, "%-5d  ", l->memloc);
                
                LineList t = l->lines;
                while (t != NULL) {
                    fprintf(listing, "%4d ", t->lineno);
                    t = t->next;
                }
                fprintf(listing, "\n");
                l = l->next;
            }
        }
    }
}