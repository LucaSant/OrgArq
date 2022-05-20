// Antônio Medrado 10748389

#include "registros.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define REGSIZE 238

void imprimeRegistro(FILE *f) { // Imprime os dados de um registro, dada sua posição no arquivo
    int age;
    char buffer[100];
    fread(buffer, sizeof(char), 51, f);
    printf("Firstname: %s\n", buffer);
    fread(buffer, sizeof(char), 51, f);
    printf("Lastname: %s\n", buffer);
    fread(buffer, sizeof(char), 81, f);
    printf("Email: %s\n", buffer);
    fread(buffer, sizeof(char), 51, f);
    printf("Nationality: %s\n", buffer);
    fread(&age, sizeof(int), 1, f);
    printf("Age: %d\n\n", age);
}

int gravar(char filename[31]) {
    /**
     * Gravação dos dados de várias pessoas em formato binário.
     * Escreve dados do registro e padding direto no arquivo.
     * Reescreve totalmente arquivo original.
    **/
    FILE *f = fopen(filename, "wb+");
    if (f == NULL) return 0; // Falha no processamento do arquivo
    char firstname[51], lastname[51], email[81], nationality[51];
    char trash[1] = {'$'};
    int nRegisters, age;
    scanf("%d", &nRegisters);
    for(int i = 0; i < nRegisters; i++) {
        scanf("%s %s %s %s %d", firstname, lastname, email, nationality, &age);

        fwrite(firstname, sizeof(char), strlen(firstname)+1, f);
        for(int j = 0; j < (50 - strlen(firstname)); j++) fwrite(trash, sizeof(char), 1, f);

        fwrite(lastname, sizeof(char), strlen(lastname)+1, f);
        for(int j = 0; j < (50 - strlen(lastname)); j++) fwrite(trash, sizeof(char), 1, f);

        fwrite(email, sizeof(char), strlen(email)+1, f);
        for(int j = 0; j < (80 - strlen(email)); j++) fwrite(trash, sizeof(char), 1, f);

        fwrite(nationality, sizeof(char), strlen(nationality)+1, f);
        for(int j = 0; j < (50 - strlen(nationality)); j++) fwrite(trash, sizeof(char), 1, f);

        fwrite(&age, sizeof(int), 1, f);
    }
    fclose(f);
    binarioNaTela(filename);
    return 1;
}

int recuperar(char filename[31]) {
    /**
     * Recuperação dos dados, de todas as pessoas, armazenados no arquivo de dados,
     * mostrando os dados de forma organizada na saída padrão
     * para permitir a distinção dos campos e registros.
     * Conta todos os registros e faz o processo de leitura para cada um.
    **/
    FILE *f = fopen(filename, "rb");
    if (f == NULL) return 0; // Falha no processamento do arquivo
    
    fseek(f, 0, SEEK_END);
    int ttl = (int) (ftell(f) / REGSIZE); // Número total de registros
    fseek(f, 0, SEEK_SET);

    for (int i = 0; i < ttl; i++) imprimeRegistro(f);
    fclose(f);
    return 1;
}

int recuperarRegistro(char filename[31]) {
    /**
     * Recuperação do registro a partir do RRN, mostrando seus dados
     * de forma organizada na saída padrão para permitir a distinção dos campos.
     * Posiciona o ponteiro de acordo com o RRN e lê o conteúdo do registro a seguir.
    **/
    FILE *f = fopen(filename, "rb");
    if (f == NULL) return 0; // Falha no processamento do arquivo

    int rrn;
    scanf("%d", &rrn);
    fseek(f, 0, SEEK_END);
    int ttl = (int) (ftell(f) / REGSIZE); // Número total de registros
    if (rrn >= ttl) return -1; // Não foi possível ler o arquivo

    fseek(f, rrn*REGSIZE, SEEK_SET);
    imprimeRegistro(f);
    fclose(f);
    return 1;
}

/**
 * Funções previamente fornecidas a seguir
**/

void readline(char* string){
    char c = 0;

    do{
        c = (char) getchar();

    } while(c == '\n' || c == '\r');

    int i = 0;

    do{
        string[i] = c;
        i++;
        c = getchar();
    } while(c != '\n' && c != '\r');

    string[i]  = '\0';
}

void binarioNaTela(char *nomeArquivoBinario) { /* Você não precisa entender o código dessa função. */

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}