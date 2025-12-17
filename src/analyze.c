#include "../include/globals.h"
#include "../include/symtab.h"
#include "../include/analyze.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"

static int location = 0;

static char * escopo = "global";

// função para mudar o nome da var (ex: "main$x")  
char * make_scope_name(char * name) {
    if (strcmp(escopo, "global") == 0) return name;
    
    char * buffer = malloc(strlen(escopo) + strlen(name) + 2);
    if (buffer == NULL) { printf("Erro memoria\n"); exit(1); }
    strcpy(buffer, escopo);
    strcat(buffer, "$");
    strcat(buffer, name);
    return buffer;
}

static void traverse(TreeNode * t,
                     void (* preProc) (TreeNode *),
                     void (* postProc) (TreeNode *))
{
    if (t != NULL)
    {
        preProc(t);
        {
            int i;
            for (i=0; i < MAXCHILDREN; i++)
                traverse(t->child[i], preProc, postProc);
        }
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}


static void insertNode(TreeNode * t)
{
    switch (t->nodekind)
    {
        case StmtK:
            break;

        case ExpK:
            switch (t->kind.exp)
            {
                case IdK:
                case CallK:
                    if (st_lookup(make_scope_name(t->attr.name)) != -1) {
                        st_insert(make_scope_name(t->attr.name), t->line_num, 0, "", "");
                    }
                    else if (st_lookup(t->attr.name) != -1) {
                        st_insert(t->attr.name, t->line_num, 0, "", "");
                    }
                    else {
                        printf("ERRO SEMANTICO: Identificador '%s' nao declarado (Escopo: %s) - LINHA: %d\n", t->attr.name, escopo, t->line_num);
                    }
                    break;
                default:
                    break;
            }
            break;

        case DecK:
            switch (t->kind.dec)
            {
                case FunK:
                    if (st_lookup(t->attr.name) == -1) {
                        char * type = (t->type == Integer) ? "int" : "void";
                        st_insert(t->attr.name, t->line_num, location++, "global", type);
                        escopo = t->attr.name;
                    } else {
                        printf("ERRO SEMANTICO: Redeclaracao de funcao '%s' - LINHA:  %d\n", t->attr.name, t->line_num);
                    }
                    break;

                case VarK:
                case ParamK:
                    if (st_lookup(make_scope_name(t->attr.name)) == -1) {
                        char * type = (t->type == Integer) ? "int" : "void";
                        st_insert(make_scope_name(t->attr.name), t->line_num, location++, escopo, type);
                    } else {
                         printf("ERRO SEMANTICO: Redeclaracao de variavel '%s' (Escopo '%s') - LINHA: %d\n", t->attr.name, escopo, t->line_num);
                    }
                    break;
            }
            break;
    }
}

// reseta escopo
static void postProc(TreeNode * t) {
    if (t->nodekind == DecK && t->kind.dec == FunK) {
        escopo = "global";
    }
}

void buildSymtab(TreeNode * syntaxTree)
{
    st_insert("input", 0, 0, "global", "int");
    st_insert("output", 0, 0, "global", "void");
    traverse(syntaxTree, insertNode, postProc);
    printf("\n--- TABELA DE SIMBOLOS ---\n\n");
    printSymTab(stdout);
}


static void typeError(TreeNode * t, char * message) {
    printf("ERRO SEMANTICO: %s - LINHA: %d\n", message, t->line_num);
}

static void checkNode(TreeNode * t) {
    switch (t->nodekind) {
        case ExpK:
            switch (t->kind.exp) {
                case OpK:
                    if ((t->attr.op == SOMA) || (t->attr.op == SUB) ||
                        (t->attr.op == MULT) || (t->attr.op == DIV)) {
                        if ((t->child[0]->type != Integer) || (t->child[1]->type != Integer))
                            typeError(t, "Operacao aritmetica aplicada a nao-inteiro");
                        t->type = Integer;
                    }
                    else if ((t->attr.op == MENOR) || (t->attr.op == IGUAL) || 
                             (t->attr.op == MAIOR) || (t->attr.op == MAIORIGUAL) ||
                             (t->attr.op == MENORIGUAL) || (t->attr.op == DIFERENTE)) {
                        if ((t->child[0]->type != Integer) || (t->child[1]->type != Integer))
                            typeError(t, "Comparacao aplicada a nao-inteiro");
                        t->type = Boolean;
                    }
                    else if (t->attr.op == ATRIB) {
                        if (t->child[0]->type != Integer) typeError(t, "Atribuicao para variavel nao-inteira");
                        if (t->child[1]->type != Integer) typeError(t, "Atribuicao de valor nao-inteiro");
                        t->type = Integer;
                    }
                    break;

                case ConstK:
                    t->type = Integer;
                    break; 

                case IdK:
                case CallK:
                    {
                        char * scopeName = make_scope_name(t->attr.name);
                        if (st_lookup(scopeName) != -1)
                            t->type = st_lookup_type(scopeName);
                        else
                            t->type = st_lookup_type(t->attr.name);
                    }
                    break;
                    
                default: break;
            }
            break;

        case StmtK:
            switch (t->kind.stmt) {
                case IfK:
                case WhileK:
                    if (t->child[0]->type == Void)
                        typeError(t->child[0], "Condicao do IF/WHILE nao pode ser Void");
                    break;
                default: break;
            }
            break;
            
        case DecK: // requisição do louden
            if (t->kind.dec == FunK) {
                if (strcmp(t->attr.name, "main") == 0) {
                    if (t->type != Void) {
                        typeError(t, "A funcao 'main' deve ser declarada como 'void main(void)'");
                    }
                    if (t->child[0] != NULL && t->child[0]->type != Void) {
                         typeError(t, "A funcao 'main' nao deve receber parametros (use void)");
                    }
                }
                escopo = "global";
            }
            break;
    }
}

static void preCheck(TreeNode * t) {
    if (t->nodekind == DecK && t->kind.dec == FunK) {
        escopo = t->attr.name;
    }
}

void typeCheck(TreeNode * syntaxTree)
{
    escopo = "global";
    traverse(syntaxTree, preCheck, checkNode);
}