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
#include "../headers/index_aux.h"
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

// Calcula o tamanho total do registro sem considerar o lixo (no final do registro)
int useful_reg_length(vehicle *reg, int fileType) {
    int reg_length = fileType == 1 ? 19 : 27;

    // Verifica se os campos de tamanho váriaveis existem
    if(reg->tamCidade != -1) reg_length = reg_length + reg->tamCidade + 5; // +5 relativo aos campos tamX e codCY
    if(reg->tamMarca != -1) reg_length = reg_length + reg->tamMarca + 5;
    if(reg->tamModelo != -1) reg_length = reg_length + reg->tamModelo + 5;
    return reg_length;
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
    int nulo = strncmp(field_value, "NULO", 4) == 0 ? 1 : 0;

    if(strncmp(field_name, "removido", 8) == 0) {
        if(nulo) strncpy(vh->removido, "#", 1);
        else strncpy(vh->removido, field_value, 1);

    } else if(strncmp(field_name, "tamanhoRegistro", 15) == 0) {
        if(nulo) vh->tamanhoRegistro = -2;
        else vh->tamanhoRegistro = atoi(field_value);

    } else if(strncmp(field_name, "prox", 4) == 0) {
        if(nulo) vh->prox = -2;
        else vh->prox = atol(field_value);

    } else if(strncmp(field_name, "id", 2) == 0) {
        if(nulo) vh->id = -2;
        else vh->id = atoi(field_value);

    } else if(strncmp(field_name, "ano", 3) == 0) {
        if(nulo) vh->ano = -2;
        else vh->ano = atoi(field_value);

    } else if(strncmp(field_name, "qtt", 3) == 0) {
        if(nulo) vh->qtt = -2;
        else vh->qtt = atoi(field_value);

    } else if(strncmp(field_name, "sigla", 5) == 0) {
       if(nulo) strncpy(vh->sigla, "#", 1);
        else strncpy(vh->sigla, field_value, 2);

    } else if(strncmp(field_name, "tamCidade", 9) == 0) {
        if(nulo) vh->tamCidade = -2;
        else vh->tamCidade = atoi(field_value);

    } else if(strncmp(field_name, "codC5", 5) == 0) {
        if(nulo) strncpy(vh->codC5, "#", 1);
        else strncpy(vh->codC5, field_value, 1);

    } else if(strncmp(field_name, "cidade", 6) == 0) {
        if(nulo) strncpy(vh->cidade, "#", 1);
        else strncpy(vh->cidade, field_value, strlen(field_value));

    } else if(strncmp(field_name, "tamMarca", 8) == 0) {
        if(nulo) vh->tamMarca = -2;
        else vh->tamMarca = atoi(field_value);

    } else if(strncmp(field_name, "codC6", 5) == 0) {
        if(nulo) strncpy(vh->codC6, "#", 1);
        else strncpy(vh->codC6, field_value, 1);

    } else if(strncmp(field_name, "marca", 5) == 0) {
        if(nulo) strncpy(vh->marca, "#", 1);
        else strncpy(vh->marca, field_value, strlen(field_value));

    } else if(strncmp(field_name, "tamModelo", 9) == 0) {
        if(nulo) vh->tamModelo = -2;
        else vh->tamModelo = atoi(field_value);

    } else if(strncmp(field_name, "codC7", 5) == 0) {
        if(nulo) strncpy(vh->codC7, "#", 1);
        else strncpy(vh->codC7, field_value, 1);

    } else if(strncmp(field_name, "modelo", 6) == 0) {
        if(nulo) strncpy(vh->modelo, "#", 1);
        else strncpy(vh->modelo, field_value, strlen(field_value));

    } else return 0; // Erro
    return 1;
}

// Cria um struct veículo com informações do input de acordo com funcionalidade [7]
vehicle *vh_from_input() {
    char field_name[31], field_value[31];
    vehicle *vh = cria_veiculo();
    scan_quote_string(field_value);
    if(strncmp(field_value, "NULO", 4) != 0) {
        strcpy(field_name, "id");
        set_field(vh, field_name, field_value);
    }     
    
    scan_quote_string(field_value);
    if(strncmp(field_value, "NULO", 4) != 0) {
        strcpy(field_name, "ano");
        set_field(vh, field_name, field_value);
    }  

    scan_quote_string(field_value);
    if(strncmp(field_value, "NULO", 4) != 0) {
        strcpy(field_name, "qtt");
        set_field(vh, field_name, field_value);
    } 

    scan_quote_string(field_value);
    if(strncmp(field_value, "NULO", 4) != 0) {
        strcpy(field_name, "sigla");
        set_field(vh, field_name, field_value);
    } 

    scan_quote_string(field_value);
    strcpy(field_name, "cidade");
    set_field(vh, field_name, field_value);
    if(strcmp(vh->cidade, "$")  != 0 && strcmp(vh->cidade, "#") != 0){
        vh->tamCidade = (int) strlen(field_value);
        set_field(vh, "codC5", "0");
    }

    scan_quote_string(field_value);
    strcpy(field_name, "marca");
    set_field(vh, field_name, field_value);
    if(strcmp(vh->marca, "$")  != 0 && strcmp(vh->marca, "#") != 0){
        vh->tamMarca = (int) strlen(field_value);
        set_field(vh, "codC6", "1");
    }
    
    scan_quote_string(field_value);
    strcpy(field_name, "modelo");
    set_field(vh, field_name, field_value);
    if(strcmp(vh->modelo, "$")  != 0 && strcmp(vh->modelo, "#") != 0){
        vh->tamModelo = (int) strlen(field_value);
        set_field(vh, "codC7", "2");
    }
    
    return vh;
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

// Atualiza o conteúdo de um struct veículo com base num struct de valores atualizados
int update_vehicle(vehicle *vh, vehicle *update, int fileType) {
    assert(vh != NULL && update != NULL);

    // Faz as devidas alterações

    if(strncmp(update->removido, "$", 1) != 0) {
        if(strncmp(update->removido, "#", 1) == 0) strncpy(vh->removido, "$", 1);
        else strncpy(vh->removido, update->removido, 1);
    }
    if(update->prox != -1) {
        if(update->prox == -2) vh->prox = -1;
        else vh->prox = update->prox;
    }
    if(update->id != -1) {
        if(update->id == -2) vh->id = -1;
        else vh->id = update->id;
    }
    if(update->ano != -1) {
        if(update->ano == -2) vh->ano = -1;
        else vh->ano = update->ano;
    }
    if(update->qtt != -1) {
        if(update->qtt == -2) vh->qtt = -1;
        else vh->qtt = update->qtt;
    }
    if(strncmp(update->sigla, "$$", 2) != 0) {
        if(strncmp(update->sigla, "#", 1) == 0) strncpy(vh->sigla, "$$", 2);
        else strncpy(vh->sigla, update->sigla, 2);
    }
    if(update->tamCidade != -1) {
        if(update->tamCidade == -2) vh->tamCidade = -1;
        else vh->tamCidade = update->tamCidade;
    }
    if(strncmp(update->codC5, "$", 1) != 0) {
        if(strncmp(update->codC5, "#", 1) == 0) strncpy(vh->codC5, "$", 1);
        else strncpy(vh->codC5, update->codC5, 1);
    }
    if(strncmp(update->cidade, "$", 1) != 0) {
        if(strncmp(update->cidade, "#", 1) == 0) strncpy(vh->cidade, "$", 1);
        else strncpy(vh->cidade, update->cidade, strlen(update->cidade));
    }
    if(update->tamMarca != -1) {
        if(update->tamMarca == -2) vh->tamMarca = -1;
        else vh->tamMarca = update->tamMarca;
    }
    if(strncmp(update->codC6, "$", 1) != 0) {
        if(strncmp(update->codC6, "#", 1) == 0) strncpy(vh->codC6, "$", 1);
        else strncpy(vh->codC6, update->codC6, 1);
    }
    if(strncmp(update->marca, "$", 1) != 0) {
        if(strncmp(update->marca, "#", 1) == 0) strncpy(vh->marca, "$", 1);
        else strncpy(vh->marca, update->marca, strlen(update->marca));
    }
    if(update->tamModelo != -1) {
        if(update->tamModelo == -2) vh->tamModelo = -1;
        else vh->tamModelo = update->tamModelo;
    }
    if(strncmp(update->codC7, "$", 1) != 0) {
        if(strncmp(update->codC7, "#", 1) == 0) strncpy(vh->codC7, "$", 1);
        else strncpy(vh->codC7, update->codC7, 1);
    }
    if(strncmp(update->modelo, "$", 1) != 0) {
        if(strncmp(update->modelo, "#", 1) == 0) strncpy(vh->modelo, "$", 1);
        else strncpy(vh->modelo, update->modelo, strlen(update->modelo));
    }

    // Conserta quaisquer inconsistências
    if(strncmp(vh->cidade, "$", 1) == 0) {
        strncpy(vh->codC5, "$", 1);
        vh->tamCidade = -1;
    } else {
        strncpy(vh->codC5, "0", 1);
        vh->tamCidade = strlen(vh->cidade);
    }
    if(strncmp(vh->marca, "$", 1) == 0) {
        strncpy(vh->codC6, "$", 1);
        vh->tamMarca = -1;
    } else {
        strncpy(vh->codC6, "1", 1);
        vh->tamMarca = strlen(vh->marca);
    }
    if(strncmp(vh->modelo, "$", 1) == 0) {
        strncpy(vh->codC7, "$", 1);
        vh->tamModelo = -1;
    } else {
        strncpy(vh->codC7, "2", 1);
        vh->tamModelo = strlen(vh->modelo);
    }
    int len = useful_reg_length(vh, fileType);
    if(fileType == 1 && len > 97) return 0; // Erro (tamanho maior que possível)
    vh->tamanhoRegistro = fileType == 1 ? 97 : len;
    
    return 1;
}

// Cria um struct veículo e preenche-o com base no input em uma linha
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

// Procura posição na pilha de removidos para o "novo registro removido".
long find_removed_stack_position(FILE *data, long regOffset, int regSize) {
    long address, prevAddress;
    int size;
    fseek(data, 1, SEEK_SET);
    fread(&address, sizeof(long), 1, data);
    if(address != -1) {
        do {
            prevAddress = address;
            fseek(data, address+1, SEEK_SET);
            fread(&size, sizeof(int), 1, data);
            fread(&address, sizeof(long), 1, data);
        } while(address != -1 && size > regSize);
        return prevAddress; // Reg. imediatamente superior à posição do "novo registro"
    } else return -1; // Pilha vazia
}

// Remove um registro de um arquivo de dados e de índice
int rem_register(FILE *data, FILE *index, int fileType, long offset) {
    int aux;

    // Remove logicamente registro no arquivo de dados e atualiza a pilha de removidos
    fwrite("1", sizeof(char), 1, data);
    if(fileType == 1) {
        int rrn = (int) ((offset - 182) / 97);

        // Vai ao cabeçalho, guarda topo da pilha e atualiza-o
        fseek(data, 1, SEEK_SET);
        fread(&aux, sizeof(int), 1, data); // Antigo topo
        fseek(data, 1, SEEK_SET);
        fwrite(&rrn, sizeof(int), 1, data);

        // Próximo RRN no registro em questão é o antigo topo
        fseek(data, 182 + (97 * rrn) + 1, SEEK_SET);
        fwrite(&aux, sizeof(int), 1, data);

    } else if(fileType == 2) {
        fread(&aux, sizeof(int), 1, data); // Tamanho do registro

        // Pega a posição do registro imediatamente anterior ao reg. em questão na pilha
        long position = find_removed_stack_position(data, offset, aux);
        if(position == -1) { // Reg. em questão vai no topo da pilha
            fseek(data, 1, SEEK_SET);
        } else {
            fseek(data, position+5, SEEK_SET);
        }

        // Guarda posição do registro imediatamente posterior ao reg. em questão na pilha
        fread(&position, sizeof(long), 1, data);
        // Insere reg. em questão na pilha de removidos
        fseek(data, -8, SEEK_CUR);
        fwrite(&offset, sizeof(long), 1, data);
        fseek(data, offset+5, SEEK_SET);
        fwrite(&position, sizeof(long), 1, data); // -1 (fim) ou offset do próximo registro removido
    }

    // Atualiza o arquivo de índice
    //...
    return 1;
}

// Procura posição na pilha de removidos para adicionar o novo registro
long find_added_stack_position(FILE *data, int tamReg, int fileType) {
    int size;
    fseek(data, 1, SEEK_SET);
    if(fileType == 1) {
        int rrn;
        fread(&rrn, sizeof(int), 1, data);
        if(rrn == -1){
            fseek(data, 174, SEEK_SET);
            fread(&rrn, sizeof(int), 1, data);
        }
        return (long) (rrn);
    } else if(fileType == 2) {
        long offset;
        fread(&offset, sizeof(long), 1, data);
        if(offset == -1){
            fseek(data, 178, SEEK_SET);
            fread(&offset, sizeof(long), 1, data);
            return offset;
        } else {
            fseek(data, (offset + 1), SEEK_SET); // Vai até o campo tamanho do registro (campo do registro removido)
            fread(&size, sizeof(int), 1, data);
            if(size >= tamReg){ // Se o registro novo couber no espaço
                return offset; //retorna o byte offset
            } else { // Caso contrário, retorna o byte offset do fim do arquivo
                fseek(data, 178, SEEK_SET);
                fread(&offset, sizeof(long), 1, data);
                return offset;
            }
        }
    }
}

// Adiciona um registro em um arquivo de dados (no offset indicado) e no arquivo de índice
int add_register(vehicle *vh, FILE *data, FILE *index, int fileType, long offset) {
    int size = 97; // Tamanho do registro tipo1. Caso tipo2, valor será sobrescrito
    int rm_rrn; // Proximo rrn do registro removido 
    long rm_offset; // Proximo byte offset do registro removido
    fseek(data, 0, SEEK_END);
    long eof = ftell(data);
    int rrn;
    if(eof != offset) fseek(data, offset, SEEK_SET); 

    // Escreve registro no offset indicado
    fwrite("0", sizeof(char), 1, data); // removido


    // Essa seção do registro varia a depender to tipo de arquivo
    if(fileType == 1) {
        int aux = (int) vh->prox;
        if(eof != offset) { // Se for escrever em um registro removido X
            fread(&rm_rrn, sizeof(int), 1, data); // Lê o próximo registro removido de X
            fseek(data, (offset + 1), SEEK_SET);
        }
        fwrite(&aux, sizeof(int), 1, data); // prox      
    } else if(fileType == 2) {
        if(eof != offset) { // Estará sobrescrevendo um registro removido
            fread(&size, sizeof(int), 1, data); // Tamanho do antigo registro
            fread(&rm_offset, sizeof(long), 1, data); // Lẽ o endereço do próximo registro de removido
            fseek(data, (offset + 1), SEEK_SET); // Volta os campos tamanho e próximo byte offset para sobrescrever
        } else { // Insere no fim do arquivo
            size = vh->tamanhoRegistro - 5; // Insere no final do arquivo
        }

        //int aux = (int) vh->tamanhoRegistro;
        fwrite(&size, sizeof(int), 1, data); // tamanhoRegistro
        fwrite(&vh->prox, sizeof(long), 1, data); // prox
    }

    // Comportamento comum aos dois tipos de arquivo
    fwrite(&vh->id, sizeof(int), 1, data); // id
    fwrite(&vh->ano, sizeof(int), 1, data); // ano
    fwrite(&vh->qtt, sizeof(int), 1, data); // qtt
    fwrite(&vh->sigla, sizeof(char), 2, data); // sigla
    if(vh->tamCidade > 0) {
        fwrite(&vh->tamCidade, sizeof(int), 1, data); // tamCidade
        fwrite(&vh->codC5, sizeof(char), 1, data); // codC5
        fwrite(&vh->cidade, sizeof(char), vh->tamCidade, data); // cidade
    }
    if(vh->tamMarca > 0) {
        fwrite(&vh->tamMarca, sizeof(int), 1, data); // tamMarca
        fwrite(&vh->codC6, sizeof(char), 1, data); // codC6
        fwrite(&vh->marca, sizeof(char), vh->tamMarca, data); // marca
    }
    if(vh->tamModelo > 0) {
        fwrite(&vh->tamModelo, sizeof(int), 1, data); // tamModelo
        fwrite(&vh->codC7, sizeof(char), 1, data); // codC7
        fwrite(&vh->modelo, sizeof(char), vh->tamModelo, data); // modelo
    }
    if(fileType == 2) size += 5;
    // Adiciona lixo ao fim do registro, se necessário
    for(long i = ftell(data); i < offset + size; i++) {
        fwrite("$", sizeof(char), 1, data);
    }

    // Atualiza o cabeçalho, se necessário
    if(eof == offset) { // Caso 1: adicionado no fim do arquivo
        long next_offset = ftell(data); // Fim do arquivo
        if(fileType == 1){
            int next_rrn = (int) ((next_offset - 182) / 97);
            fseek(data, 174, SEEK_SET);
            fwrite(&next_rrn, sizeof(int), 1, data);
        } else {
            fseek(data, 178, SEEK_SET);
            fwrite(&next_offset, sizeof(long), 1, data);
        }
    } else { // Caso 2: adicionado no espaço de um registro removido
        int num_rm; // Número de registros removidos
        if(fileType == 1){ 
            fseek(data, 1, SEEK_SET);
            fwrite(&rm_rrn, sizeof(int), 1, data);
            fseek(data, 178, SEEK_SET);
            fread(&num_rm, sizeof(int), 1, data); // Lê o número de registros removidos
            num_rm--;
            fseek(data, -4, SEEK_CUR);
            fwrite(&num_rm, sizeof(int), 1, data);
        } else {
            fseek(data, 1, SEEK_SET);
            fwrite(&rm_offset, sizeof(long), 1, data);
            fseek(data, 186, SEEK_SET);
            fread(&num_rm, sizeof(int), 1, data);
            num_rm--;
            fseek(data, -4, SEEK_CUR);
            fwrite(&num_rm, sizeof(int), 1, data);
        }
    }

    // Atualiza o arquivo de índice
    //...
    
    return 1;
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
    } else if(fileType == 2) {
        fseek(file, 178, SEEK_SET);
        fread(&byte_offset, sizeof(long), 1, file);
        if(byte_offset == 0) byte_offset = 190;
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