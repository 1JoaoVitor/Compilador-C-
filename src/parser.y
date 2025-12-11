%{
#include <stdio.h>
#include <stdlib.h>

// Protótipos necessários para o Bison
int yylex();
int yyerror(char *message);
%}

/* Declaração de Tokens (exemplo inicial) */
%token IF ELSE INT RETURN ID NUM

%%
/* Regras Gramaticais (Dummy) */
programa: /* vazio */
    ;
%%

int yyerror(char *message) {
    fprintf(stderr, "Erro Sintatico: %s\n", message);
    return 0;
}