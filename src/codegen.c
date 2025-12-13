#include "../include/globals.h"
#include "../include/symtab.h"
#include "../include/codegen.h"
#include "../src/parser.tab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Contador para variáveis temporárias (t1, t2, t3...) */
static int tmpOffset = 0;

/* Contador para labels (L1, L2...) usados em IF/WHILE */
static int labelOffset = 0;

/* Gera um novo nome temporário (t1, t2...) */
static char * newTemp() {
    static char buffer[10];
    sprintf(buffer, "t%d", ++tmpOffset);
    /* Precisamos duplicar a string para não perder o valor */
    char * s = (char *) malloc(strlen(buffer) + 1);
    strcpy(s, buffer);
    return s;
}

/* Gera um novo label (L1, L2...) */
static char * newLabel() {
    static char buffer[10];
    sprintf(buffer, "L%d", ++labelOffset);
    char * s = (char *) malloc(strlen(buffer) + 1);
    strcpy(s, buffer);
    return s;
}

/* Função recursiva principal para gerar código */
/* Retorna o "nome" da variável onde o resultado do nó está guardado */
static char * cGen(TreeNode * tree) {
    char * p1, * p2;
    char * label1, * label2;
    char * currentTemp;

    if (tree == NULL) return NULL;

    switch (tree->nodekind) {
    
    case StmtK:
        switch (tree->kind.stmt) {
            case IfK:
                p1 = cGen(tree->child[0]); /* Condição */
                label1 = newLabel(); /* Label para o ELSE (ou fim) */
                label2 = newLabel(); /* Label para o FIM */
                
                printf("if_false %s goto %s\n", p1, label1);
                
                cGen(tree->child[1]); /* Bloco THEN */
                
                if (tree->child[2] != NULL) {
                    printf("goto %s\n", label2); /* Pula o else */
                    printf("label %s\n", label1); /* Começo do Else */
                    cGen(tree->child[2]); /* Bloco ELSE */
                    printf("label %s\n", label2); /* Fim do IF */
                } else {
                    printf("label %s\n", label1); /* Fim do IF (sem else) */
                }
                break;

            case WhileK:
                label1 = newLabel(); /* Começo do loop */
                label2 = newLabel(); /* Saída do loop */
                
                printf("label %s\n", label1);
                p1 = cGen(tree->child[0]); /* Condição */
                printf("if_false %s goto %s\n", p1, label2);
                
                cGen(tree->child[1]); /* Corpo */
                printf("goto %s\n", label1); /* Volta pro começo */
                printf("label %s\n", label2);
                break;

            case ReturnK:
                p1 = cGen(tree->child[0]);
                printf("return %s\n", p1 ? p1 : "");
                break;
                
            case CompoundK:
                /* Apenas processa os filhos em ordem */
                cGen(tree->child[0]); // Declarações locais (ignoradas na geração simples)
                cGen(tree->child[1]); // Statements
                break;
        }
        /* Statements não retornam valor temporário */
        return NULL;

    case ExpK:
        switch (tree->kind.exp) {
            case OpK:
                if (tree->attr.op == ATRIB) {
                    p1 = cGen(tree->child[0]); /* Variável destino */
                    p2 = cGen(tree->child[1]); /* Valor */
                    printf("%s = %s\n", p1, p2);
                    return p1;
                }
                
                /* Operação Aritmética */
                p1 = cGen(tree->child[0]);
                p2 = cGen(tree->child[1]);
                currentTemp = newTemp();
                
                char * op = "";
                switch(tree->attr.op) {
                    case SOMA: op = "+"; break;
                    case SUB: op = "-"; break;
                    case MULT: op = "*"; break;
                    case DIV: op = "/"; break;
                    case MENOR: op = "<"; break;
                    case IGUAL: op = "=="; break;
                    /* Adicione outros operadores aqui conforme globals.h */
                    default: op = "?"; break;
                }
                
                printf("%s = %s %s %s\n", currentTemp, p1, op, p2);
                return currentTemp;

            case ConstK:
                currentTemp = newTemp();
                printf("%s = %d\n", currentTemp, tree->attr.val);
                return currentTemp;

            case IdK:
                /* Retorna o próprio nome da variável */
                return tree->attr.name;

            case CallK:
                /* Trata chamadas de função */
                /* Processa argumentos primeiro */
                {
                    TreeNode * arg = tree->child[0];
                    // Precisamos inverter ou listar argumentos. 
                    // Simplificação: apenas gera o código dos args
                    while (arg != NULL) {
                        char * t = cGen(arg);
                        printf("param %s\n", t);
                        arg = arg->sibling;
                    }
                    
                    /* Casos especiais para output/input ou chamada normal */
                    if (strcmp(tree->attr.name, "input") == 0) {
                        currentTemp = newTemp();
                        printf("%s = call input, 0\n", currentTemp);
                        return currentTemp;
                    } 
                    else if (strcmp(tree->attr.name, "output") == 0) {
                        printf("call output, 1\n");
                        return NULL;
                    }
                    else {
                        currentTemp = newTemp();
                        // Contar argumentos seria ideal, aqui pusemos 'n' genérico
                        printf("%s = call %s, n\n", currentTemp, tree->attr.name);
                        return currentTemp;
                    }
                }
                break;
            case TypeK:
                break;
        }
        break;
        
    case DecK:
        /* Declarações não geram código executável, apenas reservam espaço.
           Ignoraremos por enquanto, pois a tabela de símbolos já cuidou disso. */
           if (tree->kind.dec == FunK) {
            printf("func %s\n", tree->attr.name); /* Marca o início da função */
            
            /* O filho 0 são os parâmetros (ignoramos na geração simples) */
            /* O filho 1 é o corpo (Compound Statement) - Visitamos ele! */
            cGen(tree->child[1]);
            
            printf("end %s\n", tree->attr.name); /* Marca o fim (opcional) */
        }
        break;
    }
    
    /* Processa o próximo comando (irmão) */
    cGen(tree->sibling);
    
    return NULL;
}

void codeGen(TreeNode * syntaxTree, char * codefile) {
    // Redireciona stdout para um arquivo se quiser, ou imprime na tela
    // Por simplicidade, imprimiremos na tela
    printf("\n--- CODIGO INTERMEDIARIO (3 ENDERECOS) ---\n\n");
    cGen(syntaxTree);
}