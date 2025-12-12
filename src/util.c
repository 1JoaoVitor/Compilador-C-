#include "../include/globals.h"
#include "../include/util.h"
#include <string.h>

/* Função para criar novo nó de Declaração */
TreeNode * newDecNode(DecKind kind) {
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL) printf("Erro de memoria ao criar no de declaracao\n");
    else {
        for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = DecK;
        t->kind.dec = kind;
        t->lineno = linha_atual;
    }
    return t;
}

/* Função para criar novo nó de Instrução (Statement) */
TreeNode * newStmtNode(StmtKind kind) {
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL) printf("Erro de memoria ao criar no de instrucao\n");
    else {
        for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = linha_atual;
    }
    return t;
}

/* Função para criar novo nó de Expressão */
TreeNode * newExpNode(ExpKind kind) {
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL) printf("Erro de memoria ao criar no de expressao\n");
    else {
        for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = linha_atual;
        t->type = Void;
    }
    return t;
}

/* Variável auxiliar para indentação da impressão */
static int indentno = 0;
#define INDENT indentno+=2
#define UNINDENT indentno-=2

static void printSpaces(void) {
    int i;
    for (i=0;i<indentno;i++) printf(" ");
}

/* Procedimento recursivo para imprimir a árvore */
void printTree(TreeNode * tree) {
    int i;
    INDENT;
    while (tree != NULL) {
        printSpaces();
        if (tree->nodekind == StmtK) {
            switch (tree->kind.stmt) {
                case IfK: printf("If\n"); break;
                case WhileK: printf("While\n"); break;
                case ReturnK: printf("Return\n"); break;
                case CompoundK: printf("Compound Statement\n"); break;
                default: printf("Unknown ExpNode kind\n"); break;
            }
        }
        else if (tree->nodekind == ExpK) {
            switch (tree->kind.exp) {
                case OpK: printf("Op: "); 
                          /* Aqui você pode melhorar imprimindo o simbolo (+, -) */
                          printf("%d\n", tree->attr.op); 
                          break;
                case ConstK: printf("Const: %d\n",tree->attr.val); break;
                case IdK: printf("Id: %s\n",tree->attr.name); break;
                case CallK: printf("Chamada de Funcao: %s\n", tree->attr.name); break;
                case TypeK: printf("Tipo\n"); break;
                default: printf("Unknown ExpNode kind\n"); break;
            }
        }
        else if (tree->nodekind == DecK) {
            switch (tree->kind.dec) {
                case VarK: printf("Var: %s\n", tree->attr.name); break;
                case FunK: printf("Funcao: %s\n", tree->attr.name); break; 
                case ParamK: printf("Parametro: %s\n", tree->attr.name); break;
                default: printf("Unknown DecNode kind\n"); break;
            }
        }
        else printf("Unknown Node kind\n");

        /* Chamada recursiva para os filhos */
        for (i=0;i<MAXCHILDREN;i++)
             printTree(tree->child[i]);
        
        tree = tree->sibling; /* Passa para o próximo irmão */
    }
    UNINDENT;
}

char * copyString(char * s) {
    int n;
    char * t;
    if (s==NULL) return NULL;
    n = strlen(s)+1;
    t = malloc(n);
    if (t==NULL) printf("Erro de memoria no copyString\n");
    else strcpy(t,s);
    return t;
}