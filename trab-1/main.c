#include <stdio.h>
#include "funcionalidades.h" // A DEFINIR

int main() {
    int func, res;
    char tipoArquivo[6], arquivoEntrada[31];
    scanf("%d", &func);
    scanf("%s %s", tipoArquivo, arquivoEntrada);

    switch(func){
        case 1:
            res = create_table(tipoArquivo, arquivoEntrada);
            break;
        case 2:
            res = select(tipoArquivo, arquivoEntrada);
            break;
        case 3:
            res = select_where(tipoArquivo, arquivoEntrada);
            break;
        case 4:
            res = elect_rrn(tipoArquivo, arquivoEntrada);
            break;
        default:
            printf("Comando invalido! Deve estar entre 1 e 4\n");
            break;
    }
    if (res == 0) printf("Falha no processamento do arquivo.\n");

    return 0;
}
