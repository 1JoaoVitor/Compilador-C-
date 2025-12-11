#include <stdio.h>

// Função gerada pelo Bison
extern int yyparse();

int main(int argc, char *argv[]) {
    printf("Iniciando analise...\n");
    
    // Chama o analisador sintático (que chama o léxico)
    if (yyparse() == 0) {
        printf("Analise concluida com sucesso!\n");
    }
    
    return 0;
}