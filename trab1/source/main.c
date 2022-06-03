/*
    SCC0215 - Organização de Arquivos (2022.1)
    Antônio Medrado - 10748389
    Lucas Sant'Anna - 10748521
*/

#include <stdio.h>
#include "../headers/funcionalidades.h"
#include "../headers/auxiliares.h"

int main() {
    int func, res; // Funcionalidade (1...4) e resultado (1 = Sucesso | 0 = Falha)
    char tipoArquivo[6], arquivoEntrada[50];
    scanf("%d", &func);
    scanf("%s %s", tipoArquivo, arquivoEntrada);

    switch(func){
        case 1: // Ler registros a partir de um arquivo CSV e salvá-los em um arquivo binário tipo1 ou tipo2
            res = csv_to_bin(tipoArquivo, arquivoEntrada);
            break;
        case 2: // Imprimir todos os registros de um arquivo
            res = read_all(tipoArquivo, arquivoEntrada);
            break;
        case 3: // Imprimir todos os registros que satisfaçam dados filtros de busca
            res = read_filter(tipoArquivo, arquivoEntrada);
            break;
        case 4: // Procurar um registro por seu RRN no arquivo e imprimí-lo
            res = read_rrn(tipoArquivo, arquivoEntrada);
            break;
        default:
            printf("Comando invalido! Deve estar entre 1 e 4\n");
            break;
    }
    if (res == 0) printf("Falha no processamento do arquivo.\n");

    return 0;
}