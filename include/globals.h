#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>

extern FILE* source; 
extern int linha_atual;

/* suficiente para IF e coisas assim */
#define MAXCHILDREN 3

/* tipos de nós */
typedef enum {StmtK, ExpK, DecK} NodeKind; //definição mais geral
typedef enum {IfK, WhileK, ReturnK, CompoundK} StmtKind; // palavras reservadas, if, while, return e conteúdo entre {}, o bloco (Compound)
typedef enum {OpK, ConstK, IdK, CallK, TypeK} ExpKind; // expressões com algum resultado/cálculo (TypeK usado para nós de tipo int/void)
typedef enum {VarK, FunK, ParamK} DecKind; // declarações, variavel, funcao, paramentros 

/* Tipos de variáveis para verificação de tipo (Semântico - Fase 3) */
typedef enum {Void, Integer, Boolean} ExpType;

/* A ESTRUTURA DO NÓ DA ÁRVORE */
typedef struct treeNode {
    struct treeNode * child[MAXCHILDREN]; /* Filhos (ex: condição do if) */
    struct treeNode * sibling;            /* Irmãos (ex: próxima linha de código) */
    
    int lineno;                           /* Linha onde apareceu (para erros) */
    
    NodeKind nodekind;                    /* É Stmt, Exp ou Declaração? */

    // Uso de union como sugerido pelo professor
    union { 
        StmtKind stmt; 
        ExpKind exp; 
        DecKind dec;
    } kind;                               /* Qual o subtipo exato? */
    
    /* Atributos do nó */
    union {
        int op;         /* Operador (token) se for OpK */
        int val;        /* Valor se for ConstK (número) */
        char * name;    /* Nome se for IdK ou FunK */
    } attr;
    
    ExpType type; /* Para verificação de tipo (int/void) */

} TreeNode;

#endif

//Exemplo

// int main(void) {      // Declaração de Função (FunK) 
//    int x;             // Declaração de Variável (VarK) 
//    x = 10;            // Expressão (x é IdK, 10 é ConstK, = é Atribuição) 
//    if (x > 0) {       // Statement (IfK) 
//       return x;       // Statement (ReturnK) 
//    }
// }