#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliares.h"

/*
--------------------------------------------------------------------------------------------------------------
Lista de Funções do arquivo:
*Funções não incluidas no .h (para uso interno do arquivo):
    get_token -> pega o próximo token até encontrar ',' ou o final da linha
    separate_data -> a parti de uma string com dados dividos por ',' e os separa retornando uma estrutura
    calc_reg_lenght ->  retorna o tamanho do registro baseado no tipo de documento e nos dados a serem armazenados

*Funções incluidas no .h:
    data_reg -> lê dados de um arquivo .csv e cria registros de dados em um arquivo .bin

--------------------------------------------------------------------------------------------------------------
*/

//Estrutura de dados que guarda os valores a serem armazenados nos registros de dados
struct File_data {
    int id, year, quant;
    char city[30], sg[3], brand[30], model[30];
    int city_l, brand_l, model_l; // o tamanho das strings correspondentes aos campos variáveis
} ;

char *get_token(char **ptr) { //devolve um token por vez - veja a função separate_data
    char token[35]; 
    int i = 0;
    while (1){
        //ate encontrar ',' ou o fim da linha
        if( (*ptr)[0] == ',' || (*ptr)[0] == '\r' || (*ptr)[0] == '\n') { break; }
        token[i] = (*ptr)[0]; //lê caractere por caractere da string para qual *ptr aponta
        i++;
        (*ptr)++;
    }
    (*ptr)++; //ao fim do laço *ptr apontará para um caractere que define sua parada
    //então é necesário incrementar para que o ponteiro aponte um caractere válido
    
    token[i] = '\0'; // para indicar o fim da string que o token representa 
    char *p = token[0] == '\0' ? NULL : token; //se não há valor no token, retorna NULL
    return p;
}

struct File_data separate_data(char line[]){ //Separa todos os tokens de linha pré definida
    struct File_data d; // os dados retirados da linha serão armazenados na estrutura
    char num[10];
    char *token;
    char *ptr = line;
    
    token = get_token(&ptr); // ID
    strcpy(num, token);
    d.id = atoi(num); //converte string para int

    token = get_token(&ptr); // ANO
    if(token != NULL){strcpy(num, token); d.year = atoi(num);} 
    else {d.year = -1;} // se o campo de tipo int não existe, será armazenado o valor -1
    
    token = get_token(&ptr); // CIDADE
    if(token != NULL){strcpy(d.city, token); d.city_l = strlen(token);}
    else {strcpy(d.city, ""); d.city_l = 0;} //se o campo de tipo string não existir
    //tamanho da string é 0

    token = get_token(&ptr); // QUANTIDADE
    if(token != NULL){strcpy(num,token); d.quant = atoi(num);}
    else {d.quant = -1;}

    token = get_token(&ptr); // SIGLA
    if(token != NULL){strcpy(d.sg,token);} else {strcpy(d.sg, "");}

    token = get_token(&ptr); // MARCA
    if(token != NULL){strcpy(d.brand,token); d.brand_l = strlen(token);}
    else {strcpy(d.brand, ""); d.brand_l = 0;}

    token = get_token(&ptr); // MODELO
    if(token != NULL){strcpy(d.model,token); d.model_l = strlen(token);}
    else {strcpy(d.model, ""); d.model_l = 0;}
    /*
    printf("id:%d\n", d.id);
    printf("Ano de Fabricação: %d\n", d.year);
    printf("Cidade: %s - %s\n", d.city, d.sg);
    printf("Quantidade: %d\n", d.quant);
    printf("Marca: %s\n", d.brand);
    printf("Modelo: %s\n", d.model);
    printf("\n ------------------- \n");
    */    
    return d;
}

int calc_reg_length(struct File_data reg, char *fileType){ //calcula o tamanho total do registro
    if(strncmp(fileType, "tipo1",5) == 0){return 97;} // registro de tamanho fixo
    int length = 27 + reg.city_l + reg.brand_l + reg.model_l ;
    
    //verifica se os campos de tamanho váriaveis existem
    if(reg.city_l != 0){length = length + 5;}
    if(reg.brand_l != 0){length = length + 5;}
    if(reg.model_l != 0){length = length + 5;}
    return length;
}

void data_reg(FILE *input_file, FILE *output_file, char *fileType){ //lê do .csv e escreve os registros de dados no .bin
    char line[100]; 
    int header = strncmp(fileType, "tipo1", 5) == 0 ? 182 : 190 ; //define o tamnho do cabeçalho
    int rrn = header;
    while(fgets(line, 100, input_file) != NULL) //lê uma linha por vez até o fim do arquivo
    {   
        struct File_data d = separate_data(line); // separa os dados da linha, armazena-os em um estrutura de dados
        int reg_length = calc_reg_length(d, fileType);  // cálcula o tamanho do registro 

        //Escrevendo os campos de tamanho fixo no arquivo binário
        fwrite("1", sizeof(char), 1, output_file); // STATUS

        if(strncmp(fileType, "tipo2", 5) == 0){ fwrite(&reg_length, sizeof(int), 1, output_file); } // TAMANHO DO REGISTRO
        
        int next_rrn = rrn + reg_length - 1; 
        fwrite(&next_rrn, sizeof(int), 1, output_file); // PRÓXIMO RRN

        fwrite(&d.id, sizeof(int), 1, output_file); // ID

        fwrite(&d.year, sizeof(int), 1, output_file); // ANO

        fwrite(&d.quant, sizeof(int), 1, output_file); //QUANTIDADE

        if(strcmp(d.sg,"") == 0){fwrite("$$", sizeof(char), 2, output_file);} // SIGLA
        else{fwrite(d.sg, sizeof(char), 2, output_file);}

        //Escreve os campos de tamanho váriavel no arquivo binário
        if(d.city_l != 0){
            fwrite(d.city_l, sizeof(int), 1, output_file);
            fwrite("0", sizeof(char), 1, output_file);
            fwrite(d.city, sizeof(char), d.city_l, output_file);
        }
            
        if(d.brand_l != 0){
            fwrite(d.brand_l, sizeof(int), 1, output_file);
            fwrite("1", sizeof(char), 1, output_file);
            fwrite(d.brand, sizeof(char), d.brand_l, output_file);                
        }

        if(d.model_l != 0){
            fwrite(d.model, sizeof(int), 1, output_file);
            fwrite("2", sizeof(char), 1, output_file);
            fwrite(d.model, sizeof(char), d.model_l, output_file);
        }

        if(strncmp(fileType, "tipo1", 5) == 0){
            //colocar os $ no final do registro caso seja o do tipo fixo
        }
        rrn = next_rrn;
    }
}


    
