#include "../include/globals.h"
#include "../include/symtab.h"
#include "../include/analyze.h"
#include <string.h>

/* Contador para alocação de memória (simulado) */
static int location = 0;

/* Função auxiliar para percorrer a árvore recursivamente */
/* A função 'traverse' aplica 'preProc' antes de visitar filhos e 'postProc' depois */
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

/* Faz nada (usado quando não precisamos de ação no pré ou pós) */
static void nullProc(TreeNode * t)
{
    if (t==NULL) return;
    else return;
}

/* =========================================================== */
/* PASSO 1: CONSTRUÇÃO DA TABELA DE SÍMBOLOS                   */
/* =========================================================== */

/* Função que insere nós na tabela */
static void insertNode(TreeNode * t)
{
    switch (t->nodekind)
    {
        case StmtK:
            /* Se entrar numa função, ou bloco, o ideal seria mudar o escopo. 
               Aqui simplificamos assumindo que o parser já estruturou bem. */
            break;

        case ExpK:
            switch (t->kind.exp)
            {
                case IdK: // Case serve para ambos
                case CallK:
                    /* Verifica se variável/função foi declarada */
                    /* Busca primeiro no escopo atual, depois no global se precisar */
                    /* Por enquanto, apenas verificamos se existe na tabela (lookup) */
                    if (st_lookup(t->attr.name) == -1)
                        /* Neste ponto, em um compilador real, verificariamos escopos.
                           Para este projeto básico, se não achou, marcamos erro */
                        printf("ERRO SEMANTICO: Uso de identificador nao declarado '%s' na linha %d\n", t->attr.name, t->lineno);
                    else
                        /* Se já existe, adicionamos a linha atual como referência */
                        st_insert(t->attr.name, t->lineno, 0);
                    break;
                default:
                    break;
            }
            break;

        case DecK:
            switch (t->kind.dec)
            {
                case VarK: // Case serve para ambos
                case ParamK:
                    /* Declaração de Variável: Insere na tabela */
                    if (st_lookup(t->attr.name) == -1) {
                        /* Inserimos com localização de memória fictícia */
                        st_insert(t->attr.name, t->lineno, location++);
                    } else {
                        printf("ERRO SEMANTICO: Redeclaracao de variavel '%s' na linha %d\n", t->attr.name, t->lineno);
                    }
                    break;

                case FunK:
                    /* Declaração de Função */
                    if (st_lookup(t->attr.name) == -1) {
                        st_insert(t->attr.name, t->lineno, location++);
                        /* Dica: Aqui poderiamos mudar a variavel 'escopo_atual' para o nome da função */
                    } else {
                        printf("ERRO SEMANTICO: Redeclaracao de funcao '%s' na linha %d\n", t->attr.name, t->lineno);
                    }
                    break;
            }
            break;
    }
}

void buildSymtab(TreeNode * syntaxTree)
{
    // Funções nativas do C-
    st_insert("input", 0, 0);
    st_insert("output", 0, 0);

    /* Percorre a árvore em pre-order (visita pai antes dos filhos) para garantir
       que declarações sejam processadas antes do uso */
    traverse(syntaxTree, insertNode, nullProc);
    
    /* Requisito: Imprimir a tabela ao final */
    printf("\n--- TABELA DE SIMBOLOS ---\n\n");
    printSymTab(stdout);
}

/* =========================================================== */
/* PASSO 2: CHECAGEM DE TIPOS (TYPE CHECK)                     */
/* =========================================================== */

static void checkNode(TreeNode * t)
{
    switch (t->nodekind)
    {
        case ExpK:
            switch (t->kind.exp)
            {
                case OpK:
                    /* Exemplo simples: Verificar se os dois filhos são do mesmo tipo */
                    /* (Lógica completa de tipos seria implementada aqui) */
                    break;
                default:
                    break;
            }
            break;
        case StmtK:
            if (t->kind.stmt == IfK)
                /* Verificar se a condição do IF é Booleana ou Inteira */
                ;
            break;
        default:
            break;
    }
}

void typeCheck(TreeNode * syntaxTree)
{
    traverse(syntaxTree, nullProc, checkNode);
}