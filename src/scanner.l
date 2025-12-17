%{
#include <stdio.h>
#include <string.h>
#include "../include/globals.h"
#include "parser.tab.h"
#include "../include/util.h"
#include <stdlib.h>

int line_num = 1;

void erro_lexico(char *lexema) {
    printf("ERRO LEXICO: '%s' LINHA: %d\n", lexema, line_num);
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
                      while ( (c = input()) != '*' && c != EOF ) {
                          if (c == '\n') line_num++;
                      }
                      if ( c == '*' ) {
                          while ( (c = input()) == '*' );
                          if ( c == '/' ) break; // fim do comentario
                          if ( c == '\n' ) line_num++;
                      }
                      if ( c == EOF ) {
                          printf("ERRO LEXICO: Comentario nao terminado - LINHA: %d\n", line_num);
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

\n      { line_num++; }
{ESPACO}         {}

.                { erro_lexico(yytext); }

%%