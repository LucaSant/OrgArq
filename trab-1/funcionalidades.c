#include "funcionalidades.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int create_header(FILE *output, char *fileType) {
    if(strncmp(fileType, "tipo1", 5) == 0) { // Registro de cabeçalho de 182 bytes
        fwrite("0", sizeof(char), 1, output); // status
        fwrite(-1, sizeof(int), 1, output); // topo
        fwrite("LISTAGEM DA FROTA DOS VEICULOS NO BRASIL", sizeof(char), 40, output); // descricao
        fwrite("CODIGO IDENTIFICADOR: ", sizeof(char), 22, output); // desC1
        fwrite("ANO DE FABRICACAO: ", sizeof(char), 19, output); // desC2
        fwrite("QUANTIDADE DE VEICULOS: ", sizeof(char), 24, output); // desC3
        fwrite("ESTADO: ", sizeof(char), 8, output); // desC4
        fwrite("0", sizeof(char), 1, output); // codC5
        fwrite("NOME DA CIDADE: ", sizeof(char), 16, output); // desC5
        fwrite("1", sizeof(char), 1, output); // codC6
        fwrite("MARCA DO VEICULO: ", sizeof(char), 18, output); // desC6
        fwrite("2", sizeof(char), 1, output); // codC7
        fwrite("MODELO DO VEICULO: ", sizeof(char), 19, output); // desC7
        fwrite(0, sizeof(int), 1, output); // proxRRN
        fwrite(0, sizeof(int), 1, output); // nroRegRem
        return 1;
    } else if(strncmp(fileType, "tipo2", 5) == 0) { // Registro de cabeçalho de 190 bytes
        fwrite("0", sizeof(char), 1, output); // status
        fwrite(-1, sizeof(long int), 1, output); // topo
        fwrite("LISTAGEM DA FROTA DOS VEICULOS NO BRASIL", sizeof(char), 40, output); // descricao
        fwrite("CODIGO IDENTIFICADOR: ", sizeof(char), 22, output); // desC1
        fwrite("ANO DE FABRICACAO: ", sizeof(char), 19, output); // desC2
        fwrite("QUANTIDADE DE VEICULOS: ", sizeof(char), 24, output); // desC3
        fwrite("ESTADO: ", sizeof(char), 8, output); // desC4
        fwrite("0", sizeof(char), 1, output); // codC5
        fwrite("NOME DA CIDADE: ", sizeof(char), 16, output); // desC5
        fwrite("1", sizeof(char), 1, output); // codC6
        fwrite("MARCA DO VEICULO: ", sizeof(char), 18, output); // desC6
        fwrite("2", sizeof(char), 1, output); // codC7
        fwrite("MODELO DO VEICULO: ", sizeof(char), 19, output); // desC7
        fwrite(0, sizeof(long int), 1, output); // proxByteOffset
        fwrite(0, sizeof(int), 1, output); // nroRegRem
        return 1;
    } else return 0; // Erro
}

int create_table(char *tipoArquivo, char *arquivoEntrada){
    char arquivoSaida[31];
    scanf("%s", arquivoSaida);
    FILE *input_file = fopen(arquivoEntrada, "r");
    FILE *output_file = fopen(arquivoSaida, "wb+");
    int header = create_header(output_file, tipoArquivo) // Cria cabeçalho do arquivo com base no tipo
    if(!header) return 0; // Erro
    // Selecionar a criação de registos com base no tipo do arquivo
    // Ler (registros separados por linebreak, campos separados por vírgula)
    return 1;
}

int select(char *tipoArquivo, char *arquivoEntrada){
    return 1;
}

int select_where(char *tipoArquivo, char *arquivoEntrada){
    return 1;
}

int select_rrn(char *tipoArquivo, char *arquivoEntrada){
    return 1;
}
