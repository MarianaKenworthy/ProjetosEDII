#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct segurado
{
    char codigo[4];
    char nome[50];
    char seguradora[50];
    char tipo[30];
} seg;

typedef struct indice
{
    char chave[4];
    int offset;
} ind;

void insere(FILE *input, FILE *output, FILE *indice, FILE *cursor, FILE *secundario1, FILE *secundario2);
void montarCabecalho(FILE *indice, FILE *output);
void buscaP(FILE *input, FILE *output, FILE *indice, FILE *cursor);
void buscaS (FILE *input, FILE *output, FILE *secundario1, FILE *secundario2, FILE* indice, FILE* cursor);
void imprime(FILE *output, FILE *indice, char *codigoBuscado);
