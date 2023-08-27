#include<stdio.h>

int main() {
    FILE *fd;
    
    //////////////////////////////
    struct segurados {
        char cod_cli[4];
        char nome_cli[50];
        char nome_seg[50];
        char tipo_seg[30];
    } vet[3] = {{"001", "Maria", "Porto", "Residencia"},
                {"002", "Joao", "Zurich", "Auto"},
                {"003", "Pedro", "Porto", "Vida"}};
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct remove {
        char cod_cli[4];
    } vet_r[1] = {{"002"}};
       
    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
}

