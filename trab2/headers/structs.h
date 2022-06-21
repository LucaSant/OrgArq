/*
    SCC0215 - Organização de Arquivos (2022.1)
    Antônio Medrado - 10748389
    Lucas Sant'Anna - 10748521
*/

#ifndef STRUCTS_H
#define STRUCTS_H

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

typedef struct table {
    int id;
    int rrn;
    long byte_offset;
} table;

#endif //STRUCTS_H