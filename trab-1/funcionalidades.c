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

int select_where(char *fileType, char *input_filename){

}

int select_rrn(char *fileType, char *input_filename){

}

