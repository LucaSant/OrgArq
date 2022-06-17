/*
    SCC0215 - Organização de Arquivos (2022.1)
    Antônio Medrado - 10748389
    Lucas Sant'Anna - 10748521
*/

#include <stdio.h>
#include "../headers/structs.h"

// Funções auxiliares, que serão usadas pelas funcionalidades
char *get_path(char *file);
int get_tipo_arquivo(char *tipoArquivo);
int tratamento_de_lixo(char *src, char *dest, int size);
vehicle *cria_veiculo();
void print_reg(vehicle *vh);
vehicle *reg_to_struct(FILE *f, int fileType);
int set_field(vehicle *vh, char *field_name, char *field_value);
vehicle *field_to_struct();
int filter_cmp(vehicle *filter, vehicle *reg);
int struct_match(vehicle *est1, vehicle *est2);
int create_header(FILE *output, int fileType);
char *get_token(char **ptr);
vehicle *parse_data(char *line);
int useful_reg_length(vehicle *reg, int fileType);
void data_reg(FILE *input_file, FILE *output_file, int fileType);