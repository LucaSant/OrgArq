#include "funcionalidades.h"
#include <stdio.h>

int main() {
    int func, res;
    char tipoArquivo[6], arquivoEntrada[31], arquivoSaida[31];
    scanf("%d", &func);
    scanf("%s %s", tipoArquivo, arquivoEntrada);
    switch(func){
        case 1: 
            scanf("%s", arquivoSaida);
            res = createTable(arquivoEntrada, tipoArquivo, arquivoSaida);
            break;
        case 2:
            res = select(tipoArquivo, arquivoEntrada);
            break;
        case 3:
            res = select_where(tipoArquivo, arquivoEntrada);
            break;
        case 4:
            res = select_rrn(tipoArquivo, arquivoEntrada);
            break;
        default:
            printf("Comando invalido! Deve estar entre 1 e 4\n");
            break;
    }
    if (res == 0) printf("Falha no processamento do arquivo.\n");

    return 0;
}