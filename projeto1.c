#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "projeto1.h"

void montaCabecalho(FILE *output){
    char tam;
    char asterisco = '*';
    int proxReg = -1;
    char pipeline = '|';
    /* Assim ficaria {3*-1|}, onde:
     * '*' Indica que é removido para não ler em outros momentos, mas no caso é o cabeçalho dos removidos
     * '-1' Indica que por enquanto nenhum foi removido, porém vai mudar para a distância do mais recentemente removido
     * '|' Indica o começo do arquivo de verdade, foi um jeito mais fácil de arrumar esse problema, levar isso em conta nas funções
     */
    tam = sizeof(char) * 2 + sizeof(int);
    char checarSeExiste;
    fseek(output, 6, 0);
    fread(&checarSeExiste, sizeof(char), 1, output);
    if (checarSeExiste != '|'){
        fseek(output, 0, 0);
        fwrite(&tam, sizeof(char), 1, output);
        fwrite(&asterisco, sizeof(char), 1, output);
        fwrite(&proxReg, sizeof(int), 1, output);
        fwrite(&pipeline, sizeof(char), 1, output);
    }
}

bool achaEspaco(FILE *output, char tam){
    char tamanhoRegistroAtual, asterisco, pipe;
    int proximoReg;
    int posAtual = -1, posFinal, posAnterior = -1;
    fseek(output, 0, 0);

    fread(&tamanhoRegistroAtual, sizeof(char), 1, output);
    fread(&asterisco, sizeof(char), 1, output);
    fread(&proximoReg, sizeof(int), 1, output);
    fread(&pipe, sizeof(char), 1, output);
    if (proximoReg == -1){
        // Caso não tenha nenhuma seguradora removida
        fseek(output, 0, 2);
        return false;
    }

    do{
        // Tudo isso aqui é para guardar as posições e arrumar a pilha de remoção ao inserir
        // 3*2| ; 3*-1 ; 3*3 ; 3*1   (ao inserir no lugar do terceiro)
        // 3*2| ; 3*-1 ; 3*1 ; ...   (onde 3 é os bytes, * mostra que estão removidos e o último o próximo da pilha de remoção)
        posAnterior = posAtual;
        posAtual = proximoReg;
        if (posAtual == -1)
            return false;
        fseek(output, proximoReg, 0);
        if (fread(&tamanhoRegistroAtual, sizeof(char), 1, output) == 0)
            return false;
        if (fread(&asterisco, sizeof(char), 1, output) == 0)
            return false;
        if (fread(&proximoReg, sizeof(int), 1, output) == 0)
            return false;
        if (fread(&pipe, sizeof(char), 1, output) == 0)
            return false;
        posFinal = proximoReg;
    } while (tam > tamanhoRegistroAtual);

    if (posAnterior == -1)
        fseek(output, posAnterior + 3, 0);
    else
        fseek(output, posAnterior + 2, 0);
    fwrite(&posFinal, sizeof(int), 1, output);
    fseek(output, posAtual + 1, 0); // Deixa o cursor exatamente no local para inserir
    return true;
}

void montaCampos(FILE *input, FILE *output, FILE *cursor){
    char tam, segInicialInsere;
    seg segurado;

    fseek(cursor, 0, 0);
    if (fread(&segInicialInsere, sizeof(char), 1, cursor) == 0){    
        // Checa se o cursor externo de inserção existe
        segInicialInsere = 0;
    }
    fseek(input, segInicialInsere * sizeof(seg), 0); // Coloca o cursor no lugar onde parou da última vez (se houve)
    if (fread(&segurado, sizeof(seg), 1, input) == 0){
        printf("Nao ha mais seguradoras no arquivo de insercao.");
        return;
    }

    segInicialInsere += 1;
    fseek(cursor, 0, 0);
    fwrite(&segInicialInsere, sizeof(char), 1, cursor); // Coloca o cursor externo de inserção novo no arquivo dos cursores

    char registro[135];
    sprintf(registro, "%s#%s#%s#%s#", segurado.codigo, segurado.nome, segurado.seguradora, segurado.tipo);
    tam = strlen(registro);
    if (!achaEspaco(output, tam)){
        fseek(output, 0, 2);                   // Se não houve espaço sobrando, coloca no final
        fwrite(&tam, sizeof(char), 1, output); // Se não achou espaço, coloca o tamanho do segurado
    }
    // Caso contrário, somente o segurado, assim o tamanho em bytes continua o tamanho original, com o lixo
    fwrite(&registro, sizeof(char), tam, output);
}

void removeReg(FILE *input, FILE *output, FILE *cursor){
    char codigo[4], aux[4], verificaRemovido, segInicialRemove;

    fseek(cursor, 1, 0); // fseek(cursor, 0, 0) é para o cursor de inserção
    if (fread(&segInicialRemove, sizeof(char), 1, cursor) == 0){
        // Checa se o cursor externo de remoção existe
        segInicialRemove = 0;
    }

    fseek(input, segInicialRemove * 4, 0); // Cada código no remove.bin tem tamanho 4 (002.)
    if (fread(codigo, sizeof(char), 4, input) == 0){
        printf("Nao ha mais seguradoras no arquivo de remocao.");
        return;
    } // Lê o codigo a ser removido do remove.bin

    segInicialRemove += 1;
    fseek(cursor, 1, 0);
    fwrite(&segInicialRemove, sizeof(char), 1, cursor); // Coloca o cursor externo de remoção novo no arquivo dos cursores

    int contaDistancia = 7; // Começa no 7 por conta do cabeçalho

    fseek(output, 7, 0);                  // Garante que está no começo
    fread(&aux, sizeof(char), 4, output); // Lê o código em cadastro.dat, para comparar com o remove.bin

    fseek(output, 7, 0);
    bool primeiroReg = false;
    // Checagem para o caso específico de a primeira checagem der certo, ou seja:
    // codigo[4] = 001. | aux[4] = B001 (B é o tamanho em bytes do segurado)
    if ((codigo[0] == aux[1]) && (codigo[1] == aux[2]) && (codigo[2] == aux[3])){
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
            // if para voltar o cursor, pois leu 4 a mais no fread em cima
        }
    }

    if (!primeiroReg)                // Assim que achar, volta e coloca o asterisco: 37001 -> 37*01
        fseek(output, -3, SEEK_CUR); // Booleano ali pois naquele caso não entra no fread do while, para ter que voltar 3
    char asterisco = '*';
    fwrite(&asterisco, sizeof(char), 1, output);

    fseek(output, 2, 0); // Salva o próximo da pilha ('-1' se não tiver) em proximo
    int proximo;
    fread(&proximo, sizeof(int), 1, output);

    fseek(output, -4, SEEK_CUR);
    fwrite(&contaDistancia, sizeof(int), 1, output); // Escreve a distância do mais recentemente removido no cabeçalho

    fseek(output, contaDistancia + 2, 0);     // Vai para o mais recentemente removido
    fwrite(&proximo, sizeof(int), 1, output); // Coloca o próximo da pilha, se for -1 é o último (primeiro que foi colocado)
}

void compacta(FILE **output){
    // Para compactar, criamos outro arquivo só com as partes que são necessárias, que toma o lugar do original, removendo fragmentações
    fseek(*output, 7, 0); // 4 por conta do tamanho do cabeçalho
    FILE *output2 = fopen("output2.dat", "w+b");
    int contador = 0;
    char ch, tam, i, buffer[sizeof(seg)];

    montaCabecalho(output2); // Cabeçalho no arquivo novo, do mesmo método que é feito originalmente
    fread(&tam, sizeof(char), 1, *output);
    while (fread(buffer, sizeof(char), tam, *output) != 0){
        for (i = 0; i < tam; i++){
            if (buffer[0] != '*'){ 
                // Se foi removido, não entra no arquivo novo
                if (buffer[i] == '#') // Conta as '#' para arrumar a fragmentação
                    contador++;

                if (contador == 4){
                    i++;
                    fwrite(&i, sizeof(char), 1, output2);     // Escreve o quanto leu, se houve fragmentação será menor que "tam"
                    fwrite(buffer, sizeof(char), i, output2); // Escreve o segurado
                    contador = 0;
                    break; // Feito para não ler o lixo
                }
            }
            else
                break;
        }
        fread(&tam, sizeof(char), 1, *output); // Pega o tamanho do próximo segurado em cadastro.dat
        i = 0;
    }
    fclose(*output);
    fclose(output2);
    remove("cadastro.dat");
    rename("output2.dat", "cadastro.dat"); // Assim apaga o cadastro antigo e um novo compactado toma seu lugar
}