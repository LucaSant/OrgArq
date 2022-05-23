#include <stdio.h>
/*
    Funções que serão usadas pelas funcionalidades

*/


struct File_data separate_data(char line[]);
void data_reg(FILE *input_file, FILE *output_file, char *fileType);