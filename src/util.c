#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/globals.h"
#include "../include/util.h"
#include "parser.tab.h"

extern int line_num;

TreeNode * newStmtNode(StmtKind kind)
{ 
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
        printf("Out of memory error at line %d\n",line_num);
    else {
        for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->line_num = line_num;
    }
    return t;
}

TreeNode * newExpNode(ExpKind kind)
{ 
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
        printf("Out of memory error at line %d\n",line_num);
    else {
        for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->line_num = line_num;
        t->type = Void;
    }
    return t;
}

TreeNode * newDecNode(DecKind kind)
{ 
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
        printf("Out of memory error at line %d\n",line_num);
    else {
        for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = DecK;
        t->kind.dec = kind;
        t->line_num = line_num;
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
        printf("Out of memory error at line %d\n",line_num);
    else strcpy(t,s);
    return t;
}

int indent_num = 0;

#define INDENT indent_num += 2
#define UNINDENT indent_num -= 2

void printSpaces(void) {
    int i;
    for (i=0; i < indent_num; i++)
        printf(" "); 
}

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
        
        for (i=0; i<MAXCHILDREN; i++)
            printTree(tree->child[i]);
        
        tree = tree->sibling;
    }
    UNINDENT;
}

/* Gera arquivo .dot*/

static int nodeCount = 0;

static int printDotNode(FILE * fp, TreeNode * tree) {
    if (tree == NULL) return -1;
    
    int myID = nodeCount++;
    int childID;
    int i;
    
    fprintf(fp, "  node%d [label=\"", myID);
    
    if (tree->nodekind == StmtK) {
        switch (tree->kind.stmt) {
            case IfK: fprintf(fp, "If"); break;
            case WhileK: fprintf(fp, "While"); break;
            case ReturnK: fprintf(fp, "Return"); break;
            case CompoundK: fprintf(fp, "Compound"); break;
            default: fprintf(fp, "Stmt"); break;
        }
    } else if (tree->nodekind == ExpK) {
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
        switch (tree->kind.dec) {
            case FunK: fprintf(fp, "Func %s", tree->attr.name); break;
            case VarK: fprintf(fp, "Var %s", tree->attr.name); break;
            case ParamK: fprintf(fp, "Param %s", tree->attr.name); break;
            default: fprintf(fp, "Dec"); break;
        }
    }
    
    fprintf(fp, "\"];\n");
    
    for (i = 0; i < MAXCHILDREN; i++) {
        TreeNode * child = tree->child[i];
        
        if (child != NULL) {
            if (child->nodekind == StmtK && child->kind.stmt == CompoundK) {
                   
                int k;
                for (k=0; k<2; k++) { 
                    TreeNode * grandChild = child->child[k];
                    while (grandChild != NULL) {
                        childID = printDotNode(fp, grandChild);
                        fprintf(fp, "  node%d -> node%d;\n", myID, childID);
                        grandChild = grandChild->sibling;
                    }
                }
            }
            else {
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
    fprintf(fp, "  node [shape=box, fontname=\"Arial\"];\n"); 
    
    while (tree != NULL) {
        printDotNode(fp, tree);
        tree = tree->sibling;
    }
    
    fprintf(fp, "}\n");
    fclose(fp);
    printf("\nArquivo 'arvore.dot' gerado com sucesso!\n");
}