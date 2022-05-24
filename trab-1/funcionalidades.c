#include "funcionalidades.h"
#include "auxiliares.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int createTable(char *output_filename, char *input_filename, char *fileType){ //Funcionalidade 1
    FILE *input_file = fopen(input_filename, "r"); // abre arquivo .csv para leitura
    FILE *output_file = fopen(output_filename, "ab+"); // abre/cria arquivo .bin para leitura/escrita (no final)
    //..
    //inserir cabeçalho no .bin
    //..

    data_reg(output_file, input_file, fileType); // função que lê do .csv e  escreve os registros de dados
    fclose(input_file); //fechando arquivos
    fclose(output_file);
    return 1;
}

int select(char *fileType, char *input_filename){

}

int select_where(char *fileType, char *output_filename){
    int n; // quantidade de vezes o nome do campo e o valor do campo serão lidos (número de filtros)
    scanf("%d", &n);
    sfv filter[n]; //cria um vetor de estrutura que guarda informações dos filtros

    for(int i = 0; i < n; i++){
        scanf("%s", filter[i].field_name);
        scanf("%[^\n]s", filter[i].string_value); 

        if(filter[i].string_value[0] == '\"' && filter[i].string_value[strlen(filter[i].string_value) - 1] == '\"'){ // strings devem ser especificadas entre aspas
            filter[i].is_str = 1; // o valor do campo é uma string
            filter[i].int_value = -1; //não há valor inteiro
        }else{
            filter[i].is_str = 0;
            filter[i].int_value = atoi(filter[i].string_value); //o valor do campo é um inteiro e deve ser convertido
        }
    }

    FILE *output_file = fopen(output_filename, "ab+");

    /*
    //passar por todos os registros e imprimir aqueles que conterem todos os fitros
    */

    fclose(output_file);
}

int select_rrn(char *fileType, char *output_filename){



}

