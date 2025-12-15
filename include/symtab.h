#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <stdio.h>
#include "globals.h"

/* * st_insert: Insere um identificador na tabela.
 * - name: nome da variável/função
 * - lineno: linha onde apareceu
 * - loc: endereço de memória  (usado na geração do código)
 * - type: tipo do dado (Integer, Void)
 */
void st_insert(char * name, int lineno, int loc, ExpType type);

/* * st_lookup: Busca um identificador.
 * - Retorna o endereço de memória (loc) se encontrar, ou -1 se não existir.
 */
int st_lookup(char * name);

ExpType st_lookup_type(char * name);

/* Imprime a tabela formatada com Tipos */
void printSymTab(FILE * listing);

#endif