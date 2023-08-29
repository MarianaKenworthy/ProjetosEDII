#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* To-do:
- Onde parou no arquivo cadastro, insere, e remove;
- First-fit do insere;
- Compactação (quando tudo estiver pronto);
- Deixar bonitinho (🏳‍🌈) - Ponteiro a mais;
*/

typedef struct segurado{
    char codigo[4];
    char nome[50];
    char seguradora[50];
    char tipo[30];
} seg;

void montaCabecalho(FILE *output){
    char tam;
    char registro[4];
    sprintf(registro, "**|");
    // Assim ficaria {3**|}, onde:
    // '*' Indica que é removido para não ler em outros momentos, mas no caso é o cabeçalho dos removidos
    // '*' Indica que por enquanto nenhum foi removido, porém vai mudar para a distância do mais recentemente removido
    // '|' Indica o começo do arquivo de verdade, foi um jeito mais fácil de arrumar esse problema, levar isso em conta nas funções
    tam = strlen(registro);

    fwrite(&tam, sizeof(char), 1, output);
    fwrite(&registro, sizeof(char), tam, output);
}

void achaEspaco (FILE* output, char tam){
    char ch;
    fseek(output, 2, 0);

    do {
        fread(&ch, sizeof(char), 1, output);
        fseek(output, ch, 0);

        if(ch == '*'){
            fseek(output,0, 2);
            return;
        }
    } while(tam < ch);

}

void montaCampos(FILE *input, FILE *output){
    char tam;
    seg segurado;
    fread(&segurado, sizeof(seg), 1, input);
    char registro[135];
    sprintf(registro, "%s#%s#%s#%s#", segurado.codigo, segurado.nome, segurado.seguradora, segurado.tipo);
    tam = strlen(registro);
    achaEspaco(output, tam);

    fwrite(&tam, sizeof(char), 1, output);
    fwrite(&registro, sizeof(char), tam, output);
}



void removeReg(FILE *input, FILE *output){
    char codigo[4], aux[4], verificaRemovido;
    fread(codigo, sizeof(char), 4, input); // Lê o codigo a ser removido do remove.bin

    char contaDistancia = 4;

    fseek(output, 4, 0);                  // Garante que está no começo (4 por conta do cabeçalho)
    fread(&aux, sizeof(char), 4, output); // Lê o código em cadastro.dat, para comparar com o remove.bin

    fseek(output, 4, 0);
    bool primeiroReg = false;
    if ((codigo[0] == aux[1]) || (codigo[1] == aux[2]) || (codigo[2] == aux[3])){
        primeiroReg = true;
        fseek(output, 1, SEEK_CUR);
    }

    while ((codigo[0] != aux[1]) || (codigo[1] != aux[2]) || (codigo[2] != aux[3])){
        contaDistancia += aux[0] + 1;
        fseek(output, aux[0] + 1, SEEK_CUR); // Vai para o próximo código em cadastro.dat
        if (fread(aux, sizeof(char), 4, output) == 0){
            printf("\nCodigo nao encontrado\n");
            return;
        }
        else{
            if ((codigo[0] != aux[1]) || (codigo[1] != aux[2]) || (codigo[2] != aux[3]))
                fseek(output, -4, SEEK_CUR);
        }
    }

    if (!primeiroReg)
        fseek(output, -3, SEEK_CUR); // Assim que achar, volta e coloca o asterisco: 37001 -> 37*01
    char asterisco = '*';
    fwrite(&asterisco, sizeof(char), 1, output);

    fseek(output, 2, 0); // Salva o próximo da pilha ('*' se não tiver) em proximo
    char proximo;
    fread(&proximo, sizeof(char), 1, output);

    fseek(output, -1, SEEK_CUR);
    fwrite(&contaDistancia, sizeof(char), 1, output); // Escreve a distância do mais recentemente removido no cabeçalho

    fseek(output, contaDistancia + 2, 0);      // Vai para o mais recentemente removido
    fwrite(&proximo, sizeof(char), 1, output); // Coloca o próximo da pilha, se for asterisco é o último (primeiro que foi colocado)
}

void compacta(FILE *output){
    // Precisa lembrar de zerar a pilha aqui mas como ainda não tem pilha não fiz
    // Também precisa mudar o tamanho dos registros naquele primeiro bit já que encurta eles
    fseek(output, 0, 0);
    FILE *output2 = output;
    int contador = 0, i;
    char ch, proximo, leitura, buffer[sizeof(seg)];

    while (fread(&ch, sizeof(char), 1, output) != 0){
        if (contador == 0)
            proximo = ch;

        if (ch = '#')
            contador++;

        if (contador == 4){
            fseek(output2, proximo, 1);
            fread(&leitura, sizeof(char), 1, output2);
            fread(buffer, sizeof(char), leitura, output2);

            fwrite(&leitura, sizeof(char), 1, output);
            fwrite(buffer, sizeof(char), leitura, output);
            // fseek(output, tamanho atualizado, SEEK_CUR);
            contador = 0;
        }
    }
}

int main(){
    FILE *output = fopen("cadastro.dat", "r+b");
    if (!output)
        output = fopen("cadastro.dat", "w+b");
    montaCabecalho(output);

    FILE *input1 = fopen("insere.bin", "rb");
    FILE *input2 = fopen("remove.bin", "rb");

    char escolha;
    do{
        printf("\n---------------------------\n\nO que deseja fazer?\n\n1- Inserir\n2- Remover\n3- Compactar\n4- Sair\n");
        scanf(" %c", &escolha);
        switch (escolha){
        case '1':{
            montaCampos(input1, output);
            break;
        }
        case '2':{
            removeReg(input2, output);
            break;
        }
        case '3':{
            break;
        }
        }
    } while (escolha != '4');

    fclose(input1);
    fclose(input2);
    fclose(output);
}