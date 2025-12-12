#ifndef _UTIL_H_
#define _UTIL_H_

#include "globals.h"

TreeNode * newStmtNode(StmtKind);
TreeNode * newExpNode(ExpKind);
TreeNode * newDecNode(DecKind);

void printTree(TreeNode *);

/* Função do strdup que as vezes falta no windows */
char * copyString(char *);

#endif