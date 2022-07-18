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
#include "../headers/arvore_b.h"

// Funcionalidade [1]: Ler registros a partir de um arquivo CSV e salvá-los em um arquivo binário tipo1 ou tipo2
int csv_to_bin(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Ero (tipo errado)

    char arquivoSaida[31];
    scanf("%s", arquivoSaida);
    FILE *input_file = fopen(arquivoEntrada, "r");
    FILE *output_file = fopen(arquivoSaida, "wb+");

    int header = create_header(output_file, fileType); // Cria cabeçalho do arquivo com base no tipo
    if(!header) return 0; // Erro
    data_reg(output_file, input_file, fileType); // Lê do .csv e  escreve os registros de dados

    fclose(input_file);
    fclose(output_file);
    binarioNaTela(arquivoSaida);
    return 1;
}

// Funcionalidade [2]: Imprimir todos os registros de um arquivo
int read_all(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Ero (tipo errado)

    FILE *input_file = fopen(arquivoEntrada, "rb");
    if(input_file == NULL) {
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

    FILE *input_file = fopen(arquivoEntrada, "rb");
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

    FILE *input_file = fopen(arquivoEntrada, "rb");
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

// Funcionalidade [5]: Criar um arquivo de índice sobre um arquivo de dados de entrada - o campo de busca: id
int create_index(char *tipoArquivo, char* arquivoEntrada){
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Erro (tipo errado)

    char arquivoIndice[31];
    scanf("%s", arquivoIndice);
    FILE *data_file = fopen(arquivoEntrada, "rb+");
    fwrite("0", sizeof(char), 1, data_file); // Consistência
    
    // Passa as informações para a memória e depois escreve-as num arquivo de índice
    
    indlist *ilist = data_to_mem(data_file, fileType);
    int num_ind = ilist->size;
    _index **arr = index_sort(ilist);
    mem_to_index(arquivoIndice, arr, fileType, num_ind);
    
    fseek(data_file, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, data_file); // Consistência
    fclose(data_file);
    
    binarioNaTela(arquivoIndice);
    return 1;
}

// Funcionalidade [6]: Fazer remoção lógica de registro em arquivo seguindo abordagem dinâmica e Worst Fit
int remove_reg(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Erro (tipo errado)

    int headerSize = fileType == 1 ? 182 : 190;
    char arquivoIndice[31];
    int n; // Número de remoções
    scanf("%s %d", arquivoIndice, &n);

    FILE *data_file = fopen(arquivoEntrada, "rb+");
    fwrite("0", sizeof(char), 1, data_file); // Consistência
    FILE *index_file = fopen(arquivoIndice, "rb+");
    fwrite("0", sizeof(char), 1, index_file); // Consistência

    vehicle *filtro; // Filtro de busca
    vehicle *vh; // Contém as informações do registro a ser removido
    // Para busca binária
    int num_ind = 0;
    _index **arr = index_to_mem(index_file, fileType, &num_ind);
    long eof, offset;
    

    // Para cada remoção...
    for(int i = 0; i < n; i++) {
        filtro = field_to_struct();
        // Para cada filtro, pesquisar no arquivo e fazer as devidas remoções
        if(filtro->id != -1) { // Busca usando arquivo de índice

            offset = index_binary_search(arr, 0, num_ind-1, filtro->id, fileType);
            if(offset != -1) { // Encontrou registro com esse ID
                fseek(data_file, offset, SEEK_SET);
                vh = reg_to_struct(data_file, fileType);
                
                // Se registro não está logicamente removido e condiz com filtro, faz sua remoção
                if(filter_cmp(filtro, vh) == 1 && strncmp(vh->removido, "1", 1) != 0) {
                    rem_register(data_file, fileType, offset);
                }
                free(vh);
            }
        } else { // Busca linear no próprio arquivo de dados
            fseek(data_file, 0, SEEK_END);
            eof = ftell(data_file); // Byte offset no fim do arquivo
            fseek(data_file, headerSize, SEEK_SET);

            while(ftell(data_file) < eof) {
                offset = ftell(data_file);
                vh = reg_to_struct(data_file, fileType);
                // Se registro não está logicamente removido e condiz com filtro, faz sua remoção
                if(filter_cmp(filtro, vh) == 1 && strncmp(vh->removido, "1", 1) != 0) {
                    rem_register(data_file, fileType, offset);
                }
                free(vh);
            }
        }
        free(filtro);
    }

    // Limpa o que for necessário e tualiza o arquivo de índice
    destroy_iarr(arr, num_ind);
    fseek(index_file, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, index_file); // Consistência
    fclose(index_file);

    num_ind = 0;
    indlist *ilist = data_to_mem(data_file, fileType);
    num_ind = ilist->size;
    arr = index_sort(ilist);
    mem_to_index(arquivoIndice, arr, fileType, num_ind);

    fseek(data_file, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, data_file); // Consistência
    fclose(data_file);
    
    binarioNaTela(arquivoEntrada);
    binarioNaTela(arquivoIndice);
    return 1;
}

// Funcionalidade [7]: Fazer inserção de registro em arquivo seguindo abordagem dinâmica e Worst Fit
int insert_reg(char *tipoArquivo, char *arquivoEntrada) {
    long aux = -1;
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Ero (tipo errado)

    char arquivoIndice[31];
    int n; // Número de inserções
    scanf("%s %d", arquivoIndice, &n);

    FILE *data_file = fopen(arquivoEntrada, "rb+");
    fwrite("0", sizeof(char), 1, data_file); // Consistência
    FILE *index_file = fopen(arquivoIndice, "rb+");
    fwrite("0", sizeof(char), 1, index_file); // Consistência

    vehicle *vh; // Contém info do registro a ser inserido
    long offset;

    // Para cada inserção...
    for(int i = 0; i < n; i++) {
        vh = vh_from_input(); // Adquire informações do registro com input
        vh->tamanhoRegistro = useful_reg_length(vh, fileType) - 5; // calcula o tamanho do registro
        offset = find_added_stack_position(data_file, (vh->tamanhoRegistro) + 5, fileType);
        if(fileType == 1) offset = (offset * 97) + 182; // De RRN para byte offset
        add_register(vh, data_file, fileType, offset, aux); // Insere o registro    
        free(vh);
    }

    // Limpa o que for necessário e tualiza o arquivo de índice
    fseek(index_file, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, index_file); // Consistência
    fclose(index_file);

    
    indlist *ilist = data_to_mem(data_file, fileType);
    int num_ind = ilist->size;
    _index **arr = index_sort(ilist);
    mem_to_index(arquivoIndice, arr, fileType, num_ind);

    fseek(data_file, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, data_file); // Consistência
    fclose(data_file);

    binarioNaTela(arquivoEntrada);
    binarioNaTela(arquivoIndice);
    return 1;
}

// Funcionalidade [8]: Fazer atualização de registros em arquivo seguindo abordagem dinâmica
int update_reg(char *tipoArquivo, char *arquivoEntrada) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Erro (tipo errado)

    int headerSize = fileType == 1 ? 182 : 190;
    char arquivoIndice[31];
    int n; // Número de atualizações
    scanf("%s %d", arquivoIndice, &n);

    FILE *data_file = fopen(arquivoEntrada, "rb+");
    fwrite("0", sizeof(char), 1, data_file); // Consistência
    FILE *index_file = fopen(arquivoIndice, "rb+");
    fwrite("0", sizeof(char), 1, index_file); // Consistência

    vehicle *filtro; // Filtro de busca
    vehicle *updt; // Contém campos a se atualizar no vetor
    vehicle *vh; // Contém infos do registro a ser atualizado
    long eof, offset;

    indlist *updated_regs = create_upd_list(); // Contém endereços dos registros atualizados
    
    // Para busca binária
    int num_ind = 0;
    _index **arr = index_to_mem(index_file, fileType, &num_ind);

    // Para cada atualização...
    for(int i = 0; i < n; i++) {
        filtro = field_to_struct();
        updt = field_to_struct();
        
        // Para cada filtro, pesquisar no arquivo e fazer as devidas atualizações
        if(filtro->id != -1) { // Busca usando arquivo de índice
            offset = index_binary_search(arr, 0, num_ind-1, filtro->id, fileType);
            if(offset != -1){
                fseek(data_file, offset, SEEK_SET);
                vh = reg_to_struct(data_file, fileType);
                if(filter_cmp(filtro, vh) == 1 && strncmp(vh->removido, "0", 1) == 0) {
                    if(upd_register(vh, updt, data_file, fileType, offset, eof, updated_regs) == 0) return 0;
                }
                free(vh);
            }

        } else { // Busca linear no próprio arquivo de dados
            fseek(data_file, 0, SEEK_END);
            eof = ftell(data_file); // Byte offset no fim do arquivo
            fseek(data_file, headerSize, SEEK_SET);

            while(ftell(data_file) < eof) {
                offset = ftell(data_file);
                vh = reg_to_struct(data_file, fileType);
                // Se registro não está logicamente removido e condiz com filtro, faz sua atualização
                if(filter_cmp(filtro, vh) == 1 && strncmp(vh->removido, "0", 1) == 0) {
                    if(upd_register(vh, updt, data_file, fileType, offset, eof, updated_regs) == 0) return 0;
                }
                free(vh);
            }
        }
        free(filtro);
        free(updt);
    }

    // Limpa/corrige o que for necessário e atualiza o arquivo de índice
    correct_upd_regs(updated_regs ,data_file);

    destroy_iarr(arr, num_ind);
    fseek(index_file, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, index_file); // Consistência
    fclose(index_file);

    
    indlist *ilist = data_to_mem(data_file, fileType);
    num_ind = ilist->size;
    arr = index_sort(ilist);
    mem_to_index(arquivoIndice, arr, fileType, num_ind);

    fseek(data_file, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, data_file); // Consistência
    fclose(data_file);

    binarioNaTela(arquivoEntrada);
    binarioNaTela(arquivoIndice);
    return 1;
}


// Funcionalidade [9]: ---
int create_index_btree(char *tipoArquivo, char* arquivoEntrada, int order) {
    int fileType = get_tipo_arquivo(tipoArquivo);
    if(fileType == 0) return 0; // Erro (tipo errado)

    char arquivoIndice[31];
    scanf("%s", arquivoIndice);
    b_tree *bt = create_btree(4, arquivoIndice); // Cria a árvore B
    node *root = create_node(order, '0', fileType); // Cria o nó raiz | tipo de nó igual a 0
    add_node_to_btree(bt, root); // Adiciona o nó raiz
    FILE *data_file = fopen(arquivoEntrada, "rb+");
    fwrite("0", sizeof(char), 1, data_file); // Consistência
    
    // Passa as informações para a memória e depois escreve-as num arquivo de índice
    
    indlist *ilist= data_to_mem(data_file, fileType);
    int num_ind = ilist->size;
    // Transfere os indices de lista para array
    _index **arr = (_index**)malloc((ilist->size) * sizeof(_index*));
    int i = 0;
    while(ilist->size > 0) {
        arr[i] = ilist->head;
        ilist->head = ilist->head->prox;
        ilist->size--;
        i++;
    }
    free(ilist);
    mem_to_btree(bt, arr, fileType, num_ind);
    
    // Finalizando processo
    fseek(data_file, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, data_file); // Consistência
    free(bt->root);
    free(bt);
    binarioNaTela(arquivoIndice);
    return 1;
}