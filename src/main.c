
#include <stdio.h>
#include "parser.tab.h" 

extern int yylex();
extern char* yytext;
extern int linha_atual;
extern FILE *yyin;

// debug
const char* nomes_tokens[] = {
    "FIM", "IF", "ELSE", "INT", "RETURN", "VOID", "WHILE",
    "ID", "NUM", "SOMA", "SUB", "MULT", "DIV",
    "MENOR", "MENORIGUAL", "MAIOR", "MAIORIGUAL", "IGUAL", "DIFERENTE", "ATRIB",
    "PONTOVIRGULA", "VIRGULA", "APAREN", "FPAREN", "ACOLCH", "FCOLCH", "ACHAVE", "FCHAVE"
};

// Mapeia o código numérico do token para string (ajuste conforme o parser.tab.h gera)
// Nota: O Bison gera tokens começando de 258. Essa função é um helper simples.
void print_token(int token) {
    if(token >= 258) {
        printf("Token: %d (%s) \tLexema: %s \tLinha: %d\n", token, "TOKEN", yytext, linha_atual);
    }
    else {
        printf("Token: %d (ASCII) \tLexema: %s \tLinha: %d\n", token, yytext, linha_atual);
    }
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        printf("Erro ao abrir arquivo %s\n", argv[1]);
        return 1;
    }

    printf("--- INICIANDO ANALISE LEXICA ---\n");
    
    int token;
    while ((token = yylex()) != 0) {
        // Exibe cada token encontrado
        // O código do token (ex: 258 para IF) vem do Bison
        printf("LINHA %d: Token %d, Lexema: %s\n", linha_atual, token, yytext);
    }

    printf("--- FIM DA ANALISE LEXICA ---\n");
    fclose(yyin);
    return 0;
}

