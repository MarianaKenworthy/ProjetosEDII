#include "projeto2.c"

int main()
{
    FILE *output = fopen("cadastro.dat", "r+b");
    if (!output)
        output = fopen("cadastro.dat", "w+b");

    FILE *input1 = fopen("insere.bin", "rb");
    FILE *input2 = fopen("busca_p.bin", "rb");
    FILE *input3 = fopen("busca_s.bin", "rb");

    FILE *indice = fopen("indice.bin", "r+b");
    if (!indice)
        indice = fopen("indice.bin", "w+b");

    FILE *secundario1 = fopen("secundario1.bin", "r+b");
    if (!secundario1)
        secundario1 = fopen("secundario1.bin", "w+b");

    FILE *secundario2 = fopen("secundario2.bin", "r+b");
    if (!secundario2)
        secundario2 = fopen("secundario2.bin", "w+b");

    FILE *cursor = fopen("cursorauxiliar.dat", "r+b");
    if (!cursor)
        cursor = fopen("cursorauxiliar.dat", "w+b");

    montarCabecalho(indice, output);
    char escolha;
    do
    {
        printf("\n---------------------------\n\nO que deseja fazer?\n\n1- Inserir\n2- Busca Primaria\n3- Busca Secundaria\n4- Sair\n");
        scanf(" %c", &escolha);
        switch (escolha)
        {
        case '1':
        {
            insere(input1, output, indice, cursor, secundario1, secundario2);
            break;
        }
        case '2':
        {
            buscaP(input2, output, indice, cursor);
            break;
        }
        case '3':
        {
            buscaS(input3, output, secundario1, secundario2, indice, cursor);
            break;
        }
        }
    } while (escolha != '4');

    fclose(output);
    fclose(input1);
    fclose(input2);
    fclose(input3);
    fclose(indice);
    fclose(cursor);
}