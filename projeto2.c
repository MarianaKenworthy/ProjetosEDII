#include "projeto2.h"

void insere(FILE *input, FILE *output, FILE *indice, FILE *cursor)
{
    char tam, registro[135], cursorInsere, auxChar;
    int auxOffsetAnterior, auxTamAnterior;
    seg segurado;
    ind novo;

    fseek(cursor, 0, 0);
    if (fread(&cursorInsere, sizeof(char), 1, cursor) == 0)
    {
        // Checa se o cursor externo de inserção existe
        cursorInsere = 0;
    }
    fseek(input, cursorInsere * sizeof(seg), 0); // Coloca o cursor no lugar onde parou da última vez (se houve)

    if (fread(&segurado, sizeof(seg), 1, input) == 0)
    {
        printf("Nao ha mais seguradoras no arquivo de insercao.");
        return;
    }

    cursorInsere += 1;
    fseek(cursor, 0, 0);
    fwrite(&cursorInsere, sizeof(char), 1, cursor); // Coloca o cursor externo de inserção novo no arquivo dos cursores

    sprintf(registro, "%s#%s#%s#%s#", segurado.codigo, segurado.nome, segurado.seguradora, segurado.tipo);
    tam = strlen(registro);
    novo.tam = tam + 1;

    fseek(output, 0, 2);
    fwrite(&tam, sizeof(char), 1, output);
    fwrite(&registro, sizeof(char), tam, output);

    sprintf(novo.chave, "%s", segurado.codigo);
    fseek(indice, 0, 0);
    if (fread(&auxChar, sizeof(char), 1, indice) == 0)
    {
        novo.offset = 0;
    }
    else
    {
        fseek(indice, -(2 * sizeof(int)), 2);
        fread(&auxOffsetAnterior, sizeof(int), 1, indice);
        fread(&auxTamAnterior, sizeof(int), 1, indice);
        novo.offset = auxOffsetAnterior + auxTamAnterior;
    }
    fseek(indice, 0, 2);
    fwrite(&novo, sizeof(ind), 1, indice);
}

void buscaP(FILE *input, FILE *output, FILE *indice, FILE *cursor)
{
    char codigoBuscado[4], cursorBuscaPrimaria;

    fseek(cursor, 1, 0);
    if (fread(&cursorBuscaPrimaria, sizeof(char), 1, cursor) == 0)
    {
        // Checa se o cursor externo de busca primária existe
        cursorBuscaPrimaria = 0;
    }
    fseek(input, cursorBuscaPrimaria * 4, 0); // Coloca o cursor no lugar onde parou da última vez (se houve)

    if (fread(&codigoBuscado, sizeof(char), 4, input) == 0)
    {
        printf("Nao ha mais seguradoras no arquivo de busca primaria.");
        return;
    }

    cursorBuscaPrimaria += 1;
    fseek(cursor, 1, 0);
    fwrite(&cursorBuscaPrimaria, sizeof(char), 1, cursor); // Coloca o cursor externo de busca primária novo no arquivo dos cursores

    // Aqui o código a ser buscado está salvo em codigoBuscado[4], e o cursor está arrumado e não vai ser mais usado

    char codigoAtualIndice[4] = {-1, -1, -1, -1};
    int offsetAtualIndice;
    fseek(indice, 0, 0);
    while (codigoAtualIndice[0] != codigoBuscado[0] || codigoAtualIndice[1] != codigoBuscado[1] || codigoAtualIndice[2] != codigoBuscado[2] || codigoAtualIndice[3] != codigoBuscado[3])
    {
        if (fread(&codigoAtualIndice, sizeof(char), 4, indice) == 0 || fread(&offsetAtualIndice, sizeof(int), 1, indice) == 0)
        {
            printf("O segurado nao foi encontrado.");
            return;
        }
        fseek(indice, 4, 1);
    }
    // Ao sair do while, o offset do código está salvo em offsetAtualIndice, agora falta só printar

    fseek(output, offsetAtualIndice, 0);
    int contadorHashtag = 0, i = 0;
    char registro[135], j = 1;
    while (contadorHashtag != 4)
    {
        fread(&registro[i], sizeof(char), 1, output);
        if (registro[i] == '#')
            contadorHashtag++;
        i++;
    }
    printf("\nCodigo do segurado: ");
    while (registro[j] != '#')
    {
        printf("%c", registro[j]);
        j++;
    }
    j++;
    printf("\nNome do segurado: ");
    while (registro[j] != '#')
    {
        printf("%c", registro[j]);
        j++;
    }
    j++;
    printf("\nSeguradora: ");
    while (registro[j] != '#')
    {
        printf("%c", registro[j]);
        j++;
    }
    j++;
    printf("\nTipo do seguro: ");
    while (registro[j] != '#')
    {
        printf("%c", registro[j]);
        j++;
    }
}

void buscaS(FILE *input, FILE *output, FILE *indice, FILE *cursor)
{
    return;
}