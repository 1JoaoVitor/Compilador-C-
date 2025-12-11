#include <stdio.h>

int main() {
    int idade;
    printf("Digite sua idade: ");
    scanf("%d", &idade);
    printf("Ano que vem voce tera %d anos.\n", idade + 1);
    return 0;
}