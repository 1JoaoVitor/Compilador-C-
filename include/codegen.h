#ifndef _CODGEN_H_
#define _CODGEN_H_

#include "globals.h"

/* Gera o código intermediário percorrendo a AST */
void codeGen(TreeNode * syntaxTree, char * codefile);

#endif