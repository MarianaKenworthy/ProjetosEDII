#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "projeto2.h"

void insere (FILE* input, FILE* output, FILE* indice){  
    char tam, registro[135];
    int aux;
    seg segurado;
    ind novo;

    if (fread(&segurado, sizeof(seg), 1, input) == 0){
        printf("Nao ha mais seguradoras no arquivo de insercao.");
        return;
    }

    sprintf(registro, "%s#%s#%s#%s#", segurado.codigo, segurado.nome, segurado.seguradora, segurado.tipo);
    tam = strlen(registro);

    fseek(output, 0, 2);   
    fwrite(&tam, sizeof(char), 1, output);
    fwrite(&registro, sizeof(char), tam, output);

    sprintf(novo.chave, "%s", segurado.codigo);
    fseek(indice, -4, 2);
    fread(&aux, sizeof(int), 1, indice);
    novo.offset = aux + tam +1;
    
    fwrite(&novo, sizeof(ind), 1, indice);



}



