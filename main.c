#include <stdio.h>
#include "cabecalho.h" // A DEFINIR

int main() {
    int func, res;
    scanf("%d", &func);
    char tipoArquivo[6], arquivoEntrada[31];
    scanf("%s %s", tipoArquivo, arquivoEntrada);

    switch(func){
        case 1:
            char arquivoSaida[31];
            scanf("%s", arquivoSaida);
            res = leituraGravacao(tipoArquivo, arquivoEntrada, arquivoSaida);
            break;
        case 2:
            res = recuperaTudo(tipoArquivo, arquivoEntrada);
            break;
        case 3:
            int n;
            scanf("%d", &n);
            res = recuperaBusca(tipoArquivo, arquivoEntrada, n);
            break;
        case 4:
            int RRN;
            scanf("%d", &RRN);
            res = recuperaRegistro(tipoArquivo, arquivoEntrada, RRN);
            break;
        default:
            printf("Comando invalido! Deve estar entre 1 e 4\n");
            break;
    }
    if (res == 0) printf("Falha no processamento do arquivo.\n");

    return 0;
}