/*
    SCC0215 - Organização de Arquivos (2022.1)
    Antônio Medrado - 10748389
    Lucas Sant'Anna - 10748521
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/auxiliares.h"
#include "../headers/funcionalidades.h"
#include "../headers/structs.h"
#include "../headers/funcoesFornecidas.h"


// Funcionalidade [1]: Ler registros a partir de um arquivo CSV e salvá-los em um arquivo binário tipo1 ou tipo2
int csv_to_bin(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType != 1 && fileType != 2) return 0; // Erro (tipo errado)

    char arquivoSaida[31];
    scanf("%s", arquivoSaida);
    FILE *input_file = fopen(get_path(arquivoEntrada), "r");
    FILE *output_file = fopen(get_path(arquivoSaida), "wb+");

    int header = create_header(output_file, fileType); // Cria cabeçalho do arquivo com base no tipo
    if(!header) return 0; // Erro
    data_reg(output_file, input_file, fileType); // Lê do .csv e  escreve os registros de dados

    fclose(input_file);
    fclose(output_file);
    binarioNaTela(get_path(arquivoSaida));
    return 1;
}

// Funcionalidade [2]: Imprimir todos os registros de um arquivo
int read_all(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Erro (tipo errado)

    FILE *input_file = fopen(get_path(arquivoEntrada), "rb");
    if(input_file == NULL) {
        printf("errooooo\n");
        return 0; // Erro (não há arquivo)

    }
    char status[1];
    fread(status, sizeof(char), 1, input_file);
    if(strncmp(status, "1", 1) != 0) return 0; // Erro (arquivo inconsistente)

    fseek(input_file, 0, SEEK_END);
    long eof = ftell(input_file); // Byte offset no fim do arquivo
    if(fileType == 1) fseek(input_file, 182, SEEK_SET);
    else fseek(input_file, 190, SEEK_SET);
    vehicle *vh;
    while(ftell(input_file) < eof) {
        vh = reg_to_struct(input_file, fileType);
        print_reg(vh);
        free(vh);
    }
    fclose(input_file);
    return 1;
}

// Funcionalidade [3]: Imprimir todos os registros que satisfaçam dados filtros de busca
int read_filter(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Erro (tipo errado)

    FILE *input_file = fopen(get_path(arquivoEntrada), "rb");
    if(input_file == NULL) return 0; // Erro (não há arquivo)

    char status[1];
    fread(status, sizeof(char), 1, input_file);
    if(strncmp(status, "1", 1) != 0) return 0; // Erro (arquivo inconsistente)

    vehicle *filtro = cria_veiculo();
    int n;
    scanf("%d", &n);
    char field_name[20], field_value[50];
    for(int i = 0; i < n; i++) {
        scanf("%s %[^\r\n]", field_name, field_value);
        set_field(filtro, field_name, field_value);
    }

    fseek(input_file, 0, SEEK_END);
    long eof = ftell(input_file); // Byte offset no fim do arquivo
    if(fileType == 1) fseek(input_file, 182, SEEK_SET);
    else fseek(input_file, 190, SEEK_SET);
    vehicle *vh;

    while(ftell(input_file) < eof) {
        vh = reg_to_struct(input_file, fileType);
        if(filter_cmp(filtro, vh) == 1) print_reg(vh);
        free(vh);
    }

    free(filtro);
    fclose(input_file);
    return 1;
}

// Funcionalidade [4]: Procurar um registro por seu RRN no arquivo e imprimí-lo
int read_rrn(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType != 1) return 0; // Erro (tipo errado)

    FILE *input_file = fopen(get_path(arquivoEntrada), "rb");
    if(input_file == NULL) return 0; // Erro (não há arquivo)

    char status[1];
    fread(status, sizeof(char), 1, input_file);
    if(strncmp(status, "1", 1) != 0) return 0; // Erro (arquivo inconsistente)

    int RRN;
    scanf("%d", &RRN);
    fseek(input_file, 0, SEEK_END);
    int ttl = (int) ((ftell(input_file) - 182) / 97); // Quantidade de registros
    if(RRN >= ttl) {
        printf("Registro inexistente.\n");
        return 1;
    }

    fseek(input_file, 182+97*RRN, SEEK_SET);
    vehicle *reg = reg_to_struct(input_file, fileType);
    
    print_reg(reg);
    if(strncmp(reg->removido, "0", 1) != 0) printf("Registro inexistente.\n");
    free(reg);
    fclose(input_file);
    return 1;
}
