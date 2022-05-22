#include "funcionalidades.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tratamento_de_lixo(char *str, int size) {
    if(str == NULL) return 0; // Erro
    for(int i = 0; i < size; i++) {
        if(str[i] == '$') {
            str[i] = '\0';
            break;
        }
    }
    return 1;
}

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

// Lê todas as informações pedidas (de um registro dado o seu RRN) na ordem também pedida
int leitura_rrn_tipo1(FILE *f, int RRN) {
    int tmp, tam_marca, pos_marca, tam_modelo, pos_modelo, tam_cidade, pos_cidade;
    char buffer1[100], buffer2[100];

    // Para poder acessar corretamente os campos de tamanho variável
    tam_marca = tam_modelo = tam_cidade = -1;
    fseek(f, 182+(97*RRN)+23, SEEK_SET);
    fread(buffer1, sizeof(char), 1, f);
    if(strncmp(buffer1, "0", 1) == 0) {
        pos_cidade = ftell(f);
        fseek(f, -5, SEEK_CUR);
        fread(tam_cidade, sizeof(int), 1, f);
        fseek(f, 5+tam_cidade, SEEK_CUR);
        fread(buffer1, sizeof(char), 1, f);
    }
    if(strncmp(buffer1, "1", 1) == 0) {
        pos_marca = ftell(f);
        fseek(f, -5, SEEK_CUR);
        fread(tam_marca, sizeof(int), 1, f);
        fseek(f, 5+tam_marca, SEEK_CUR);
        fread(buffer1, sizeof(char), 1, f);
    }
    if(strncmp(buffer1, "2", 1) == 0) {
        pos_modelo = ftell(f);
        fseek(f, -5, SEEK_CUR);
        fread(tam_modelo, sizeof(int), 1, f);
        fseek(f, 5+tam_modelo, SEEK_CUR);
        fread(buffer1, sizeof(char), 1, f);
    }

    // Marca
    fseek(f, 136, SEEK_SET);
    fread(buffer1, sizeof(char), 18, f);
    buffer1[18] = '\0';
    if(tam_marca != -1) {
        fseek(f, pos_marca, SEEK_SET);
        fread(buffer2, sizeof(char), tam_marca, f);
        tratamento_de_lixo(buffer2, tam_marca);
    } else strcpy(buffer2, "NAO PREENCHIDO");
    printf("%s%s\n", buffer1, buffer2);

     // Modelo
    fseek(f, 155, SEEK_SET);
    fread(buffer, sizeof(char), 19, f);
    buffer1[19] = '\0';
    if(tam_modelo != -1) {
        fseek(f, pos_modelo, SEEK_SET);
        fread(buffer2, sizeof(char), tam_modelo, f);
        tratamento_de_lixo(buffer2, tam_modelo);
    } else strcpy(buffer2, "NAO PREENCHIDO");
    printf("%s%s\n", buffer1, buffer2);

    // Ano
    fseek(f, 67, SEEK_SET);
    fread(buffer1, sizeof(char), 19, f);
    buffer1[19] = '\0';
    fseek(f, 182+(97*RRN)+9, SEEK_SET);
    fread(buffer2, sizeof(int), 1, f);
    printf("%s%d\n", buffer1, tmp);

    // Cidade
    fseek(f, 119, SEEK_SET);
    fread(buffer1, sizeof(char), 16, f);
    buffer1[16] = '\0';
    if(tam_cidade != -1) {
        fseek(f, pos_cidade, SEEK_SET);
        fread(buffer2, sizeof(char), tam_cidade, f);
        tratamento_de_lixo(buffer2, tam_cidade);
    } else strcpy(buffer2, "NAO PREENCHIDO");
    printf("%s%s\n", buffer1, buffer2);

    // Qtd de veículos
    fseek(f, 86, SEEK_SET);
    fread(buffer1, sizeof(char), 24, f);
    buffer1[24] = '\0';
    fseek(f, 182+(97*RRN)+13, SEEK_SET);
    fread(buffer2, sizeof(int), 1, f);
    printf("%s%d\n", buffer1, tmp);

    printf("\n");
    return 1;
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

// Funcionalidade [4]: Procurar um registro por seu RRN no arquivo e imprimir suas informações
int select_rrn(char *tipoArquivo, char *arquivoEntrada){
    if(strncmp(tipoArquivo, "tipo1", 5) != 0) return 0; // Erro (tipo errado)
    FILE *input_file = fopen(arquivoEntrada, "rb");
    if(input_file == NULL) return 0; // Erro (não há arquivo)

    int RRN;
    scanf("%d", &RRN);
    fseek(input_file, 0, SEEK_END);
    int ttl = (ftell(input_file) - 182) / 97; // Quantidade de registros
    if(RRN >= ttl) return 0; // Erro (não há tal registro)

    leitura_rrn_tipo1(input_file, RRN);
    fclose(input_file);
    return 1;
}
