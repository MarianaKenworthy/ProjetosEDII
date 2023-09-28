#include <stdio.h>

int main()
{
    FILE *fd;

    //////////////////////////////
    struct segurados
    {
        char cod_cli[4];
        char nome_cli[50];
        char nome_seg[50];
        char tipo_seg[30];
    } vet[8] = {{"002", "Maria", "Porto", "Residencia"},
                {"001", "Joao", "Zurich", "Auto"},
                {"004", "Pedro Miguel", "Porto", "Residencia"},
                {"003", "Catarina", "Zurich", "Vida"},
                {"006", "Joao Silva", "da Silva", "Vida"},
                {"005", "Maria da Silva Neves", "Timao", "Auto"},
                {"008", "Ticianay", "da Silva", "Auto"},
                {"007", "Veronica", "PopIt", "Residencia"}};

    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);

    //////////////////////////////
    struct buscaP
    {
        char cod_cli[4];
    } vet_p[6] = {{"003"},
                  {"004"},
                  {"001"},
                  {"006"},
                  {"008"},
                  {"999"}};

    fd = fopen("busca_p.bin", "w+b");
    fwrite(vet_p, sizeof(vet_p), 1, fd);
    fclose(fd);

    //////////////////////////////
    struct buscaS
    {
        char nome[50];
    } vet_s[6] = {{"Porto"},
                  {"Zurich"},
                  {"PopIt"},
                  {"da Silva"},
                  {"Timao"},
                  {"Nao existe"}};

    fd = fopen("busca_s.bin", "w+b");
    fwrite(vet_s, sizeof(vet_s), 1, fd);
    fclose(fd);
}
