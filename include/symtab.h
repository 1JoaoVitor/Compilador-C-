#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <stdio.h>
#include "globals.h" 

void st_insert(char * name, int line_num, int loc, char * scope, char * typeID);

int st_lookup(char * name);

ExpType st_lookup_type(char * name);

void printSymTab(FILE * listing);

#endif