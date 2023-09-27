#include "projeto2.h"

void insere(FILE *input, FILE *output, FILE *indice, FILE *cursor, FILE *secundario1, FILE *secundario2)
{
    int aux = 1;
    fseek(indice, 0, 0);
    fwrite(&aux, sizeof(char), 1, indice);
    char tam, registro[135], cursorInsere, auxChar, auxTamAnterior;
    int auxOffsetAnterior;
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
        aux = 0;
        fseek(indice, 0, 0);
        fwrite(&aux, sizeof(int), 1, indice);
        return;
    }

    cursorInsere += 1;
    fseek(cursor, 0, 0);
    fwrite(&cursorInsere, sizeof(char), 1, cursor); // Coloca o cursor externo de inserção novo no arquivo dos cursores

    sprintf(registro, "%s#%s#%s#%s#", segurado.codigo, segurado.nome, segurado.seguradora, segurado.tipo);
    tam = strlen(registro);

    // anota o registro no arquivo principal
    fseek(output, 0, 2);
    fwrite(&tam, sizeof(char), 1, output);
    fwrite(&registro, sizeof(char), tam, output);

    // anota indice prim
    sprintf(novo.chave, "%s", segurado.codigo);
    fseek(indice, sizeof(int), 0);

    if (fread(&auxChar, sizeof(char), 1, indice) == 0)
        novo.offset = 0;
    else
    {
        fseek(indice, -sizeof(int), 2);
        fread(&auxOffsetAnterior, sizeof(int), 1, indice);
        fseek(output, auxOffsetAnterior, 0);
        fread(&auxTamAnterior, sizeof(char), 1, output);
        novo.offset = auxOffsetAnterior + auxTamAnterior + 1;
    }
    fseek(indice, 0, 2);
    fwrite(&novo, sizeof(ind), 1, indice);

    aux = 0;
    fseek(indice, 0, 0);
    fwrite(&aux, sizeof(int), 1, indice);

    // anota secundario

    char string1[50], string2[50];
    int i, auxInt;
    bool flagcompara = 1;
    strcpy(string1, segurado.seguradora);

    fseek(secundario1, 0, 0);
    // procura o nome da seguradora nas chaves secundarias
    // deve dar pra reciclar isso aqui pra busca sec
    for (i = 0; fread(&auxChar, sizeof(char), 1, secundario1) != 0; i++)
    {

        if (auxChar == '#')
        {
            string2[i] = '\0';
            flagcompara = strcmp(string1, string2);

            if (flagcompara != 0)
            {
                i = 0;
                fseek(secundario1, 4, 1);
            }
            else
                break;
        }
        else
            string2[i] = auxChar;
    }

    int posicao;

    if (flagcompara == 0)
    { // Se a seguradora já existir

        fread(&auxInt, sizeof(int), 1, &secundario1);
        fseek(secundario2, 0, 2);
        posicao = ftell(secundario2);
        fseek(secundario1, -4, 1);
        fwrite(&posicao, sizeof(int), 1, secundario1);
        fwrite(&novo.chave, sizeof(char), 4, secundario2);
        fwrite(&auxInt, sizeof(int), 1, secundario2);
    }
    else
    {

        fwrite(&segurado.seguradora, sizeof(char), strlen(segurado.seguradora), secundario1);
        fwrite("#", sizeof(char), 1, secundario1);
        fseek(secundario2, 0, 2);
        posicao = ftell(secundario2);
        fwrite(&posicao, sizeof(int), 1, secundario1);
        fwrite(&novo.chave, sizeof(char), 4, secundario2);
        auxInt = -1;
        fwrite(&auxInt, sizeof(int), 1, secundario2);
    }
}

void montarCabecalho(FILE *indice, FILE *output)
{
    int aux = 0;
    if (fread(&aux, sizeof(int), 1, indice) == 0)
    {
        aux = 0;
        fseek(indice, 0, 0);
        fwrite(&aux, sizeof(int), 1, indice);
    }
    else
    {
        if (aux == 1)
        {
            // Quer dizer que algo deu errado na inserção
            fseek(output, 0, 0);
            int contadorHashtag = 0, i = 0, offsetComeco = 0, tamanhoReg = 0;
            char registro[135], j = 1;
            bool acabou = false;
            while (!acabou)
            {
                while (contadorHashtag != 4)
                {
                    if (fread(&registro[i], sizeof(char), 1, output) == 0)
                    {
                        acabou = true;
                        break;
                    }
                    tamanhoReg++;
                    if (registro[i] == '#')
                        contadorHashtag++;
                    i++;
                }
                contadorHashtag = 0;
                i = 0;
                // Salvar em indice registro[1...3] + . + offsetComeco:
                fwrite(&registro[1], sizeof(char), 1, indice);
                fwrite(&registro[2], sizeof(char), 1, indice);
                fwrite(&registro[3], sizeof(char), 1, indice);
                char aux = 0;
                fwrite(&aux, sizeof(char), 1, indice);
                fwrite(&offsetComeco, sizeof(int), 1, indice);
                fwrite(&tamanhoReg, sizeof(int), 1, indice);

                offsetComeco += tamanhoReg;
                tamanhoReg = 0;
            }
        }
    }
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

    imprime(output, indice, codigoBuscado);
}

void imprime(FILE *output, FILE *indice, char *codigoBuscado)
{
    char codigoAtualIndice[4] = {-1, -1, -1, -1};
    int offsetAtualIndice;
    fseek(indice, sizeof(int), 0);
    while (codigoAtualIndice[0] != codigoBuscado[0] || codigoAtualIndice[1] != codigoBuscado[1] || codigoAtualIndice[2] != codigoBuscado[2] || codigoAtualIndice[3] != codigoBuscado[3])
    {
        if (fread(&codigoAtualIndice, sizeof(char), 4, indice) == 0 || fread(&offsetAtualIndice, sizeof(int), 1, indice) == 0)
        {
            printf("O segurado nao foi encontrado.");
            return;
        }
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

void buscaS(FILE *input, FILE *output, FILE *secundario1, FILE *secundario2, FILE *indice, FILE *cursor)
{
    char procura[50], auxChar, codigo[4];
    char buffer[50];
    int i, flagcompara, posicao;

    fread(&procura, sizeof(char), 50, input);

    fseek(secundario1, 0, 0);

    // procura o nome da seguradora nas chaves secundarias
    for (i = 0; fread(&auxChar, sizeof(char), 1, secundario1) != 0; i++)
    {

        if (auxChar == '#')
        {
            buffer[i] = '\0';
            flagcompara = strcmp(procura, buffer);

            if (flagcompara != 0)
            {
                i = 0;
                fseek(secundario1, 4, 1);
            }
            else
                break;
        }
        else
            buffer[i] = auxChar;
    }

    if (flagcompara == 0)
    {
        fread(&posicao, sizeof(int), 1, secundario1);
        fseek(secundario2, posicao, 0);
        fread(&codigo, sizeof(char), 4, secundario2);
        imprime(output, indice, codigo);
    }
    else
        printf("\nCodigo nao encontrado\n");
}