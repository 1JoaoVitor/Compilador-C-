#ifndef _ANALYZE_H_
#define _ANALYZE_H_

#include "globals.h"

/* Passo 1: Constrói a tabela de símbolos */
void buildSymtab(TreeNode *);

/* Passo 2: Verifica os tipos (int/void) */
void typeCheck(TreeNode *);

#endif