// Antônio Medrado 10748389

#include <stdio.h>
#include "registros.h"

int main() {
    int comando, res;
    scanf("%d", &comando);
    char filename[31];
    scanf("%s", filename);

    switch(comando){
        case 1:
            res = gravar(filename);
            break;
        case 2:
            res = recuperar(filename);
            break;
        case 3:
            res = recuperarRegistro(filename);
            break;
        default:
            printf("Comando invalido! Deve estar entre 1 e 3\n");
            break;
    }
    if (res == 0) printf("Falha no processamento do arquivo\n");
    else if (res == -1) printf("Nao foi possível ler o arquivo\n");

    return 0;
}
