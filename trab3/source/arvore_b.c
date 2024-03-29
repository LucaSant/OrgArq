/*
    SCC0215 - Organização de Arquivos (2022.1)
    Antônio Medrado - 10748389
    Lucas Sant'Anna - 10748521
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../headers/arvore_b.h"

node *create_node(int order, char type, int fileType) {
    node *new_node = (node*)malloc(sizeof(node));
    assert(new_node);
    // Cria o nó de acordo com o tipo do nó, tipo do arquivo e a ordem da arvóre
    new_node->type[0] = type;
    new_node->num_keys = 0;
    new_node->keys = (int*)malloc((order - 1) * sizeof(int));
    if(fileType == 1) {
        new_node->key_rrn = (int*)malloc((order - 1) * sizeof(int));
        new_node->key_offset = NULL;
    } else {
        new_node->key_offset = (long*)malloc((order - 1) * sizeof(long));
        new_node->key_rrn = NULL;   
    }
    new_node->child = (int*)malloc((order) * sizeof(int));

    // Preenche os campos como vazios
    for(int i = 0; i < order; i++) {
        if(i < order-1) {
            new_node->keys[i] = -1;
            if(fileType == 1) new_node->key_rrn[i] = -1;
            else new_node->key_offset[i] = -1;
        }
        new_node->child[i] = -1;
    }
    return new_node;
}

long get_node_offset(int rrn, int fileType) {
    // Calcula o Byte Offset
    int page_size;
    if(fileType == 1) page_size = 45;
    else page_size = 57;
    long offset = (rrn + 1) * page_size; // rrn + 1 (relativo ao cabeçalho)
    return offset;
}

node *read_node(b_tree *btree, int rrn, int fileType) {
    node *this_node = create_node(4, '2', fileType);
    long offset = get_node_offset(rrn, fileType);
    // Variáveis auxiliares
    int aux;
    long aux2;
    this_node->node_rrn = rrn;
    // Vai ao Nó e realiza a leitura
    fseek(btree->file, offset, SEEK_SET); 
    fread(this_node->type, sizeof(char), 1, btree->file); // Status
    fread(&aux, sizeof(int), 1, btree->file); // Número de Nós
    this_node->num_keys = aux;
    for(int i = 0; i < 3; i++) {
        fread(&aux, sizeof(int), 1, btree->file);
        this_node->keys[i] = aux;
        if(fileType == 1) {
            fread(&aux, sizeof(int), 1, btree->file);
            this_node->key_rrn[i] = aux;
        } else {
            fread(&aux2, sizeof(long), 1, btree->file);
            this_node->key_offset[i] = aux2;
        }
    }
    for(int i = 0; i < 4; i++) {
        fread(&aux, sizeof(int), 1, btree->file);
        this_node->child[i] = aux;
    }
    return this_node;
}

void write_node(b_tree *btree, node *n, int fileType) {
    FILE *btree_file = btree->file;
    long offset = get_node_offset(n->node_rrn, fileType);
    fseek(btree->file, offset, SEEK_SET);
    fwrite(n->type, sizeof(char), 1, btree->file);
    fwrite(&(n->num_keys), sizeof(int), 1, btree->file);
    for(int i = 0; i < 3; i++) {
        fwrite(&(n->keys[i]), sizeof(int), 1, btree->file);
        if(fileType == 1) fwrite(&(n->key_rrn[i]), sizeof(int), 1, btree->file);
        else fwrite(&(n->key_offset[i]), sizeof(long), 1, btree->file);
    }
    for(int i = 0; i < 4; i++) {
        int c = n->child[i];
        fwrite(&c, sizeof(int), 1, btree->file);
    }
}


int update_node_type(b_tree *btree, node *n){
    if(n->node_rrn == btree->root->node_rrn) { // Nó raiz
        n->type[0] = '0';
        return 0;
    }
    for(int i = 0; i < btree->order; i++) { 
        if(n->child[i] != -1) { // Nó intermediário
            n->type[0] = '1';
            return 1;
        }
    }
    n->type[0] = '2'; // Nó folha
    return 2;
}

void add_node_to_btree(b_tree *btree, node *n) {
    if(n->type[0] == '0') btree->root = n;
    btree->num_nodes++;
    n->node_rrn = btree->next_rrn;
    btree->next_rrn++;
}

b_tree *create_btree(int order, char *arquivo) {
    b_tree *new_bt = (b_tree*)malloc(sizeof(b_tree));
    new_bt->file = fopen(arquivo, "wb+");
    new_bt->order = order;
    // Cabeçalho
    strncpy(new_bt->status, "0", 1);
    new_bt->root = NULL;
    new_bt->num_nodes = 0;
    new_bt->next_rrn = 0;
}

b_tree *read_btree(int order, char *arquivo, int fileType) {
    b_tree *bt = (b_tree*)malloc(sizeof(b_tree));
    bt->file = fopen(arquivo, "rb+");
    bt->order = order;
    // Cabeçalho
    strncpy(bt->status, "0", 1);
    fseek(bt->file, 0, SEEK_SET);
    fwrite("0", sizeof(char), 1, bt->file);
    long x = ftell(bt->file);
    int aux;
    // RRN no nó raiz
    fread(&aux, sizeof(int), 1, bt->file);
    bt->root = read_node(bt, aux, fileType);
    fseek(bt->file, 5, SEEK_SET);
    // Pŕoximo RRN disponível
    fread(&aux, sizeof(int), 1, bt->file);
    bt->next_rrn = aux;
    // Número de nós na árvore
    fread(&aux, sizeof(int), 1, bt->file);
    bt->num_nodes = aux;
    fseek(bt->file, 0, SEEK_SET);
    return bt;
}

int get_root_rrn(b_tree *btree) {
    if(btree->root != NULL) return btree->root->node_rrn;
    else return -1;
}

// Escreve o cabeçalho da arvóre binṕaria em disco
void write_btree_header(b_tree *btree, int fileType) {
    fseek(btree->file, 0, SEEK_SET);
    fwrite(btree->status, sizeof(char), 1, btree->file);
    int root_rrn = get_root_rrn(btree);
    fwrite(&root_rrn, sizeof(int), 1, btree->file);
    fwrite(&(btree->next_rrn), sizeof(int), 1, btree->file);
    fwrite(&(btree->num_nodes), sizeof(int), 1, btree->file);
    
    int limit;
    if(fileType == 1) limit = 45;
    else limit = 57;
    for(int i = 13; i < limit; i++) fwrite("$", sizeof(char), 1, btree->file);
   
    fseek(btree->file, 0, SEEK_SET);
}

// Lê o cabeçalho da arvóre binária para a RAM
void read_btree_header(b_tree *btree, int fileType) {
    fseek(btree->file, 1, SEEK_SET);
    int aux;
    fread(btree->status, sizeof(char), 1, btree->file);
    fread(&aux, sizeof(int), 1, btree->file);
    if((aux != get_root_rrn(btree)) && (aux != -1)) {
        btree->root = read_node(btree, aux, fileType);
    }

}

// Função Split
int split(b_tree *btree, node *curr_node, node *to_promo, int fileType) {
    node *big_node = create_node(5, '0', fileType); // Nó agregador - Tem um nó a mais 
    // Cópia as chaves do nó atual para o grande nó
    int where_add = 0;
    int i;
    for(i = 0; i < 4; i++) {
        if(i < 3){
            big_node->keys[i] = curr_node->keys[i];
            if(to_promo->keys[0] > curr_node->keys[i]) where_add++; // já verifica onde adicionar a nova chave
            if(fileType == 1) big_node->key_rrn[i] = curr_node->key_rrn[i];
            else big_node->key_offset[i] = curr_node->key_offset[i];
            big_node->num_keys++;
        }
        big_node->child[i] = curr_node->child[i];
    }

    // Move as chaves no grande nó
    i = 3; // order - 2
    while(i >= where_add) {
        if(big_node->keys[i] != -1) {
            big_node->keys[i+1] = big_node->keys[i];
            if(fileType == 1) big_node->key_rrn[i+1] = big_node->key_rrn[i];
            else big_node->key_offset[i+1] = big_node->key_offset[i];
            big_node->child[i+2] = big_node->child[i+1];
        }
        i--;
    }

    // Adiciona a nova chave no grande nó
    big_node->keys[where_add] = to_promo->keys[0];
    if(fileType == 1) big_node->key_rrn[where_add] = to_promo->key_rrn[0];
    else big_node->key_offset[where_add] = to_promo->key_offset[0];
    big_node->child[where_add+1] = to_promo->child[1]; 

    /*
    printf("Node agregador: [%d|%d][%d|%d][%d|%d][%d|%d]\n", 
        big_node->keys[0], big_node->key_rrn[0],
        big_node->keys[1], big_node->key_rrn[1],
        big_node->keys[2], big_node->key_rrn[2],
        big_node->keys[3], big_node->key_rrn[3]
        );
    */
    
    // Separa o grande nó
    node *new_node = create_node(4, '2', fileType); // *FALTA DEFINIR O TIPO DE NÓ*
    add_node_to_btree(btree, new_node);
    curr_node->num_keys = 0;

    for(i = 0; i < 4; i++) { // Nó antigo as chaves em decorrência do split
        if(i < 2) { // Antes da chave a ser promovida*
            curr_node->keys[i] = big_node->keys[i];
            if(fileType == 1) curr_node->key_rrn[i] = big_node->key_rrn[i];
            else curr_node->key_offset[i] = big_node->key_offset[i];
            if(i == 0) { 
                curr_node->child[i] = big_node->child[i]; // o primeiro filho da esquerda
            } else { // A última chave do nó é resetada
                curr_node->keys[i+1] = -1;
                if(fileType == 1) curr_node->key_rrn[i+1] = -1;
                else curr_node->key_offset[i+1] = -1;
                curr_node->child[i+2] = -1;
            }
            curr_node->child[i+1] = big_node->child[i+1]; // Todos os filhos direitos seguintes
            curr_node->num_keys++;

        } else if (i == 2) { // Chave que será promovida*
            to_promo->keys[0] = big_node->keys[i];
            if(fileType == 1) to_promo->key_rrn[0] = big_node->key_rrn[i];
            else to_promo->key_offset[0] = big_node->key_offset[i];
            // A chave a ser promovida recebe como filhos:  
            to_promo->child[1] = new_node->node_rrn; // O novo nó como filho direito
            if(curr_node->type[0] == '0') to_promo->type[0] = '0';

        } else { // Novo nó criado a partir do split
            
            new_node->keys[0] = big_node->keys[i];
            if(fileType == 1) new_node->key_rrn[0] = big_node->key_rrn[i];
            else new_node->key_offset[0] = big_node->key_offset[i];
            new_node->child[0] = big_node->child[i];
            new_node->child[1] = big_node->child[i+1];
            new_node->num_keys++;
        }
    }

    if(curr_node->type[0] == '0') strncpy(to_promo->type, "0", 1);
    else strncpy(to_promo->type, "1", 1);
    update_node_type(btree, new_node);
    write_node(btree, new_node, fileType);
    free(big_node);
    return 1;
}

long find_in_btree(b_tree *btree, int this_rrn, int key, int fileType) {
    // Não encontrou nenhuma chave que corresponda ao valor passado
    if(this_rrn == -1) { 
        return -1; 
    }

    node *this_node;
    if(this_rrn == btree->root->node_rrn) this_node = btree->root; // Se for: Nó raiz já está em RAM
    else this_node = read_node(btree, this_rrn, fileType);// Caso contrário: Carrega Nó para RAM

    // Procura no nó atual enquanto verifica para qual filho pode precisar descer
    int i = 0;
    long found;
    while(i < this_node->num_keys) {
        // Percorre as chaves do nó enquanto elas forem menores que a chave a ser adicionada
        if(key > this_node->keys[i]){ i++; 
        } else if(key == this_node->keys[i]) {
            if(fileType == 1) {
                found = (long)this_node->key_rrn[i];
                free(this_node);
                return found; // retorna o RRN como long int
            } else { 
                found = this_node->key_offset[i];
                free(this_node);
                return found; 
            }
        // Chave pode estar no nó filho à esquerda (nó com chaves menores)
        } else { break; }
    }
    int where_add = i;
    long ret = find_in_btree(btree, this_node->child[where_add], key, fileType);
    free(this_node);
    return ret;
}


// Insere um nova chave à árvore
int insertion_btree(b_tree *btree, int this_rrn, _index *indice, node *to_promo, int fileType) {

    if(this_rrn == -1) { // Chegou ao fim da etapa de busca
        to_promo->keys[0] = indice->id;
        if(fileType == 1) to_promo->key_rrn[0] = indice->rrn;
        else to_promo->key_offset[0] = indice->byte_offset;
        to_promo->child[0] = -1;
        to_promo->child[1] = -1;
        return 1; // 1 - PROMOÇÃO
    }

    node *this_node;
    // Verifica se o RRN atual é ou não o RRN do nó raiz
    if(this_rrn == btree->root->node_rrn) this_node = btree->root; // Se for: Nó raiz já está em RAM
    else this_node = read_node(btree, this_rrn, fileType);// Caso contrário: Carrega Nó para RAM

    int i = 0;
    while(i < this_node->num_keys) {
        // Percorre as chaves do nó enquanto elas forem menores que a chave a ser adicionada
        if(indice->id > this_node->keys[i]) i++; 
        else if(indice->id == this_node->keys[i]) return -1; // ERROR - Chave já existente
        else break; // Chave pode estar no nó filho à esquerda (nó com chaves menores)

    }
    int where_add = i; 
    int ret = insertion_btree(btree, this_node->child[where_add], indice, to_promo, fileType);
    if(ret == -1 || ret == 0){ 
        if(this_node->type[0] != '0') free(this_node);
        return ret; } // Erro ou Não há promoção
    else if(this_node->num_keys < btree->order - 1) { // Promoção e nó não cheio
        // Move as chaves do nó
        i = 2; // order - 2
        while(i >= where_add) {
            if(this_node->keys[i] != -1) {
                this_node->keys[i+1] = this_node->keys[i];
                if(fileType == 1) this_node->key_rrn[i+1] = this_node->key_rrn[i];
                else this_node->key_offset[i+1] = this_node->key_offset[i];
                this_node->child[i+2] = this_node->child[i+1];
            }
            i--;
        }
        // Adiciona a nova chave no nó
        this_node->keys[where_add] = to_promo->keys[0];
        if(fileType == 1) this_node->key_rrn[where_add] = to_promo->key_rrn[0];
        else this_node->key_offset[where_add] = to_promo->key_offset[0];
        this_node->child[where_add+1] = to_promo->child[1]; 
        this_node->num_keys++; // Incrementa o número de chaves no nó
        update_node_type(btree, this_node); // Nó pode deixar de ser do tipo folha
        write_node(btree, this_node, fileType);
        if(this_node->type[0] != '0') free(this_node);
        return 0; // 0 - NÃO PROMOÇÃO

    } else { // Promoção e nó cheio
        split(btree, this_node, to_promo, fileType);
        if(to_promo->type[0] == '0') { // Caso o split foi realizado no nó raiz

            node *new_root = create_node(btree->order, '0', fileType); // Cria um novo nó para ser a raiz
            add_node_to_btree(btree, new_root); // Adiciona o novo nó raiz a estrutura da árvore B
            update_node_type(btree, this_node); //  Nó deixa de ser raiz
            write_node(btree, this_node, fileType); // Reescreve o nó atual

            // Escreve no novo nó raiz
            btree->root->child[0] = this_node->node_rrn;
            btree->root->child[1] = to_promo->child[1];
            btree->root->keys[0] = to_promo->keys[0];
            if(fileType == 1) btree->root->key_rrn[0] = to_promo->key_rrn[0];
            else btree->root->key_offset[0] = to_promo->key_offset[0];
            btree->root->num_keys++;
            write_node(btree, btree->root, fileType); // Escreve no arquivo o novo nó raiz
            return 0; // 0 - NÃO PROMOÇÃO
        }
        update_node_type(btree, this_node); // NECESSÁRIO (?) --------
        write_node(btree, this_node, fileType); // Reescreve nó no arquivo
        return 1; // 1 - PROMOÇÃO
    }
}

void mem_to_btree(b_tree *btree, _index **indices, int fileType, int ind_vector_size){
    // Cria um nó de uma chave para guardar os dados para promoção
    node *to_promo = create_node(2, '2', fileType);
    for(int i = 0; i < ind_vector_size; i++) {
        insertion_btree(btree, btree->root->node_rrn, indices[i], to_promo, fileType);
    }
    // Reescreve o cabeçalho
    write_btree_header(btree, fileType);
    // Termina de limpar o array da memória e fecha tudo
    free(indices);
    fseek(btree->file, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, btree->file); // Integridade
    fclose(btree->file);
}