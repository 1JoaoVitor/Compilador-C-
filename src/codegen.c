#include "../include/globals.h"
#include "../include/symtab.h"
#include "../include/codegen.h"
#include "../src/parser.tab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static int tmpOffset = 0;
static int labelOffset = 0;

static char * newTemp() {
    static char buffer[50];
    sprintf(buffer, "t%d", ++tmpOffset);
    char * s = (char *) malloc(strlen(buffer) + 1);
    if (s == NULL) { printf("Erro de memoria\n"); exit(1); }
    strcpy(s, buffer);
    return s;
}


static char * newLabel() {
    static char buffer[50];
    sprintf(buffer, "L%d", ++labelOffset);
    char * s = (char *) malloc(strlen(buffer) + 1);
    if (s == NULL) { printf("Erro de memoria\n"); exit(1); }
    strcpy(s, buffer);
    return s;
}


static void emitCode(char * op, char * arg1, char * arg2, char * res) {
    
    if (strcmp(op, "ASSIGN") == 0) {
        printf("%s = %s\n", res, arg1);
    }
    else if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || 
             strcmp(op, "*") == 0 || strcmp(op, "/") == 0 ||
             strcmp(op, "<") == 0 || strcmp(op, "==") == 0 ||
             strcmp(op, ">") == 0 || strcmp(op, ">=") == 0 ||
             strcmp(op, "<=") == 0 || strcmp(op, "!=") == 0) {
        printf("%s = %s %s %s\n", res, arg1, op, arg2);
    }

    else if (strcmp(op, "LOAD") == 0) {
        printf("%s = %s[%s]\n", res, arg1, arg2);
    }
    else if (strcmp(op, "STORE") == 0) {  
        printf("%s[%s] = %s\n", arg1, arg2, res);
    }
    else if (strcmp(op, "GOTO") == 0) {
        printf("goto %s\n", arg1);
    }
    else if (strcmp(op, "IFF") == 0) {
        printf("if_false %s goto %s\n", arg1, arg2);
    }
    else if (strcmp(op, "LAB") == 0) {
        printf("%s:\n", arg1); 
    }

    else if (strcmp(op, "PARAM") == 0) {
        printf("param %s\n", arg1);
    }
    else if (strcmp(op, "CALL") == 0) {
        if (res != NULL)
            printf("%s = call %s, %s\n", res, arg1, arg2);
        else
            printf("call %s, %s\n", arg1, arg2);
    }
    else if (strcmp(op, "RET") == 0) {
        if (arg1 != NULL) printf("return %s\n", arg1);
        else printf("return\n");
    }
    else if (strcmp(op, "FUN") == 0) {
        printf("\nfunc %s\n", arg1);
    }
    else if (strcmp(op, "END") == 0) {
        printf("end %s\n", arg1);
    }
    else {
        printf("%s = %s %s %s\n", res, arg1, op, arg2);
    }
}


static char * cGen(TreeNode * tree) {
    char * result = NULL; 
    
    char * p1, * p2;
    char * label1, * label2;
    char * currentTemp;
    char * offsetTemp;

    if (tree == NULL) return NULL;

    switch (tree->nodekind) {
    
    case StmtK:
        switch (tree->kind.stmt) {
            case IfK:
                p1 = cGen(tree->child[0]); 
                label1 = newLabel();
                label2 = newLabel(); 
                
                emitCode("IFF", p1, label1, NULL); /* if_false cond goto L1 */
                
                cGen(tree->child[1]); 
                
                if (tree->child[2] != NULL) {
                    emitCode("GOTO", label2, NULL, NULL);
                    emitCode("LAB", label1, NULL, NULL);
                    cGen(tree->child[2]); /* Else */
                    emitCode("LAB", label2, NULL, NULL);
                } else {
                    emitCode("LAB", label1, NULL, NULL);
                }
                break;

            case WhileK:
                label1 = newLabel(); 
                label2 = newLabel(); 
                
                emitCode("LAB", label1, NULL, NULL);
                p1 = cGen(tree->child[0]); 
                emitCode("IFF", p1, label2, NULL);
                
                cGen(tree->child[1]); 
                emitCode("GOTO", label1, NULL, NULL);
                emitCode("LAB", label2, NULL, NULL);
                break;

            case ReturnK:
                p1 = cGen(tree->child[0]);
                emitCode("RET", p1, NULL, NULL);
                break;
                
            case CompoundK:
                cGen(tree->child[0]); 
                cGen(tree->child[1]); 
                break;
        }
        break;

    case ExpK:
        switch (tree->kind.exp) {
            case OpK:
                if (tree->attr.op == ATRIB) {
                    TreeNode * lhs = tree->child[0];
                    p2 = cGen(tree->child[1]); 

                    /* CASO 1: Atribuição em Vetor (v[i] = valor) */
                    if (lhs->nodekind == ExpK && lhs->kind.exp == IdK && lhs->child[0] != NULL) {
                        p1 = cGen(lhs->child[0]); /* Indice */
                        offsetTemp = newTemp();
                        emitCode("*", p1, "4", offsetTemp); /* Offset = i * 4 */
                        /* STORE: vec[offset] = val */
                        emitCode("STORE", lhs->attr.name, offsetTemp, p2);
                    } 
                    /* CASO 2: Atribuição Simples (x = valor) */
                    else {
                        emitCode("ASSIGN", p2, NULL, lhs->attr.name);
                    }
                    result = p2;
                }
                else {
                    /* OPERAÇÕES ARITMÉTICAS */
                    p1 = cGen(tree->child[0]);
                    p2 = cGen(tree->child[1]);
                    currentTemp = newTemp();
                    
                    char * op = "+";
                    switch(tree->attr.op) {
                        case SOMA: op = "+"; break;
                        case SUB: op = "-"; break;
                        case MULT: op = "*"; break;
                        case DIV: op = "/"; break;
                        case MENOR: op = "<"; break;
                        case IGUAL: op = "=="; break;
                        case MAIOR: op = ">"; break;
                        case MAIORIGUAL: op = ">="; break;
                        case MENORIGUAL: op = "<="; break;
                        case DIFERENTE: op = "!="; break;
                        default: op = "?"; break;
                    }
                    
                    emitCode(op, p1, p2, currentTemp);
                    result = currentTemp;
                }
                break;

            case ConstK:
                {
                    char * valStr = (char *) malloc(20);
                    if (valStr == NULL) { printf("Erro de memoria\n"); exit(1); }
                    sprintf(valStr, "%d", tree->attr.val);
                    result = valStr;
                }
                break;

            case IdK:
                if (tree->child[0] != NULL) {
                    p1 = cGen(tree->child[0]); 
                    offsetTemp = newTemp();
                    emitCode("*", p1, "4", offsetTemp);
                    
                    currentTemp = newTemp();
                    emitCode("LOAD", tree->attr.name, offsetTemp, currentTemp);
                    result = currentTemp;
                }
                else {
                    result = tree->attr.name;
                }
                break;

            case CallK:
                {
                    TreeNode * arg = tree->child[0];
                    int n = 0;
                    while (arg != NULL) {
                        char * t = cGen(arg);
                        emitCode("PARAM", t, NULL, NULL);
                        arg = arg->sibling;
                        n++;
                    }
                    
                    char nStr[20]; 
                    sprintf(nStr, "%d", n);

                    if (strcmp(tree->attr.name, "input") == 0) {
                        currentTemp = newTemp();
                        emitCode("CALL", "input", "0", currentTemp);
                        result = currentTemp;
                    } 
                    else if (strcmp(tree->attr.name, "output") == 0) {
                        emitCode("CALL", "output", "1", NULL);
                        result = NULL;
                    }
                    else {
                        currentTemp = newTemp();
                        emitCode("CALL", tree->attr.name, nStr, currentTemp);
                        result = currentTemp;
                    }
                }
                break;
            
            case TypeK:
                break;
        }
        break;
        
    case DecK:
        if (tree->kind.dec == FunK) {
            emitCode("FUN", tree->attr.name, NULL, NULL);
            cGen(tree->child[1]);
            emitCode("END", tree->attr.name, NULL, NULL);
        }
        break;
    }
    
    cGen(tree->sibling);
    
    return result;
}

void codeGen(TreeNode * syntaxTree) {
    printf("\n--- CODIGO INTERMEDIARIO (TAC) ---\n\n");
    cGen(syntaxTree);
}