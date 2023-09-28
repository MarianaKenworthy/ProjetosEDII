#include <stdio.h>

int main() {
    FILE *fd;
    
    struct segurados {
        char cod_cli[4];
        char nome_cli[50];
        char nome_seg[50];
        char tipo_seg[30];
    } vet[6] = {{"002", "Maria", "Porto", "Auto"},
                {"001", "Joao", "Zurich", "Auto"},
                {"003", "Pedro", "Porto", "Residencia"},
				
				{"006", "Felipe", "Porto", "Vida"},
                {"005", "Davi", "Azul", "Auto"},
                {"004", "Leo", "Zurich", "Residencia"}};
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct busca_p {
        char cod_cli[4];
    } vet_bp[6] = {{"003"},
                   {"009"},
				   {"002"},
				   
				   {"006"},
				   
				   {"002"},
				   {"006"}};
       
    fd = fopen("busca_p.bin", "w+b");
    fwrite(vet_bp, sizeof(vet_bp), 1, fd);
    fclose(fd);
    
    //////////////////////////////
    char nome_seg[8][50] = {"Porto", "XXX", "Zurich",
	                                 "Porto",
									 "Porto", "Zurich", "XXX", "Azul"};
       
    fd = fopen("busca_s.bin", "w+b");
    for (int i=0; i<8; i++)
       fwrite(nome_seg[i], sizeof(char), 50, fd);
    fclose(fd);
    
    /*char buffer[50];
    fd = fopen("busca_s.bin", "r+b");
    for (int i=0; i<8; i++)
       {
         fread(buffer, sizeof(buffer), 1, fd);
         printf("\n%s",buffer);
       }
    fclose(fd);
    getch();*/
}
