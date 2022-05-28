#include <stdio.h>
/*
    Funções que serão usadas pelas funcionalidades

*/
typedef struct search_filters_vector{
    char field_name[15];
    int is_str;
    char string_value[30];
    char int_value;
}sfv;

struct File_data separate_data(char line[]);
void data_reg(FILE *input_file, FILE *output_file, char *fileType);