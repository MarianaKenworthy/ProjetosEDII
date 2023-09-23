typedef struct segurado
{
    char codigo[4];
    char nome[50];
    char seguradora[50];
    char tipo[30];
} seg;

void montaCabecalho(FILE *output);
bool achaEspaco(FILE *output, char tam);
void montaCampos(FILE *input, FILE *output, FILE *cursor);
void removeReg(FILE *input, FILE *output, FILE *cursor);
void compacta(FILE **output);