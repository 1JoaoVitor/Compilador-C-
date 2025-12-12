%{
#include <stdio.h>
#include <stdlib.h>

extern int linha_atual;
extern int yylex();
extern char* yytext;
void yyerror(const char *s);
%}

%token IF ELSE INT RETURN VOID WHILE
%token ID NUM
%token SOMA SUB MULT DIV
%token MENOR MENORIGUAL MAIOR MAIORIGUAL IGUAL DIFERENTE ATRIB
%token PONTOVIRGULA VIRGULA APAREN FPAREN ACOLCH FCOLCH ACHAVE FCHAVE

%%
/* Gramática temporária apenas para aceitar tokens soltos para teste */
programa: 
    programa token
    | token
    ;

token: IF | ELSE | INT | RETURN | VOID | WHILE
     | ID | NUM
     | SOMA | SUB | MULT | DIV
     | MENOR | MENORIGUAL | MAIOR | MAIORIGUAL | IGUAL | DIFERENTE | ATRIB
     | PONTOVIRGULA | VIRGULA | APAREN | FPAREN | ACOLCH | FCOLCH | ACHAVE | FCHAVE
     ;
%%

void yyerror(const char *s) {
    printf("ERRO SINTATICO: %s LINHA: %d\n", s, linha_atual);
}