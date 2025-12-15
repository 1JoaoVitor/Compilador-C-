#include "../include/globals.h"
#include "../include/symtab.h"
#include "../include/analyze.h"
#include "parser.tab.h"
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
                        st_insert(make_scope_name(t->attr.name), t->lineno, 0, Integer);
                    }
                    /* 2. Se não achar, busca no escopo GLOBAL */
                    else if (st_lookup(t->attr.name) != -1) {
                        st_insert(t->attr.name, t->lineno, 0, Integer);
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
                        st_insert(t->attr.name, t->lineno, location++, t->type);
                        
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
                        st_insert(make_scope_name(t->attr.name), t->lineno, location++, t->type);
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
    st_insert("input", 0, 0, Integer);
    st_insert("output", 0, 0, Void);
    
    /* Usamos afterNode para saber quando SAIMOS de uma função */
    traverse(syntaxTree, insertNode, afterNode);
    
    printf("\n--- TABELA DE SIMBOLOS ---\n\n");
    printSymTab(stdout);
}

/* Verificação de Tipos */

/* Função auxiliar pra imprimir erros de tipo sem parar a compilação */
static void typeError(TreeNode * t, char * message) {
    printf("ERRO SEMANTICO: %s LINHA: %d\n", message, t->lineno);
}

/* Callback chamado DEPOIS de processar os filhos (Pós-ordem) */
/* Aqui verificamos se os tipos dos filhos são compatíveis com o pai */
static void checkNode(TreeNode * t) {
    switch (t->nodekind) {
        case ExpK:
            switch (t->kind.exp) {
                case OpK:
                    /* Verifica operações aritméticas */
                    if ((t->attr.op == SOMA) || (t->attr.op == SUB) ||
                        (t->attr.op == MULT) || (t->attr.op == DIV)) {
                        
                        if ((t->child[0]->type != Integer) ||
                            (t->child[1]->type != Integer)) {
                            typeError(t, "Operacao aritmetica aplicada a nao-inteiro");
                        }
                        t->type = Integer;
                    }
                    /* Verifica comparações */
                    else if ((t->attr.op == MENOR) || (t->attr.op == IGUAL) || /* Adicione outros se houver */
                             (t->attr.op == MAIOR)) {
                        
                        if ((t->child[0]->type != Integer) ||
                            (t->child[1]->type != Integer)) {
                            typeError(t, "Comparacao aplicada a nao-inteiro");
                        }
                        t->type = Boolean; /* Resultado de comparação é Booleano */
                    }
                    /* Verifica Atribuição */
                    else if (t->attr.op == ATRIB) {
                        if (t->child[0]->type != Integer) /* Variável */
                            typeError(t, "Atribuicao para variavel nao-inteira");
                        
                        if (t->child[1]->type != Integer) /* Valor */
                            typeError(t, "Atribuicao de valor nao-inteiro");
                            
                        t->type = Integer;
                    }
                    break;

                case ConstK:
                    t->type = Integer;
                    break;

                case IdK:
                case CallK:
                    /* Busca o tipo na tabela de símbolos */
                    /* Tenta escopo local primeiro, depois global */
                    {
                        char * scopeName = make_scope_name(t->attr.name);
                        /* Se existir no local, pega o tipo local */
                        if (st_lookup(scopeName) != -1) {
                            t->type = st_lookup_type(scopeName);
                        } 
                        /* Senão, pega global */
                        else {
                            t->type = st_lookup_type(t->attr.name);
                        }
                    }
                    break;
                    
                default:
                    break;
            }
            break;

        case StmtK:
            switch (t->kind.stmt) {
                case IfK:
                case WhileK:
                    /* Em C-, a condição deve ser avaliada (geralmente Integer ou Boolean) */
                    /* Se for Void, é erro */
                    if (t->child[0]->type == Void) {
                        typeError(t->child[0], "Condicao do IF/WHILE nao pode ser Void");
                    }
                    break;
                case ReturnK:
                    /* Poderíamos verificar se o retorno bate com a função, 
                       mas precisamos saber qual função estamos. 
                       Por simplicidade, verificamos se não está retornando void em expressão */
                    break;
                default:
                    break;
            }
            break;
            
        case DecK:
            /* Atualiza o escopo ao SAIR de uma função (Pós-ordem) */
            if (t->kind.dec == FunK) {
                escopo = "global";
            }
            break;
    }
}

/* Callback chamado ANTES de processar os filhos */
/* Usado para entrar no escopo da função */
static void preCheck(TreeNode * t) {
    if (t->nodekind == DecK && t->kind.dec == FunK) {
        escopo = t->attr.name;
    }
}

void typeCheck(TreeNode * syntaxTree)
{
    /* Garante que começamos no global */
    escopo = "global";
    
    /* Percorre a árvore verificando tipos */
    /* preCheck: entra no escopo */
    /* checkNode: verifica tipos e sai do escopo */
    traverse(syntaxTree, preCheck, checkNode);

    if (st_lookup("main") == -1) {
        printf("ERRO SEMANTICO: Funcao 'main' nao declarada.\n");
    }
}