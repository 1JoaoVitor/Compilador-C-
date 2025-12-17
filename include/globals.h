#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>

extern FILE* source; 
extern int linha_atual;

#define MAXCHILDREN 3

typedef enum {StmtK, ExpK, DecK} NodeKind; //definição mais geral
typedef enum {IfK, WhileK, ReturnK, CompoundK} StmtKind; // palavras reservadas, if, while, return e conteúdo entre {}, o bloco (Compound)
typedef enum {OpK, ConstK, IdK, CallK, TypeK} ExpKind; // expressões com algum resultado/cálculo (TypeK usado para nós de tipo int/void)
typedef enum {Void, Integer, Boolean} ExpType;
typedef enum {VarK, FunK, ParamK} DecKind; // declarações, variavel, funcao, paramentros 


typedef struct treeNode {
    struct treeNode * child[MAXCHILDREN]; 
    struct treeNode * sibling;
    
    int line_num;
    
    NodeKind nodekind; 

    // Uso de union como sugerido pelo professor
    union { 
        StmtKind stmt; 
        ExpKind exp; 
        DecKind dec;
    } kind;   
    
    union {
        int op;         
        int val;        
        char * name;    
    } attr;
    
    ExpType type; /* Para verificação de tipo (int/void) */

} TreeNode;

#endif
