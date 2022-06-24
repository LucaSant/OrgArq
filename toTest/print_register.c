#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct vehicle {
    char removido[1];
    int tamanhoRegistro;
    long prox;
    int id;
    int ano;
    int qtt;
    char sigla[2];
    int tamCidade;
    char codC5[1];
    char cidade[50];
    int tamMarca;
    char codC6[1];
    char marca[50];
    int tamModelo;
    char codC7[1];
    char modelo[50];
} vehicle;


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

int main(){
    
    
    int topo1, next1, nRR, prox, id, ano, qtt, tamCidade, tamMarca, tamModelo;
    char status, removido, cod5, cod6, cod7, teste;
    char sigla[2];
    char cidade[50];
    char marca[50];
    char modelo[50];
    long cur, topo2, next2;
    char code;

    int fileType = 2;

    long final;
    FILE *data_file = fopen("binario6.bin", "rb+");
    fseek(data_file, 1, SEEK_SET);
    if(fileType == 1){
        fread(&topo1, sizeof(int), 1, data_file);
        long offset = (long) ((topo1 * 97) - 182);
        printf("topo: rrn %d --- offset %ld\n", topo1, offset);
        fseek(data_file, 174, SEEK_SET);
        fread(&next1, sizeof(int), 1, data_file);
        offset = (long) ((next1 * 97) - 182);
        printf("prox: rrn %d --- offset %ld\n", next1, offset);
        fseek(data_file, 178, SEEK_SET);
        fread(&nRR, sizeof(int), 1, data_file);
        printf("número de reg. removidos: %d\n\n", nRR);
    } else {
        fread(&topo2, sizeof(long), 1, data_file);
        printf("topo: %ld\n", topo2);
        fseek(data_file, 178, SEEK_SET);
        fread(&next2, sizeof(long), 1, data_file);
        printf("prox: %ld\n", next2);
        fseek(data_file, 186, SEEK_SET);
        fread(&nRR, sizeof(int), 1, data_file);
        printf("número de reg. removidos: %d\n\n", nRR);
    }


    long addr;
    scanf("%ld",&addr);
    fseek(data_file, addr , SEEK_SET);
    vehicle *vh = reg_to_struct(data_file, fileType);
    printf("removido: %c\n", vh->removido[0]);
    if(fileType == 2) printf("tamReg: %d\n", vh->tamanhoRegistro);
    printf("proximo:");
    printf("%ld\n", vh->prox);
    if(strncmp(vh->removido, "0", 1) == 0) { // Não exibir registros logicamente removidos
        printf("não removido\n");

        printf("id: %d\n", vh->id);
        printf("ANO DE FABRICACAO: ");
        if(vh->ano != -1) printf("%d\n", vh->ano);
        printf("QUANTIDADE DE VEICULOS: ");
        if(vh->qtt != -1) printf("%d\n", vh->qtt);
        if(strncmp(vh->sigla, "$$", 2) != 0) printf("sigla: %c%c\n", vh->sigla[0], vh->sigla[1]);
        if(vh->tamCidade != -1) printf("tamanho cidade: %d\n", vh->tamCidade);
        if(vh->codC5[0] == '0') printf("codC5: %c\n", vh->codC5[0]);
        printf("NOME DA CIDADE: ");
        if(strncmp(vh->cidade, "$", 1) != 0) printf("%s\n", vh->cidade);    
        if(vh->tamMarca != -1) printf("tamanho marca: %d\n", vh->tamMarca);
        if(vh->codC6[0] == '1') printf("codC6: %c\n", vh->codC6[0]);        
        printf("MARCA DO VEICULO: ");
        if(strncmp(vh->marca, "$", 1) != 0) printf("%s\n", vh->marca);
        if(vh->tamModelo != -1) printf("tamanho modelo: %d\n", vh->tamModelo);
        if(vh->codC7[0] == '2') printf("codC7: %c\n", vh->codC7[0]); 
        printf("MODELO DO VEICULO: ");
        if(strncmp(vh->modelo, "$", 1) != 0) printf("%s\n", vh->modelo);
    }

    free(vh);
    fclose(data_file);
    
    return 0;
}
