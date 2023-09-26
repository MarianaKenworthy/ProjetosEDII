#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> 
#include "Projeto1/projeto1.h"

int main()
{
    FILE *output = fopen("cadastro.dat", "r+b");
    if (!output)
        output = fopen("cadastro.dat", "w+b");
    montaCabecalho(output);

    FILE *input1 = fopen("insere.bin", "rb");
    FILE *input2 = fopen("remove.bin", "rb");
    FILE *cursor = fopen("cursorauxiliar.dat", "r+b");
    if (!cursor)
        cursor = fopen("cursorauxiliar.dat", "w+b");

    char escolha;
    do
    {
        printf("\n---------------------------\n\nO que deseja fazer?\n\n1- Inserir\n2- Busca Primaria\n3- Busca Secundaria\n4- Sair\n");
        scanf(" %c", &escolha);
        switch (escolha)
        {
        case '1':
        {

            break;
        }
        case '2':
        {

            break;
        }
        case '3':
        {

        }
        }
    } while (escolha != '4');

    fclose(input1);
    fclose(input2);
    fclose(output);
    fclose(cursor);
}