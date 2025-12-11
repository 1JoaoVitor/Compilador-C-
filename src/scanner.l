%{
#include <stdio.h>
#include "parser.tab.h" /* Importante: gerado pelo Bison */
%}

/* Opção para não precisar da biblioteca externa do flex no Windows */
%option noyywrap

%%
[ \t\n]    { /* Ignora espaços e pulos de linha por enquanto */ }
.          { return 0; /* Qualquer outra coisa termina */ }
%%