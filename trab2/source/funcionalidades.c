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
    if(fileType == 0) return 0; // Ero (tipo errado)

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
    if(fileType == 0) return 0; // Ero (tipo errado)

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
    if(fileType == 0) return 0; // Ero (tipo errado)

    FILE *input_file = fopen(get_path(arquivoEntrada), "rb");
    if(input_file == NULL) return 0; // Erro (não há arquivo)

    char status[1];
    fread(status, sizeof(char), 1, input_file);
    if(strncmp(status, "1", 1) != 0) return 0; // Erro (arquivo inconsistente)
    
    vehicle *filtro = field_to_struct(); // Cria veículo com info dos filtros de busca de campos

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
    free(reg);
    fclose(input_file);
    return 1;
}

// Funcionalidade [5]: Criar arquivo de índice simples primário, campo de busca = id
int create_index(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Ero (tipo errado)
    return 1;
}

// Funcionalidade [6]: Fazer remoção lógica de registro em arquivo seguindo abordagem dinâmica e Worst Fit
int remove_reg(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Ero (tipo errado)

    int headerSize = fileType == 1 ? 182 : 190;
    char arquivoIndice[31];
    int n; // Número de remoções
    scanf("%s %d", arquivoIndice, &n);

    FILE *data_file = fopen(get_path(arquivoEntrada), "rb+");
    FILE *index_file = fopen(get_path(arquivoIndice), "rb+");
    vehicle *filtro; // Filtro de busca

    // Para cada remoção...
    for(int i = 0; i < n; i++) {
        filtro = field_to_struct();
        // Para cada filtro, pesquisar no arquivo e fazer as devidas remoções
        if(filtro->id != -1) { // Busca usando arquivo de índice
            // ...
        } else { // Busca linear no próprio arquivo de dados
            fseek(data_file, 0, SEEK_END);
            long eof = ftell(data_file); // Byte offset no fim do arquivo
            fseek(data_file, headerSize, SEEK_SET);
            vehicle *vh;
            long offset;

            while(ftell(data_file) < eof) {
                offset = ftell(data_file);
                vh = reg_to_struct(data_file, fileType);
                // Se registro não está logicamente removido e condiz com filtro, faz sua remoção
                if(filter_cmp(filtro, vh) == 1 && strncmp(vh->removido, "1", 1) != 0) {
                    fseek(data_file, offset, SEEK_SET);
                    rem_register(data_file, index_file, fileType, offset);
                }
                free(vh);
            }
        }
        free(filtro);
    }

    fclose(data_file);
    fclose(index_file);
    binarioNaTela(get_path(arquivoEntrada));
    binarioNaTela(get_path(arquivoIndice));
    return 1;
}

// Funcionalidade [7]: Fazer inserção de registro em arquivo seguindo abordagem dinâmica e Worst Fit
int insert_reg(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Ero (tipo errado)

    char arquivoIndice[31];
    int n; // Número de inserções
    scanf("%s %d", arquivoIndice, &n);

    FILE *data_file = fopen(get_path(arquivoEntrada), "rb+");
    FILE *index_file = fopen(get_path(arquivoIndice), "rb+");
    vehicle *vh; // Contém info do registro a ser inserido
    long offset;

    // Para cada inserção...
    for(int i = 0; i < n; i++) {
        vh = vh_from_input(); // Adquire informações do registro com input
        offset = find_added_stack_position(data_file, fileType);
        if(fileType == 1) offset = (offset * 97) + 182; // De RRN para byte offset
        add_register(vh, data_file, index_file, fileType, offset); // Insere o registro    
        free(vh);
    }

    fclose(data_file);
    fclose(index_file);
    binarioNaTela(get_path(arquivoEntrada));
    binarioNaTela(get_path(arquivoIndice));
    return 1;
}

// Funcionalidade [8]: Fazer atualização de registros em arquivo seguindo abordagem dinâmica
int update_reg(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Ero (tipo errado)

    int headerSize = fileType == 1 ? 182 : 190;
    char arquivoIndice[31];
    int n; // Número de atualizações
    scanf("%s %d", arquivoIndice, &n);

    FILE *data_file = fopen(get_path(arquivoEntrada), "rb+");
    FILE *index_file = fopen(get_path(arquivoIndice), "rb+");
    vehicle *filtro; // Filtro de busca
    vehicle *updt; // Contém campos a se atualizar no vetor

    // Para cada atualização...
    for(int i = 0; i < n; i++) {
        filtro = field_to_struct();
        updt = field_to_struct();
        // Para cada filtro, pesquisar no arquivo e fazer as devidas remoções
        if(filtro->id != -1) { // Busca usando arquivo de índice
            // ...
        } else { // Busca linear no próprio arquivo de dados
            fseek(data_file, 0, SEEK_END);
            long eof = ftell(data_file); // Byte offset no fim do arquivo
            fseek(data_file, headerSize, SEEK_SET);
            vehicle *vh;
            long offset;
            int regSize;

            while(ftell(data_file) < eof) {
                offset = ftell(data_file);
                vh = reg_to_struct(data_file, fileType);
                // Se registro não está logicamente removido e condiz com filtro, faz sua atualização
                if(filter_cmp(filtro, vh) == 1 && strncmp(vh->removido, "1", 1) != 0) {
                    regSize = vh->tamanhoRegistro; // Tamanho do registro antes de atualizar
                    update_vehicle(vh, updt, fileType);
                    if(fileType == 2) { // Registros de tamanho variável
                        if(vh->tamanhoRegistro > regSize) {
                            // Necessário marcar o registro antigo como removido e inserir o atualizado em nova posição
                            rem_register(data_file, index_file, fileType, offset);

                            // Checar se existe registro removido que comporte o tamanho. Senão, inserir no fim do arquivo
                            offset = find_added_stack_position(data_file, fileType);
                            fseek(data_file, offset+1, SEEK_SET);
                            fread(&regSize, sizeof(int), 1, data_file);
                            if(vh->tamanhoRegistro > regSize) offset = eof;
                        } else {
                            // Caso o novo registro seja menor que o anterior, ajustar tamanho para fins de tratamento de lixo
                            vh->tamanhoRegistro = regSize; // Guarda valor do registro anterior, maior que o novo
                        }
                    }
                    // Irá (a) sobrescrever o registro antigo, (b) sobrescrever outro registro removido ou (c) ser escrito no final
                    add_register(vh, data_file, index_file, fileType, offset);
                }
                free(vh);
            }
        }
        free(filtro);
        free(updt);
    }

    fclose(data_file);
    fclose(index_file);
    binarioNaTela(get_path(arquivoEntrada));
    binarioNaTela(get_path(arquivoIndice));
    return 1;
}