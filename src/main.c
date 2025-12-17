#include <stdio.h>
#include <stdlib.h>
#include "../include/globals.h"
#include "../include/util.h"
#include "../include/analyze.h"
#include "../include/codegen.h"

extern FILE *yyin;
extern int yyparse();
extern TreeNode * savedTree;


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

    printf("--- ANALISE SINTATICA ---\n");
    
    if (yyparse() == 0) {
        printf("Analise concluida.\n\n");
    
        printf("\n--- ANALISE SEMANTICA ---\n");
        buildSymtab(savedTree);
        typeCheck(savedTree);
    
        codeGen(savedTree);

        createDot(savedTree);
    }



    fclose(yyin);
    return 0;
}