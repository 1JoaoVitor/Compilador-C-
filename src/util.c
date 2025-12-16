#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/globals.h"
#include "../include/util.h"
#include "parser.tab.h" /* Para acessar os tokens SOMA, IF, etc */

extern int lineno;

/* --- FUNÇÕES DE CRIAÇÃO DE NÓS (Faltavam estas!) --- */

TreeNode * newStmtNode(StmtKind kind)
{ 
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
        printf("Out of memory error at line %d\n",lineno);
    else {
        for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
    }
    return t;
}

TreeNode * newExpNode(ExpKind kind)
{ 
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
        printf("Out of memory error at line %d\n",lineno);
    else {
        for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void;
    }
    return t;
}

TreeNode * newDecNode(DecKind kind)
{ 
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
        printf("Out of memory error at line %d\n",lineno);
    else {
        for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = DecK;
        t->kind.dec = kind;
        t->lineno = lineno;
    }
    return t;
}

char * copyString(char * s)
{ 
    int n;
    char * t;
    if (s==NULL) return NULL;
    n = strlen(s)+1;
    t = malloc(n);
    if (t==NULL)
        printf("Out of memory error at line %d\n",lineno);
    else strcpy(t,s);
    return t;
}

/* --- FUNÇÕES DE IMPRESSÃO DA ÁRVORE --- */

/* Variável de indentação para criar a hierarquia visual */
static int indentno = 0;

/* Macros para aumentar/diminuir indentação */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* Imprime espaços para indentação */
static void printSpaces(void) {
    int i;
    for (i=0; i<indentno; i++)
        printf(" "); /* Imprime na tela */
}

/* Função principal de impressão da árvore */
void printTree(TreeNode * tree) {
    int i;
    INDENT;
    while (tree != NULL) {
        printSpaces();
        
        if (tree->nodekind == StmtK) {
            switch (tree->kind.stmt) {
                case IfK:
                    printf("If\n");
                    break;
                case WhileK:
                    printf("While\n");
                    break;
                case ReturnK:
                    printf("Return\n");
                    break;
                case CompoundK:
                    printf("Compound Statement\n");
                    break;
                default:
                    printf("Unknown StmtNode kind\n");
                    break;
            }
        }
        else if (tree->nodekind == ExpK) {
            switch (tree->kind.exp) {
                case OpK:
                    printf("Op: ");
                    /* Traduz o número interno do Bison para o símbolo real */
                    switch(tree->attr.op) {
                        case ATRIB: printf("=\n"); break;
                        case SOMA: printf("+\n"); break;
                        case SUB: printf("-\n"); break;
                        case MULT: printf("*\n"); break;
                        case DIV: printf("/\n"); break;
                        case MENOR: printf("<\n"); break;
                        case IGUAL: printf("==\n"); break;
                        case MAIOR: printf(">\n"); break;
                        case MAIORIGUAL: printf(">=\n"); break;
                        case MENORIGUAL: printf("<=\n"); break;
                        case DIFERENTE: printf("!=\n"); break;
                        default: printf("Unknown Op: %d\n", tree->attr.op); break;
                    }
                    break;
                case ConstK:
                    printf("Const: %d\n", tree->attr.val);
                    break;
                case IdK:
                    printf("Id: %s\n", tree->attr.name);
                    break;
                case CallK:
                    printf("Chamada Funcao: %s\n", tree->attr.name);
                    break;
                case TypeK:
                    break; 
                default:
                    printf("Unknown ExpNode kind\n");
                    break;
            }
        }
        else if (tree->nodekind == DecK) {
            switch (tree->kind.dec) {
                case FunK:
                    printf("Funcao: %s\n", tree->attr.name);
                    break;
                case VarK:
                    printf("Var: %s\n", tree->attr.name);
                    break;
                case ParamK:
                    printf("Param: %s\n", tree->attr.name);
                    break;
                default:
                    printf("Unknown DecNode kind\n");
                    break;
            }
        }
        else {
            printf("Unknown Node kind\n");
        }
        
        /* Recursão para os filhos */
        for (i=0; i<MAXCHILDREN; i++)
            printTree(tree->child[i]);
        
        /* Próximo irmão */
        tree = tree->sibling;
    }
    UNINDENT;
}

/* --- GERAÇÃO DE ARQUIVO DOT (GRAPHVIZ) --- */

/* Variável auxiliar para gerar IDs únicos para os nós do grafo */
static int nodeCount = 0;

/* Função recursiva interna que escreve no arquivo */
/* Retorna o ID do nó criado para que o pai possa se conectar a ele */
/* src/util.c - Versão modificada para esconder "Compound" */

static int printDotNode(FILE * fp, TreeNode * tree) {
    if (tree == NULL) return -1;
    
    int myID = nodeCount++;
    int childID;
    int i;
    
    /* 1. Escreve a definição do nó atual */
    fprintf(fp, "  node%d [label=\"", myID);
    
    /* (MANTENHA A PARTE DE SWITCH/CASE DOS LABELS IGUAL AO ANTERIOR...) */
    if (tree->nodekind == StmtK) {
        switch (tree->kind.stmt) {
            case IfK: fprintf(fp, "If"); break;
            case WhileK: fprintf(fp, "While"); break;
            case ReturnK: fprintf(fp, "Return"); break;
            case CompoundK: fprintf(fp, "Compound"); break; /* Ainda definimos caso apareça na raiz */
            default: fprintf(fp, "Stmt"); break;
        }
    } else if (tree->nodekind == ExpK) {
        /* ... (Copie os switchs de ExpK do código anterior aqui) ... */
        switch (tree->kind.exp) {
            case OpK: 
                switch(tree->attr.op) {
                    case ATRIB: fprintf(fp, "="); break;
                    case SOMA: fprintf(fp, "+"); break;
                    case SUB: fprintf(fp, "-"); break;
                    case MULT: fprintf(fp, "*"); break;
                    case DIV: fprintf(fp, "/"); break;
                    case MENOR: fprintf(fp, "<"); break;
                    case IGUAL: fprintf(fp, "=="); break;
                    case MAIOR: fprintf(fp, ">"); break;
                    case MAIORIGUAL: fprintf(fp, ">="); break;
                    case MENORIGUAL: fprintf(fp, "<="); break;
                    case DIFERENTE: fprintf(fp, "!="); break;
                    default: fprintf(fp, "Op"); break;
                }
                break;
            case ConstK: fprintf(fp, "%d", tree->attr.val); break;
            case IdK: fprintf(fp, "%s", tree->attr.name); break;
            case CallK: fprintf(fp, "call %s", tree->attr.name); break;
            default: fprintf(fp, "Exp"); break;
        }
    } else if (tree->nodekind == DecK) {
        /* ... (Copie os switchs de DecK do código anterior aqui) ... */
        switch (tree->kind.dec) {
            case FunK: fprintf(fp, "Func %s", tree->attr.name); break;
            case VarK: fprintf(fp, "Var %s", tree->attr.name); break;
            case ParamK: fprintf(fp, "Param %s", tree->attr.name); break;
            default: fprintf(fp, "Dec"); break;
        }
    }
    
    fprintf(fp, "\"];\n");
    
    /* 2. Processa os filhos (AQUI ESTÁ A MÁGICA PARA REMOVER COMPOUND) */
    for (i = 0; i < MAXCHILDREN; i++) {
        TreeNode * child = tree->child[i];
        
        if (child != NULL) {
            /* SE O FILHO FOR UM COMPOUND, PULAMOS ELE! */
            if (child->nodekind == StmtK && child->kind.stmt == CompoundK) {
                /* O Compound tem 2 filhos fixos no Louden:
                   child[0] = Declarações Locais
                   child[1] = Lista de Statements 
                   
                   Vamos ligar o "myID" (Pai) diretamente nos conteúdos do Compound */
                   
                int k;
                for (k=0; k<2; k++) { /* Itera sobre as declarações e os statements */
                    TreeNode * grandChild = child->child[k];
                    while (grandChild != NULL) {
                        childID = printDotNode(fp, grandChild);
                        fprintf(fp, "  node%d -> node%d;\n", myID, childID);
                        grandChild = grandChild->sibling;
                    }
                }
            }
            else {
                /* COMPORTAMENTO PADRÃO PARA OUTROS NÓS */
                TreeNode * temp = child;
                while (temp != NULL) {
                    childID = printDotNode(fp, temp);
                    fprintf(fp, "  node%d -> node%d;\n", myID, childID);
                    temp = temp->sibling;
                }
            }
        }
    }
    
    return myID;
}

void createDot(TreeNode * tree) {
    FILE * fp = fopen("arvore.dot", "w");
    if (fp == NULL) {
        printf("Erro ao criar arquivo arvore.dot\n");
        return;
    }
    
    nodeCount = 0;
    fprintf(fp, "digraph G {\n");
    fprintf(fp, "  node [shape=box, fontname=\"Arial\"];\n"); /* Estilo padrão */
    
    /* Começa a recursão. Note que a raiz também pode ter siblings (lista de funções) */
    while (tree != NULL) {
        printDotNode(fp, tree);
        tree = tree->sibling;
    }
    
    fprintf(fp, "}\n");
    fclose(fp);
    printf("\nArquivo 'arvore.dot' gerado com sucesso!\n");
}