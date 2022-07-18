/*
    SCC0215 - Organização de Arquivos (2022.1)
    Antônio Medrado - 10748389
    Lucas Sant'Anna - 10748521
*/

int csv_to_bin(char *tipoArquivo, char *arquivoEntrada); // Funcionalidade [1]
int read_all(char *tipoArquivo, char *arquivoEntrada); // Funcionalidade [2]
int read_filter(char *tipoArquivo, char *arquivoEntrada); // Funcionalidade [3]
int read_rrn(char *tipoArquivo, char *arquivoEntrada); // Funcionalidade [4]
int create_index(char *tipoArquivo, char *arquivoEntrada); // Funcionalidade [5]
int remove_reg(char *tipoArquivo, char *arquivoEntrada); // Funcionalidade [6]
int insert_reg(char *tipoArquivo, char *arquivoEntrada); // Funcionalidade [7]
int update_reg(char *tipoArquivo, char *arquivoEntrada); // Funcionalidade [8]
int create_index_btree(char *tipoArquivo, char* arquivoEntrada, int order); // Funcionalide [9]