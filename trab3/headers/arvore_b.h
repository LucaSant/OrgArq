#include <stdio.h>
#include "../headers/structs.h"
#include "../headers/auxiliares.h"

typedef struct b_tree_node {
    int node_rrn;
    char type[1];
    int num_keys;
    int *keys;
    int *key_rrn;
    long *key_offset;
    int *child;
} node;

typedef struct b_tree {
    // Dados 
    FILE *file;
    int order; // Ordem da arvóre B

    //Cabeçaçho
    char status[1]; // status do arquivo
    node *root; // estrutura do nó raiz | Cabeçalho pega o RRN (atributo address da estrutura node)
    int num_nodes; // Número de nós da arvóre B
    int next_rrn; // Próximo rrn do registro no arquivo de indíce
} b_tree;

node *create_node(int order, char type, int fileType);

void add_node_to_btree(b_tree *btree, node *n);
b_tree *create_btree(int order, char *arquivo);
b_tree *read_btree(int order, char *arquivo, int fileType);
void mem_to_btree(b_tree *btree, _index **indices, int fileType, int ind_vector_size);
long find_in_btree(b_tree *btree, int this_rrn, int key, int fileType);
int insertion_btree(b_tree *btree, int this_rrn, _index *indice, node *to_promo, int fileType);