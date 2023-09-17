#include<stdio.h>

int main() {
    FILE *fd;
    
    //////////////////////////////
    struct segurados {
        char cod_cli[4];
        char nome_cli[50];
        char nome_seg[50];
        char tipo_seg[30];
    } vet[8] = {{"001", "Maria", "Porto", "Residencia"},
                {"002", "Joao", "Zurich", "Auto"},
                {"003", "Pedro Miguel", "Porto", "Residencia"},
				{"004", "Catarina", "Porto", "Vida"},
				{"005", "Joao Silva", "Porto", "Vida"},
				{"006", "Maria da Silva Neves", "Porto", "Auto"},
				{"007", "Ticianay", "Porto", "Auto"},
				{"008", "Veronica", "Zurich", "Residencia"}
				};
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct remove {
        char cod_cli[4];
    } vet_r[5] = {{"003"},
                  {"004"},
                  {"001"},
                  {"006"},
                  {"008"}};
       
    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
}

