#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <stdio.h>
#include "globals.h" /* Necessário para reconhecer ExpType (Integer/Void) */

/* Insere na tabela */
void st_insert(char * name, int lineno, int loc, char * scope, char * typeID);

/* Busca endereço de memória (retorna -1 se não achar) */
int st_lookup(char * name);

/* NOVA FUNÇÃO: Busca o tipo da variável (Integer/Void) */
ExpType st_lookup_type(char * name);

void printSymTab(FILE * listing);

#endif