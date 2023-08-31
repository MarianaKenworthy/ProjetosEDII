#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* To-do:
- Compactação (quando tudo estiver pronto);
- Deixar bonitinho - Ponteiro a mais;
*/

typedef struct segurado{
    char codigo[4];
    char nome[50];
    char seguradora[50];
    char tipo[30];
} seg;

void montaCabecalho(FILE *output){
    char tam;
    char registro[3] = {'*', -1, '|'};
    /* Assim ficaria {3*-1|}, onde:
     * '*' Indica que é removido para não ler em outros momentos, mas no caso é o cabeçalho dos removidos
     * '-1' Indica que por enquanto nenhum foi removido, porém vai mudar para a distância do mais recentemente removido
     * '|' Indica o começo do arquivo de verdade, foi um jeito mais fácil de arrumar esse problema, levar isso em conta nas funções
     */
    tam = strlen(registro);
    char checarSeExiste;
    fseek(output, 3, 0);
    fread(&checarSeExiste, sizeof(char), 1, output);
    if(checarSeExiste != '|'){
        fseek(output, 0, 0);
        fwrite(&tam, sizeof(char), 1, output);
        fwrite(&registro, sizeof(char), tam, output);
    }
}

bool achaEspaco (FILE* output, char tam){
    char ch[3];
    fseek(output, 0, 0);

    fread(ch, sizeof(char), 3, output);
     if(ch[2] == -1){
            fseek(output,0, 2);
            return false;
    }

    do {

        fseek(output, ch[2], 1);
        fread(&ch, sizeof(char), 3, output);
        
    } while(tam > ch[0]);

    fseek(output, -5, 1);

    return true;
}

void montaCampos(FILE *input, FILE *output, FILE *cursor){
    char tam, segInicialInsere;
    seg segurado;

    fseek(cursor, 0, 0);
    if (fread(&segInicialInsere, sizeof(char), 1, cursor) == 0){
        segInicialInsere = 0;
    }
    fseek(input, segInicialInsere * sizeof(seg), 0);
    if(fread(&segurado, sizeof(seg), 1, input) == 0){
        printf("Nao ha mais seguradoras no arquivo de insercao.");
        return;
    }

    segInicialInsere += 1;
    fseek(cursor, 0, 0);
    fwrite(&segInicialInsere, sizeof(char), 1, cursor);

    char registro[135];
    sprintf(registro, "%s#%s#%s#%s#", segurado.codigo, segurado.nome, segurado.seguradora, segurado.tipo);
    tam = strlen(registro);
    if(!achaEspaco(output, tam))
        fwrite(&tam, sizeof(char), 1, output);

    fwrite(&registro, sizeof(char), tam, output);
}

void removeReg(FILE *input, FILE *output, FILE *cursor){
    char codigo[4], aux[4], verificaRemovido, segInicialRemove;
    fseek(cursor, 1, 0);
    if (fread(&segInicialRemove, sizeof(char), 1, cursor) == 0){
        segInicialRemove = 0;
    }
    fseek(input, segInicialRemove * 4, 0); // Cada código no remove.bin tem tamanho 4 (002.)
    if(fread(codigo, sizeof(char), 4, input) == 0){
        printf("Nao ha mais seguradoras no arquivo de remocao.");
        return;
    } // Lê o codigo a ser removido do remove.bin

    segInicialRemove += 1;
    fseek(cursor, 1, 0); // O fseek(cursor, 0, 0) é para o insere.bin
    fwrite(&segInicialRemove, sizeof(char), 1, cursor);

    char contaDistancia = 4;

    fseek(output, 4, 0);                  // Garante que está no começo (4 por conta do cabeçalho)
    fread(&aux, sizeof(char), 4, output); // Lê o código em cadastro.dat, para comparar com o remove.bin

    fseek(output, 4, 0);
    bool primeiroReg = false;
    if ((codigo[0] == aux[1]) && (codigo[1] == aux[2]) && (codigo[2] == aux[3])){
        primeiroReg = true;
        fseek(output, 1, SEEK_CUR);
    }

    while ((codigo[0] != aux[1]) || (codigo[1] != aux[2]) || (codigo[2] != aux[3])){
        contaDistancia += aux[0] + 1;
        fseek(output, aux[0] +1, SEEK_CUR); // Vai para o próximo código em cadastro.dat
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

    fseek(output, 2, 0); // Salva o próximo da pilha ('-1' se não tiver) em proximo
    char proximo;
    fread(&proximo, sizeof(char), 1, output);

    fseek(output, -1, SEEK_CUR);
    fwrite(&contaDistancia, sizeof(char), 1, output); // Escreve a distância do mais recentemente removido no cabeçalho

    fseek(output, contaDistancia + 2, 0);      // Vai para o mais recentemente removido
    fwrite(&proximo, sizeof(char), 1, output); // Coloca o próximo da pilha, se for -1 é o último (primeiro que foi colocado)
}

void compacta(FILE **output){
    // Também precisa mudar o tamanho dos registros naquele primeiro bit já que encurta eles
    fseek(*output, 4, 0);
    FILE *output2 = fopen("output2.dat", "w+b");
    int contador = 0, j;
    char ch, tam, i, buffer[sizeof(seg)];

    montaCabecalho(output2);
    fread(&tam, sizeof(char), 1, *output);
    while (fread(buffer, sizeof(char), tam, *output) != 0){
        for (i=0; i<tam; i++){
            if(buffer[0] != '*'){
                if(buffer[i]=='#')
                    contador++;

                if(contador==4){
                    fwrite(&i, sizeof(char), 1, output2);
                    fwrite(buffer, sizeof(char), i+1, output2);
                    fseek(*output, i-tam+1, 1);
                    contador=0;
                }
            }
        }
        fread(&tam, sizeof(char), 1, *output);
        i=0;
    }
    remove("cadastro.dat");
    rename("output2.dat", "cadastro.dat");
    fclose(*output);
    fclose(output2);
    
    
}

int main(){
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
    do{
        printf("\n---------------------------\n\nO que deseja fazer?\n\n1- Inserir\n2- Remover\n3- Compactar\n4- Sair\n");
        scanf(" %c", &escolha);
        switch (escolha){
        case '1':{
            montaCampos(input1, output, cursor);
            break;
        }
        case '2':{
            removeReg(input2, output, cursor);
            break;
        }
        case '3':{
            compacta(&output);
            output = fopen("cadastro.dat", "r+b");
            break;
        }
        }
    } while (escolha != '4');

    fclose(input1);
    fclose(input2);
    fclose(output);
    fclose(cursor);
}