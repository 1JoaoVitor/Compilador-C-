#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/symtab.h"
#include <string.h>

#define SIZE 211
#define SHIFT 4

static int hash(char * key) {
    int temp = 0;
    int i = 0;
    while (key[i] != '\0') {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

typedef struct LineListRec { 
    int lineno;
    struct LineListRec * next;
} * LineList;

typedef struct BucketListRec { 
    char * name;   /* Chave única (ex: main$x) usada na busca */
    char * id;     /* Nome limpo para exibição (ex: x) */
    char * scope;  /* Escopo para exibição (ex: main) */
    char * type;   /* Tipo para exibição (ex: int) */
    LineList lines;
    int memloc; 
    struct BucketListRec * next;
} * BucketList;

static BucketList hashTable[SIZE];

/* Helper para extrair o nome limpo de "main$x" -> "x" */
char * cleanName(char * mangled) {
    char * dollar = strchr(mangled, '$');
    if (dollar) return dollar + 1; /* Retorna o que vem depois do $ */
    return mangled; /* Se não tem $, retorna o próprio nome */
}

ExpType st_lookup_type(char * name) {
    int h = hash(name);
    BucketList l = hashTable[h];
    
    /* Procura na lista encadeada */
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    
    /* Se não achou, retorna Void (padrão de segurança) */
    if (l == NULL) return Void;
    
    /* Converte a string salva para o Enum */
    if (strcmp(l->type, "int") == 0) return Integer;
    if (strcmp(l->type, "void") == 0) return Void;
    
    return Void;
}

void st_insert(char * name, int lineno, int loc, char * scope, char * typeID) {
    int h = hash(name);
    BucketList l = hashTable[h];
    
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    
    if (l == NULL) { /* Nova variável */
        l = (BucketList) malloc(sizeof(struct BucketListRec));
        l->name = name;
        /* Copiamos as strings para garantir persistência */
        l->lines = (LineList) malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->memloc = loc;
        l->lines->next = NULL;
        
        /* Novos campos visuais */
        l->scope = strdup(scope);
        l->type = strdup(typeID);
        l->id = strdup(cleanName(name)); 

        l->next = hashTable[h];
        hashTable[h] = l;
    } 
    else { /* Já existe, apenas adiciona linha */
        LineList t = l->lines;
        while (t->next != NULL) t = t->next;
        t->next = (LineList) malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
}

int st_lookup(char * name) {
    int h = hash(name);
    BucketList l = hashTable[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    if (l == NULL) return -1;
    else return l->memloc;
}

void printSymTab(FILE * listing) {
    int i;
    /* Cabeçalho formatado com as novas colunas */
    fprintf(listing, "Nome           Escopo         Tipo    Loc   Linhas\n");
    fprintf(listing, "-------------  -------------  ------  ----  ------\n");
    
    for (i = 0; i < SIZE; ++i) {
        if (hashTable[i] != NULL) {
            BucketList l = hashTable[i];
            while (l != NULL) {
                /* Imprime: ID Limpo, Escopo, Tipo, Loc */
                fprintf(listing, "%-14s %-14s %-7s %-5d ", l->id, l->scope, l->type, l->memloc);
                
                LineList t = l->lines;
                while (t != NULL) {
                    fprintf(listing, "%d ", t->lineno);
                    t = t->next;
                }
                fprintf(listing, "\n");
                l = l->next;
            }
        }
    }
}