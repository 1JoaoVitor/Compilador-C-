#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/symtab.h"
#include <string.h>
#include "../include/util.h"

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
    int line_num;
    struct LineListRec * next;
} * LineList;

typedef struct BucketListRec { 
    char * name;   
    char * id;   
    char * scope; 
    char * type;  
    LineList lines; 
    int memloc; 
    struct BucketListRec * next;
} * BucketList;

static BucketList hashTable[SIZE];

char * cleanName(char * mangled) {
    char * cifrao = strchr(mangled, '$');
    if (cifrao) return cifrao + 1; 
    return mangled;
}


void st_insert(char * name, int line_num, int loc, char * scope, char * typeID) {
    int h = hash(name);
    BucketList l = hashTable[h];
    
    while ((l != NULL) && (strcmp(name, l->name) != 0))
    l = l->next;
    
    if (l == NULL) { 
        l = (BucketList) malloc(sizeof(struct BucketListRec));
        l->name = name;
        l->lines = (LineList) malloc(sizeof(struct LineListRec));
        l->lines->line_num = line_num;
        l->memloc = loc;
        l->lines->next = NULL;
        
        l->scope = copyString(scope);
        l->type = copyString(typeID);
        l->id = copyString(cleanName(name)); 
        
        l->next = hashTable[h];
        hashTable[h] = l;
    } 
    else { 
        LineList t = l->lines;
        while (t->next != NULL) t = t->next;
        t->next = (LineList) malloc(sizeof(struct LineListRec));
        t->next->line_num = line_num;
        t->next->next = NULL;
    }
}

ExpType st_lookup_type(char * name) {
    int h = hash(name);
    BucketList l = hashTable[h]; 
    
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    if (l == NULL) return Void;
    
    if (strcmp(l->type, "int") == 0) return Integer;
    if (strcmp(l->type, "void") == 0) return Void;
    
    return Void;
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
    fprintf(listing, "Nome           Escopo         Tipo    Loc   Linhas\n");
    fprintf(listing, "-------------  -------------  ------  ----  ------\n");
    
    for (i = 0; i < SIZE; ++i) {
        if (hashTable[i] != NULL) {
            BucketList l = hashTable[i];
            while (l != NULL) {
                fprintf(listing, "%-14s %-14s %-7s %-5d ", l->id, l->scope, l->type, l->memloc);
                
                LineList t = l->lines;
                while (t != NULL) {
                    fprintf(listing, "%d ", t->line_num);
                    t = t->next;
                }
                fprintf(listing, "\n");
                l = l->next;
            }
        }
    }
}