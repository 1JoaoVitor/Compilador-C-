#include "../include/globals.h"
#include "../include/symtab.h"
#include "../include/analyze.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int location = 0;

/* Variável para controlar o escopo atual */
/* Começa como "global". Quando entra numa função, muda para o nome dela */
static char * escopo = "global";

/* Helper para concatenar escopo + nome (ex: "main$x") */
char * make_scope_name(char * name) {
    /* Se for escopo global, não muda o nome */
    if (strcmp(escopo, "global") == 0) return name;
    
    /* Se for local, cria "escopo$nome" */
    char * buffer = malloc(strlen(escopo) + strlen(name) + 2);
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

//static void nullProc(TreeNode * t) { if (t==NULL) return; else return; }

static void insertNode(TreeNode * t)
{
    switch (t->nodekind)
    {
        case StmtK:
            /* Não fazemos nada específico aqui, o controle de escopo é feito no DecK FunK */
            break;

        case ExpK:
            switch (t->kind.exp)
            {
                case IdK:
                case CallK:
                    /* USO DE VARIÁVEL / CHAMADA */
                    /* 1. Tenta buscar no escopo LOCAL primeiro */
                    if (st_lookup(make_scope_name(t->attr.name)) != -1) {
                        st_insert(make_scope_name(t->attr.name), t->lineno, 0);
                    }
                    /* 2. Se não achar, busca no escopo GLOBAL */
                    else if (st_lookup(t->attr.name) != -1) {
                        st_insert(t->attr.name, t->lineno, 0);
                    }
                    /* 3. Se não achar em nenhum, Erro */
                    else {
                        printf("ERRO SEMANTICO: Identificador '%s' nao declarado (Escopo: %s) na linha %d\n", t->attr.name, escopo, t->lineno);
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
                    /* DECLARAÇÃO DE FUNÇÃO */
                    if (st_lookup(t->attr.name) == -1) {
                        /* Funções sempre ficam no escopo global na tabela */
                        st_insert(t->attr.name, t->lineno, location++);
                        
                        /* IMPORTANTE: Mudamos o escopo atual para o nome da função */
                        escopo = t->attr.name; 
                    } else {
                        printf("ERRO SEMANTICO: Redeclaracao de funcao '%s' na linha %d\n", t->attr.name, t->lineno);
                    }
                    break;

                case VarK:
                case ParamK:
                    /* DECLARAÇÃO DE VARIÁVEL/PARAMETRO */
                    /* Verifica apenas se já existe NO ESCOPO ATUAL */
                    /* Isso permite ter int x global e int x local */
                    if (st_lookup(make_scope_name(t->attr.name)) == -1) {
                        st_insert(make_scope_name(t->attr.name), t->lineno, location++);
                    } else {
                        printf("ERRO SEMANTICO: Redeclaracao de variavel '%s' no escopo '%s' na linha %d\n", t->attr.name, escopo, t->lineno);
                    }
                    break;
            }
            break;
    }
}

/* Função chamada DEPOIS de processar os filhos de um nó */
static void afterNode(TreeNode * t) {
    /* Se terminamos de processar uma função, voltamos para o escopo global */
    if (t->nodekind == DecK && t->kind.dec == FunK) {
        escopo = "global";
    }
}

void buildSymtab(TreeNode * syntaxTree)
{
    st_insert("input", 0, 0);
    st_insert("output", 0, 0);
    
    /* Usamos afterNode para saber quando SAIMOS de uma função */
    traverse(syntaxTree, insertNode, afterNode);
    
    printf("\n--- TABELA DE SIMBOLOS ---\n\n");
    printSymTab(stdout);
}

void typeCheck(TreeNode * syntaxTree)
{
    /* Simplificado para este exemplo */
}