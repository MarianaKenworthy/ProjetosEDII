#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

/* to-do
- tirar sizeof() da remoção (eu mesmo)
- compactação
- entrada dos casos de teste (numero 4)
- ver um jeito de a remoção abrir o output sempre no começo do arquivo pra procurar direito
- salvar tudo em arquivo pra se fechar não ter problema
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

void removeReg (FILE* input, FILE* output, struct pilha **offset, FILE* pilha){
    char codigo[4], aux[4], verificaRemovido;
    fread(codigo, sizeof(char), 4, input); //le o codigo a ser removido

    int contaDistancia = 0;

    fseek(output, 0, 0);// garante que ele ta no começo, pode mudar essa parte de lugar depois
    fread(aux, sizeof(char), 4, output);
    fseek(output, 0, 0);

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
    if ((fread(&verificaRemovido, 1, 1, output)) == 2){
        if (verificaRemovido == '*'){
            printf("\nEsse registro ja foi removido\n");
        }else{
            fwrite('*', sizeof(char), 1, output);

            //coloca no arquivo da pilha de removidos o que acabou de ser removido:
            //7637#
            //onde 76 é a distância até o removido (int), 37 é o espaço que foi removido (char) (pra inserir depois nesse lugar) e # é pipeline
            fwrite(&contaDistancia, sizeof(int), 1, pilha);
            fwrite(&aux[0]+1, sizeof(char), 1, pilha);
            fwrite('#', sizeof(char), 1, pilha);
            fseek(pilha, 0, 0);
        }
    }

    /*  
        seg seguradora;
        //char aux[2];
        char asterisco = '*';
        int i;

        /*  Esse fread pra ver se já tá removido n é exatamente correto, na hora vai ficar tudo embaralhado
        *  e n vai ser só avançar x vezes, tem q olhar a lista de ordem
        *  vai ficar por exemplo:  1 5 2 4 3 no arquivo, aí falta a lista de ordem dos registros, mudaria tudo
        
        if ((fread(&aux, 1, 2, output)) == 2){
            if (aux[2] == '*'){
                printf("\nEsse registro já foi removido\n");
            }else{
                fwrite(&asterisco, sizeof(char), 1, output);
                push(escolhido, offset);
            }
        }
    */
}

void compacta (FILE* output){ //precisa lembrar de zerar a pilha aqui mas como ainda não tem pilha nao fiz tambem precisar mudar o tamanho dos registros naquele primeiro bit já que encurta eles
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

    FILE* pilharemocao = fopen("pilha.bin", "r+b");
    if (!pilharemocao)
    pilharemocao = fopen("pilha.bin", "w+b");

    FILE* input1 = fopen("insere.bin", "rb");
    FILE* input2 = fopen("remove.bin", "rb");
    

    struct pilha *offset = NULL;

    char escolha;
    do {
    printf("O que deseja fazer?\n\n1- Inserir\n2- Remover\n3- Compactar\n4- Teste\n5- Sair\n");
    scanf(" %c", &escolha);

        switch(escolha){
            case '1': {
                
                montaCampos(input1, output);

            break;
            }

            case '2':{
                
                removeReg(input2, output, &offset, pilharemocao);

            break;
            }
            case '3':

            break;

            case '4':

            break;
        }

    } while(escolha != '5');
    
    fclose(input1);
    fclose(input2);
    fclose(output);
}