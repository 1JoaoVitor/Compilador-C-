%{
#include <stdio.h>
#include <string.h>
#include "../include/globals.h"
#include "parser.tab.h"
#include "../include/util.h"
#include <stdlib.h>

extern int lineno;

int linha_atual = 1;

void erro_lexico(char *lexema) {
    printf("ERRO LEXICO: '%s' LINHA: %d\n", lexema, linha_atual);
    exit(1);
}
%}

%option noyywrap

DIGITO      [0-9]
LETRA       [a-zA-Z]
ID          {LETRA}+
NUM         {DIGITO}+
ESPACO      [ \t\r]+

%%

"/*"             { 
                   char c;
                   for (;;) {
                      /* Loop infinito até achar o fim ou EOF */
                      while ( (c = input()) != '*' && c != EOF ) {
                          if (c == '\n') lineno++; /* IMPORTANTE: Conta linha dentro do comentário */
                      }
                      if ( c == '*' ) {
                          while ( (c = input()) == '*' );
                          if ( c == '/' ) break; /* Encontrou fim do comentário */
                          if ( c == '\n' ) lineno++;
                      }
                      if ( c == EOF ) {
                          printf("ERRO LEXICO: Comentario nao terminado - LINHA: %d\n", lineno);
                          exit(1);
                      }
                   }
                 }

"if"             { return IF; }
"else"           { return ELSE; }
"int"            { return INT; }
"return"         { return RETURN; }
"void"           { return VOID; }
"while"          { return WHILE; }

"+"              { return SOMA; }
"-"              { return SUB; }
"*"              { return MULT; }
"/"              { return DIV; }
"<"              { return MENOR; }
"<="             { return MENORIGUAL; }
">"              { return MAIOR; }
">="             { return MAIORIGUAL; }
"=="             { return IGUAL; }
"!="             { return DIFERENTE; }
"="              { return ATRIB; }
";"              { return PONTOVIRGULA; }
","              { return VIRGULA; }
"("              { return APAREN; }
")"              { return FPAREN; }
"["              { return ACOLCH; }
"]"              { return FCOLCH; }
"{"              { return ACHAVE; }
"}"              { return FCHAVE; }

{NUM}   { yylval.val = atoi(yytext); return NUM; }
{ID}    { yylval.string = copyString(yytext); return ID; }

\n      { linha_atual++; }
{ESPACO}         {}

.                { erro_lexico(yytext); }

%%