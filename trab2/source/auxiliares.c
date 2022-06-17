/*
    SCC0215 - Organização de Arquivos (2022.1)
    Antônio Medrado - 10748389
    Lucas Sant'Anna - 10748521
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../headers/auxiliares.h"
#include "../headers/funcoesFornecidas.h"


// Função que completa o path dos arquivos a serem abertos
char *get_path(char *f){
    char *path = malloc(35*sizeof(char));
    char separator[3];
    char folder1[] = "arquivos";
    char folder2[] = "antes";
    #ifdef _WIN32 //se o sistema for windows
        separator[0] = 92; // '\' em valor númerico
        separator[1] = \0
    #elif __linux__
        strcpy(separator, "/"); //se for linux
    #elif __APPLE__
        strcpy(separator, ":"); //se for mac
    #endif
    // a final teremos (para linux): ./arquivos/antes/file
    strcpy(path, ".");
    strcat(path, separator);
    strcat(path, folder1);
    strcat(path, separator);
    strcat(path, folder2);
    strcat(path, separator);
    strcat(path, f);
    return path;
}

//dá um inteiro para arquivos de tipos distintos
int get_tipo_arquivo(char *tipoArquivo) {
    if(strncmp(tipoArquivo, "tipo1", 5) == 0) return 1;
    if(strncmp(tipoArquivo, "tipo2", 5) == 0) return 2;
    return 0; // Erro
}

// Para tratar os caracteres de lixo, '$'
// Insere caractere final na posição correta da string e retorna posição do caractere final
int tratamento_de_lixo(char *src, char *dest, int size) {
    assert(src != NULL); // Erro
    int i = 0;
    while(i < size) {
        dest[i] = src[i];
        if(dest[i] == '$') break;
        i++;
    }
    dest[i] = '\0';
    return 1;
}

// Cria instância de struct vehicle para guardar dados de um registro
vehicle *cria_veiculo() {
    vehicle *vh = (vehicle*)malloc(sizeof(vehicle));
    assert(vh != NULL);
    strncpy(vh->removido, "$", 1);
    vh->tamanhoRegistro = -1;
    vh->prox = -1;
    vh->id = -1;
    vh->ano = -1;
    vh->qtt = -1;
    strncpy(vh->sigla, "$$", 2);
    vh->tamCidade = -1;
    strncpy(vh->codC5, "$", 1);
    strncpy(vh->cidade, "$", 2);
    vh->tamMarca = -1;
    strncpy(vh->codC6, "$", 1);
    strncpy(vh->marca, "$", 2);
    vh->tamModelo = -1;
    strncpy(vh->codC7, "$", 1);
    strncpy(vh->modelo, "$", 2);
    return vh;
}

// Imprime as informações de um registro (já guardado numa struct) no formato pedido
void print_reg(vehicle *vh) {
    assert(vh != NULL);
    if(strncmp(vh->removido, "0", 1) == 0) { // Não exibir registros logicamente removidos
        printf("MARCA DO VEICULO: ");
        if(strncmp(vh->marca, "$", 1) != 0) printf("%s\n", vh->marca);
        else printf("NAO PREENCHIDO\n");

        printf("MODELO DO VEICULO: ");
        if(strncmp(vh->modelo, "$", 1) != 0) printf("%s\n", vh->modelo);
        else printf("NAO PREENCHIDO\n");

        printf("ANO DE FABRICACAO: ");
        if(vh->ano != -1) printf("%d\n", vh->ano);
        else printf("NAO PREENCHIDO\n");

        printf("NOME DA CIDADE: ");
        if(strncmp(vh->cidade, "$", 1) != 0) printf("%s\n", vh->cidade);
        else printf("NAO PREENCHIDO\n");

        printf("QUANTIDADE DE VEICULOS: ");
        if(vh->qtt != -1) printf("%d\n", vh->qtt);
        else printf("NAO PREENCHIDO\n");
    }
    printf("\n");
}

// Dado um registro, insere suas informações num struct para mais fácil manipulação
vehicle *reg_to_struct(FILE *f, int fileType) {
    long offset = ftell(f);
    char buffer[100];
    int aux;
    long laux;
    vehicle *vh = cria_veiculo();

    // removido
    fread(buffer, sizeof(char), 1, f);
    strncpy(vh->removido, buffer, 1);

    // tamanhoRegistro
    if(fileType == 2) {
        fread(&aux, sizeof(int), 1, f);
        vh->tamanhoRegistro = aux;
    } else {
        vh->tamanhoRegistro = 97;
    }

    // prox
    if(fileType == 1) {
        fread(&aux, sizeof(int), 1, f);
        laux = (long) aux;
    } else {
        fread(&laux, sizeof(long), 1, f);
    }
    vh->prox = laux;

    // id
    fread(&aux, sizeof(int), 1, f);
    vh->id = aux;

    // ano
    fread(&aux, sizeof(int), 1, f);
    vh->ano = aux;

    // qtt
    fread(&aux, sizeof(int), 1, f);
    vh->qtt = aux;

    // sigla
    fread(buffer, sizeof(char), 2, f);
    strncpy(vh->sigla, buffer, 2);

    // Nome da cidade, marca do veículo e modelo do veículo
    if(fileType == 1 || 31 <= vh->tamanhoRegistro + 5) {
        fread(&aux, sizeof(int), 1, f);
        fread(buffer, sizeof(char), 1, f);

        if(strncmp(buffer, "0", 1) == 0) { // Cidade
            strncpy(vh->codC5, buffer, 1); // codC5
            vh->tamCidade = aux; // tamCidade
            fread(buffer, sizeof(char), vh->tamCidade, f);
            tratamento_de_lixo(buffer, vh->cidade, vh->tamCidade); // cidade
            buffer[0] = '\0';
        
            if(fileType == 1 || 36 + vh->tamCidade <= vh->tamanhoRegistro + 5) {
                fread(&aux, sizeof(int), 1, f);
                fread(buffer, sizeof(char), 1, f);
            }
        }
        if(strncmp(buffer, "1", 1) == 0) { // Marca
            strncpy(vh->codC6, buffer, 1); // codC6
            vh->tamMarca = aux; // tamMarca
            fread(buffer, sizeof(char), vh->tamMarca, f);
            tratamento_de_lixo(buffer, vh->marca, vh->tamMarca); // marca
            buffer[0] = '\0';

            if(fileType == 1 || 41+vh->tamCidade+vh->tamMarca <= vh->tamanhoRegistro + 5) {
                fread(&aux, sizeof(int), 1, f);
                fread(buffer, sizeof(char), 1, f);
            }
        }
        if(strncmp(buffer, "2", 1) == 0) { // Modelo
            strncpy(vh->codC7, buffer, 1); // codC7
            vh->tamModelo = aux; // tamModelo
            fread(buffer, sizeof(char), vh->tamModelo, f);
            tratamento_de_lixo(buffer, vh->modelo, vh->tamModelo); //modelo
        }
        int i = fileType == 2 ? 5 : 0; // Ajuste caso seja do tipo2
        fseek(f, offset+vh->tamanhoRegistro+i, SEEK_SET);
    }
    return vh;
}

// Altera um campo específico de um veículo
int set_field(vehicle *vh, char *field_name, char *field_value) {
    assert(vh != NULL && field_name != NULL && field_value != NULL);
    if(strncmp(field_name, "removido", 8) == 0) {
        strncpy(vh->removido, field_value, 1);
    } else if(strncmp(field_name, "tamanhoRegistro", 15) == 0) {
        vh->tamanhoRegistro = atoi(field_value);
    } else if(strncmp(field_name, "prox", 4) == 0) {
        vh->prox = atol(field_value);
    } else if(strncmp(field_name, "id", 2) == 0) {
        vh->id = atoi(field_value);
    } else if(strncmp(field_name, "ano", 3) == 0) {
        vh->ano = atoi(field_value);
    } else if(strncmp(field_name, "qtt", 3) == 0) {
        vh->qtt = atoi(field_value);
    } else if(strncmp(field_name, "sigla", 5) == 0) {
        strncpy(vh->sigla, field_value, 2);
    } else if(strncmp(field_name, "tamCidade", 9) == 0) {
        vh->tamCidade = atoi(field_value);
    } else if(strncmp(field_name, "codC5", 5) == 0) {
        strncpy(vh->codC5, field_value, 1);
    } else if(strncmp(field_name, "cidade", 6) == 0) {
        strncpy(vh->cidade, field_value, strlen(field_value));
    } else if(strncmp(field_name, "tamMarca", 8) == 0) {
        vh->tamMarca = atoi(field_value);
    } else if(strncmp(field_name, "codC6", 5) == 0) {
        strncpy(vh->codC6, field_value, 1);
    } else if(strncmp(field_name, "marca", 5) == 0) {
        strncpy(vh->marca, &field_value[1], strlen(field_value));
    } else if(strncmp(field_name, "tamModelo", 9) == 0) {
        vh->tamModelo = atoi(field_value);
    } else if(strncmp(field_name, "codC7", 5) == 0) {
        strncpy(vh->codC7, field_value, 1);
    } else if(strncmp(field_name, "modelo", 6) == 0) {
        strncpy(vh->modelo, field_value, strlen(field_value));
    } else return 0; // Erro
    return 1;
}

// Compara conteúdo de certos campos de um registro, de acordo com um filtro também guardado como struct vehicle
int filter_cmp(vehicle *filter, vehicle *reg) {
    assert(filter != NULL && reg != NULL);
    int var;
    
    // Procura por não-correspondências para qualquer campo não-nulo do filtro
    if(strncmp(filter->removido, "$", 1) != 0 && strncmp(filter->removido, reg->removido, 1) != 0) return 0;
    if(filter->prox != -1 && filter->prox != reg->prox) return 0;
    if(filter->id != -1 && filter->id != reg->id) return 0;
    if(filter->ano != -1 && filter->ano != reg->ano) return 0;
    if(filter->qtt != -1 && filter->qtt != reg->qtt) return 0;
    if(strncmp(filter->sigla, "$$", 2) != 0 && strncmp(filter->sigla, reg->sigla, 2) != 0) return 0;
    if(filter->tamCidade != -1 && filter->tamCidade != reg->tamCidade) return 0;
    if(strncmp(filter->codC5, "$", 1) != 0 && strncmp(filter->codC5, reg->codC5, 1) != 0) return 0;
    if(strncmp(filter->cidade, "$", 1) != 0 && strncmp(filter->cidade, reg->cidade, strlen(filter->cidade)) != 0) return 0;
    if(filter->tamMarca != -1 && filter->tamMarca != reg->tamMarca) return 0;
    if(strncmp(filter->codC6, "$", 1) != 0 && strncmp(filter->codC6, reg->codC6, 1) != 0) return 0;
    if(strncmp(filter->marca, "$", 1) != 0 && strncmp(filter->marca, reg->marca, strlen(filter->marca)) != 0) return 0;
    if(filter->tamModelo != -1 && filter->tamModelo != reg->tamModelo) return 0;
    if(strncmp(filter->codC7, "$", 1) != 0 && strncmp(filter->codC7, reg->codC7, 1) != 0) return 0;
    if(strncmp(filter->modelo, "$", 1) != 0 && strncmp(filter->modelo, reg->modelo, strlen(filter->modelo)) != 0) return 0;

    return 1;
}

vehicle *field_to_struct() {
    vehicle *vh = cria_veiculo();
    char nomeCampo[31], valorCampo[51];
    int x; // Número de campos
    scanf("%d", &x);
    for(int i = 0; i < x; i++) {
        scanf("%s", nomeCampo);
        scan_quote_string(valorCampo);
        
        set_field(vh, nomeCampo, valorCampo);

        strcpy(nomeCampo, "");
        strcpy(valorCampo, "");
    }
    return vh;
}

// Cria cabeçalho para função de escrita
int create_header(FILE *output, int fileType) {
    long var_long = -1;
    int var = 0;
    if(fileType == 1) { // Registro de cabeçalho de 182 bytes
        fwrite("0", sizeof(char), 1, output); // status
        fwrite(&var_long, sizeof(int), 1, output); // topo
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
        fwrite(&var, sizeof(int), 1, output); // proxRRN
        fwrite(&var, sizeof(int), 1, output); // nroRegRem
    } else if(fileType == 2) { // Registro de cabeçalho de 190 bytes
        fwrite("0", sizeof(char), 1, output); // status
        fwrite(&var_long, sizeof(long int), 1, output); // topo
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
        var_long = 0;
        fwrite(&var_long, sizeof(long), 1, output); // proxByteOffset
        fwrite(&var, sizeof(int), 1, output); // nroRegRem
    } else return 0; // Erro
    return 1;
}

// Função auxiliar para parse_data(), isolando elementos das linhas no CSV, separando por vírgulas
char *get_token(char **ptr) {
    char token[35]; 
    int i = 0;
    while(1) { // Até encontrar ',' ou o fim da linha
        if((*ptr)[0] == ',' || (*ptr)[0] == '\r' || (*ptr)[0] == '\n') break;
        token[i] = (*ptr)[0]; // Lê caractere por caractere da string para qual *ptr aponta
        i++;
        (*ptr)++;
    }
    (*ptr)++; // Ao fim do laço *ptr apontará para um caractere que define sua parada
    // Então é necesário incrementar para que o ponteiro aponte um caractere válido
    
    token[i] = '\0'; // para indicar o fim da string que o token representa 
    char *p = token[0] == '\0' ? NULL : token; //se não há valor no token, retorna NULL
    return p;
}

// Separa todos os tokens de linha pré definida
vehicle *parse_data(char *line) {
    vehicle *vh = cria_veiculo(); // Dados retirados da linha serão armazenados na estrutura, inicialmente ela está vazia
    char num[10];
    char *token;
    char *ptr = line;
    token = get_token(&ptr); // ID
    strcpy(num, token);
    vh->id = atoi(num); //converte string para int

    token = get_token(&ptr); // ANO
    if(token != NULL){strcpy(num, token); vh->ano = atoi(num); }

    token = get_token(&ptr); // CIDADE
    if(token != NULL) {strcpy(vh->cidade, token); vh->tamCidade = strlen(token);}
    
    token = get_token(&ptr); // QUANTIDADE
    if(token != NULL){ strcpy(num,token); vh->qtt= atoi(num);} 

    token = get_token(&ptr); // SIGLA
    if(token != NULL){strcpy(vh->sigla,token);}

    token = get_token(&ptr); // MARCA
    if(token != NULL){strcpy(vh->marca,token); vh->tamMarca = strlen(token);}

    token = get_token(&ptr); // MODELO
    if(token != NULL){strcpy(vh->modelo,token); vh->tamModelo = strlen(token);}

    return vh;
}

// Calcula o tamanho total do registro sem considerar o lixo (no final do registro)
int useful_reg_length(vehicle *reg, int fileType) {
    int reg_length;
    if(fileType == 1) reg_length = 19;
    else if(fileType == 2) reg_length = 27;

    // Verifica se os campos de tamanho váriaveis existem
    if(reg->tamCidade != -1) reg_length = reg_length + reg->tamCidade + 5; // +5 relativo aos campos tamX e codCY
    if(reg->tamMarca != -1) reg_length = reg_length + reg->tamMarca + 5;
    if(reg->tamModelo != -1) reg_length = reg_length + reg->tamModelo + 5;
    return reg_length;
}

void write_reg(FILE *file, int fileType, vehicle *vh){

    int useful_length = useful_reg_length(vh, fileType);  // Calcula o tamanho do registro considerando os caracteres úteis
    int length_trash = fileType == 1 ? (97 - useful_length) : 0; // Tamanho do campo relativo ao lixo
    
    long byte_offset;
    int rrn;
    //Busca o próximo byte_offset (ou RRN) dispónivel 
    if(fileType == 1){
        fseek(file, 174, SEEK_SET);
        fread(&rrn, sizeof(int), 1, file);
        byte_offset = (((long)rrn) * 97) + 182;
    }else if(fileType == 2){
        fseek(file, 178, SEEK_SET);
        fread(&byte_offset, sizeof(long), 1, file);
        if(byte_offset == 0){ byte_offset = 190; }
    }
    fseek(file, byte_offset, SEEK_SET);//vai o próximo byte offset disponível 

    // ESCREVE NOS CAMPOS DE TAMANHO ÚTIL
    char c = '0';
    fwrite(&c, sizeof(char), 1, file); // STATUS
    if(fileType == 1) {
        int prox = (int) vh->prox;
        fwrite(&prox, sizeof(int), 1, file); // Prox removido
        fwrite(&vh->id, sizeof(int), 1, file); // ID
        fwrite(&vh->ano, sizeof(int), 1, file); // ANO
        fwrite(&vh->qtt, sizeof(int), 1, file); // QUANTIDADE
        fwrite(vh->sigla, sizeof(char), 2, file); // SIGLA
    } else if(fileType == 2) {
        vh->tamanhoRegistro = useful_length - 5;
        fwrite(&vh->tamanhoRegistro, sizeof(int), 1, file); // TAMANHO DO REGISTRO
        fwrite(&vh->prox, sizeof(long int), 1, file); // Prox removido
        fwrite(&vh->id, sizeof(int), 1, file); // ID
        fwrite(&vh->ano, sizeof(int), 1, file); // ANO
        fwrite(&vh->qtt, sizeof(int), 1, file); //QUANTIDADE
        fwrite(vh->sigla, sizeof(char), 2, file); // SIGLA
    }

    // ESCREVE NOS CAMPOS DE TAMANHO VARIAVEL 
    if(vh->tamCidade != -1) { 
        c = '0';
        fwrite(&vh->tamCidade, sizeof(int), 1, file); // TAMANHO CIDADE
        fwrite(&c, sizeof(char), 1, file); // 0
        fwrite(vh->cidade, sizeof(char), vh->tamCidade, file); // CIDADE
    }
    if(vh->tamMarca != -1) {
        c = '1';
        fwrite(&vh->tamMarca, sizeof(int), 1, file); // TAMANHO MARCA
        fwrite(&c, sizeof(char), 1, file); //1
        fwrite(vh->marca, sizeof(char), vh->tamMarca, file); // MARCA               
    }
    if(vh->tamModelo != -1) {
        c = '2';
        fwrite(&vh->tamModelo, sizeof(int), 1, file); // TAMANHO MODELO
        fwrite(&c, sizeof(char), 1, file); //2
        fwrite(vh->modelo, sizeof(char), vh->tamModelo, file); // MODELO
    }
    for(int i = 0; i < length_trash; i++) { // Para o tamanho do campo relativo ao lixo
        c = '$';
        fwrite(&c, sizeof(char), 1, file); // LIXO
    }

    // Atualiza o endereço para o próximo registro disponível para escrita 
    // rrn (tipo1) ou byte_off (tipo2)
    fseek(file, 0, SEEK_END);
    byte_offset = ftell(file);
    if(fileType == 1){ // tipo1
        rrn = (int)(byte_offset - 182) / 97;
        fseek(file, 174, SEEK_SET);
        fwrite(&rrn, sizeof(int), 1,  file);
    }else{ // tipo2
        fseek(file, 178, SEEK_SET);
        fwrite(&byte_offset, sizeof(long), 1 , file);
    }
    //fseek(file, 0, SEEK_END); // Retorna ao fim do arquivo para prosseguir com a escrita
}

// Lê do .csv e escreve os registros de dados no .bin
void data_reg(FILE *output_file, FILE *input_file, int fileType) {
    char line[200]; 
    fgets(line, 200,  input_file);
    while(fgets(line, 200, input_file) != NULL) { // Lê uma linha por vez até o fim do arquivo
        
        vehicle *vh = parse_data(line); // Separa os dados da linha, armazena-os em um estrutura de dados
        write_reg(output_file, fileType, vh);
    }
    //Antes de fechar o arquivo, atualiza o status para não comprometido (consistente)
    fseek(output_file, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, output_file);
}