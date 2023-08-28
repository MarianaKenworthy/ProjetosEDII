#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

/* to-do
- onde parou no arquivo cadastro, insere, e remove
- first-fit do insere
- compactação (quando tudo estiver pronto)
- deixar bonitinho (🏳‍🌈)
*/

typedef struct segurado{
    char codigo[4];
    char nome[50];
    char seguradora[50];
    char tipo[30];
} seg;

struct pilha{
    int info;
    struct pilha *prox;
};

void push (int x, struct pilha **topo){
    struct pilha *aux;
    aux = (struct pilha *)malloc(sizeof(struct pilha));
    aux->info = x;
    aux->prox = *topo;
    *topo = aux;
}

void pop (struct pilha **p, int *x){
    struct pilha *aux;
    if(*p){
        *x = (*p)->info;
        aux = *p;
        *p = aux->prox;
        free(aux);
    }else{
        printf("vazia");
    }
}

void montaCabecalho (FILE* output){
    char tam;
    char registro[4];
    sprintf(registro, "**|");
    //assim ficaria {3**|}, onde:
    //*indica q é removido pra n ler em outros momentos, mas no caso é o cabeçalho dos removidos
    //*indica q por enquanto nenhum foi removido, porém vai mudar pra distância do mais recentemente removido
    //|indica o começo do arquivo de verdade, foi um jeito mais fácil de arrumar esse problema, vai ter q mudar algumas coisas
    tam = strlen(registro);

    fwrite(&tam, sizeof(char), 1, output);
    fwrite(&registro, sizeof(char), tam, output);
}

void montaCampos (FILE* input, FILE*output){
    char tam;
    seg segurado; 
    fread(&segurado, sizeof(seg), 1, input);
    char registro[135];
    sprintf(registro, "%s#%s#%s#%s#", segurado.codigo, segurado.nome, segurado.seguradora, segurado.tipo);
    tam = strlen(registro);

    fwrite(&tam, sizeof(char), 1, output);
    fwrite(&registro, sizeof(char), tam, output);
}

void removeReg (FILE* input, FILE* output, struct pilha **offset){//esse offset tá fazendo algo? (offset, pop, push, struct da pilha)
    char codigo[4], aux[4], verificaRemovido;
    fread(codigo, sizeof(char), 4, input); //le o codigo a ser removido

    char contaDistancia = 4;

    fseek(output, 4, 0);// garante que ele ta no começo (4 por conta do cabeçalho)
    fread(&aux, sizeof(char), 4, output);
    fseek(output, 4, 0);

    while((codigo[0]!=aux[1])||(codigo[1]!=aux[2])||(codigo[2]!=aux[3])){
        contaDistancia += aux[0] + 1;
        fseek(output, aux[0]+1, SEEK_CUR);
        if(fread(aux, sizeof(char), 4, output) == 0){
            printf("\nCodigo nao encontrado\n");
            return;
        }
    }
    //volta e coloca o asterisco: 37001 -> 37*01
    fseek(output, -3, SEEK_CUR);
    if ((fread(&verificaRemovido, 1, 1, output)) == 1){
        if (verificaRemovido == '*'){
            printf("\nEsse registro ja foi removido\n");
        }else{
            fseek(output, -1, SEEK_CUR);
            char asterisco = '*';
            fwrite(&asterisco, sizeof(char), 1, output);

            fseek(output, 2, 0);
            char proximo;
            fread(&proximo, sizeof(char), 1, output);
            fseek(output, -1, SEEK_CUR);
            fwrite(&contaDistancia, sizeof(char), 1, output);//no cabeçalho -> 2*87; proximo vai ser oq tava lá

            fseek(output, contaDistancia+2, 0);//vai pra onde tava antes agora com o proximo feito
            fwrite(&proximo, sizeof(char), 1, output);//coloca o próximo, se for asterisco é o último da pilha (primeiro q foi colocado)
        }
    }
}

void compacta (FILE* output){
    //precisa lembrar de zerar a pilha aqui mas como ainda não tem pilha nao fiz tambem precisar mudar o tamanho dos registros naquele primeiro bit
    //já que encurta eles
    fseek(output, 0, 0);
    FILE* output2 = output;
    int contador = 0, i;
    char ch, proximo, leitura, buffer[sizeof(seg)];

    while(fread(&ch, sizeof(char), 1, output)!=0){
        if(contador == 0)
            proximo = ch;

        if(ch = '#')
            contador++;

        if(contador==4){ //eu desenhei essa parte se tu quiser
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


int main () {
    FILE* output = fopen("cadastro.dat", "r+b");
    if (!output)
    output = fopen("cadastro.dat", "w+b");
    montaCabecalho(output);

    FILE* input1 = fopen("insere.bin", "rb");
    FILE* input2 = fopen("remove.bin", "rb");

    struct pilha *offset = NULL;

    char escolha;
    do {
    printf("O que deseja fazer?\n\n1- Inserir\n2- Remover\n3- Compactar\n4- Sair\n");
    scanf(" %c", &escolha);

        switch(escolha){
            case '1': {
                
                montaCampos(input1, output);

            break;
            }

            case '2':{
                
                removeReg(input2, output, &offset);

            break;
            }
            case '3':

            break;

        }

    } while(escolha != '4');

    fclose(input1);
    fclose(input2);
    fclose(output);
}