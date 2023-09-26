typedef struct segurado {
    char codigo[4];
    char nome[50];
    char seguradora[50];
    char tipo[30];
} seg; 

typedef struct indice{
    char chave[4];
    int offset;
} ind;


void insere (FILE* input, FILE* output, FILE* indice);
void buscaP;
void buscaS;
