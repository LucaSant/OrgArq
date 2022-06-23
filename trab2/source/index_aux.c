/*
    SCC0215 - Organização de Arquivos (2022.1)
    Antônio Medrado - 10748389
    Lucas Sant'Anna - 10748521
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/index_aux.h"

/*
void write_index(FILE *index_file, int fileType, table *idx) {
    fwrite(&idx->id, sizeof(int), 1, index_file);
    if(fileType == 1){
        fwrite(&idx->rrn, sizeof(int), 1, index_file);
    }else{
        fwrite(&idx->byte_offset, sizeof(long), 1, index_file);
    }
}
*/
/*
table *reg_to_table(FILE *data_file, int fileType) {
    table *idx = (table*)malloc(sizeof(table));
    int tam, aux; //tamanho: define tamanho do registro | aux: variável auxiliar para leitura do arquivo
    char rem; // rem: variável para ler campo removido do registro
    long byte_offset = ftell(data_file);
    fread(&rem, sizeof(char), 1, data_file);

    if(rem == '0'){
        if(fileType == 1) {
            int rrn = (int)((byte_offset - 182) / 97);
            idx->rrn = rrn;
            tam = 97;
            fseek(data_file, 4, SEEK_CUR);
        } else {
            idx->byte_offset = byte_offset;
            fread(&tam, sizeof(int), 1, data_file);
            fseek(data_file, 8, SEEK_CUR);
            tam = tam + 5;
        }
        fread(&aux, sizeof(int), 1, data_file);
        idx->id = aux;
        byte_offset = byte_offset + (long)(tam);
        fseek(data_file, byte_offset, SEEK_SET);
        return idx;
    } else {
        free(idx);
        return NULL;
    }
}
*/