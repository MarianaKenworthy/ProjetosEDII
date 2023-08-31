#include<stdio.h>

int main() {
    FILE *fd;
    
    //////////////////////////////
    struct segurados {
        unsigned char cod_cli[4];
        unsigned char nome_cli[50];
        unsigned char nome_seg[50];
        unsigned char tipo_seg[30];
    } vet[6] = {{"001", "Maria", "Porto", "Residencia"},
                {"002", "Joao", "Zurich", "Auto"},
                {"003", "Pedro", "Porto", "Vida"},
                {"004", "Orlando", "SeguradoraComMaiorNomeDaHistoria", "Residencia"},
                {"005", "Mauriana", "da Silva", "Vida"},};
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct remove {
        char cod_cli[4];
    } vet_r[2] = {{"002"}, {"004"}};
       
    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
}

