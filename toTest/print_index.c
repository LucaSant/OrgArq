#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    int fileType = 2;
    int id, rrn;
    long offset;
    FILE *index = fopen("indice6.bin", "rb+");
    fseek(index, 0, SEEK_END);
    long final = ftell(index);

    int index_size = fileType == 1 ? (int) ((final - 1) / 8) : ((final - 1) / 12); 
    fseek(index, 1, SEEK_SET);
    while(ftell(index) < final){
        fread(&id, sizeof(int), 1, index);
        printf("ID: %d - ", id);
        if(fileType == 1){
            fread(&rrn, sizeof(int), 1, index);
            printf("rrn: %d\n", rrn);
        } else {
            fread(&offset, sizeof(long), 1, index);
            printf("offset: %ld\n\n", offset);
        }
    }

}