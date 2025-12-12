%{
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"

int linha_atual = 1;

void erro_lexico(char *lexema) {
    printf("ERRO LEXICO: '%s' LINHA: %d\n", lexema, linha_atual);
}
%}

%option noyywrap

DIGITO      [0-9]
LETRA       [a-zA-Z]
ID          {LETRA}+
NUM         {DIGITO}+
ESPACO      [ \t\r]+

%%

"/*"             { /* remove comentários */
                    char c;
                    while (1) {
                        c = input();
                        if (c == EOF) break;
                        if (c == '\n') linha_atual++;
                        if (c == '*') {
                            char next = input();
                            if (next == '/') break;
                            unput(next);
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

{NUM}            { return NUM; }
{ID}             { return ID; }

\n               { linha_atual++; }
{ESPACO}         {}

.                { erro_lexico(yytext); }

%%