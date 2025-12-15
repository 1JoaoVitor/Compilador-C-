#include <stdio.h>
#include <stdlib.h>
#include "../include/globals.h"
#include "../include/util.h"
#include "../include/analyze.h"
#include "../include/codegen.h"

extern FILE *yyin;
extern int yyparse();
extern TreeNode * savedTree; /* Importado do parser */

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

    printf("--- INICIANDO ANALISE SINTATICA ---\n");
    
    if (yyparse() == 0) {
        printf("SUCESSO: Analise Concluida!\n");
        printf("\n--- ARVORE SINTATICA (AST) ---\n\n");
        printTree(savedTree); /* REQUISITO: Imprimir AST */

        /* Requisito: Analise Semantica */
        printf("\n--- INICIANDO ANALISE SEMANTICA ---\n");
        buildSymtab(savedTree);
        typeCheck(savedTree);

        /* Geração de Código */
        codeGen(savedTree, "saida.txt");

    }



    fclose(yyin);
    return 0;
}