/* insert.c
Contains insert() function to insert a key into a btree.
Calls itself recursively until bottom of tree is reached.
Then insert key node.
If node is out of room,
- calls split() to split node
- promotes middle key and rrn of new node
*/

/*TO DO
    - implemntar cabeçalho
    - receber dados direto do documento feito??????
    - percurso pra ler todos os dados
    - encontrar um especifico (busca)
    - conferir se o programa promove indice 2
    - conferir as mensagens que ela quer que imprima na insercao (tem q testar e conferir se condiz com o enunciado)*/



#include "teste.h"
#include <stdio.h>
#include <stdlib.h>


int insert(short rrn, char key, short *promo_r_child, char *promo_key){
    BTPAGE page,         // current page
        newpage;         // new page created if split occurs
    int found, promoted; // boolean values
    short pos,
        p_b_rrn;  // rrn promoted from below
    char p_b_key; // key promoted from below
    
    if (rrn == NIL){
        *promo_key = key;
        *promo_r_child = NIL;
        return (YES);
    }

    btread(rrn, &page);
    found = search_node(key, &page, &pos);
    if (found){
        printf("Chave %c duplicada \n", key);
        return (NO);
    }

    promoted = insert(page.child[pos], key, &p_b_rrn, &p_b_key);
    if (!promoted) {
        return (NO);
    }

    printf("chave %c promovida \n", key);

    if (page.keycount < MAXKEYS){
        ins_in_page(p_b_key, p_b_rrn, &page);
        btwrite(rrn, &page);
        return (NO);
    }

    else {
        split(p_b_key, p_b_rrn, &page, promo_key, promo_r_child, &newpage);
        printf("Divisao de no\n");
        btwrite(rrn, &page);
        btwrite(*promo_r_child, &newpage);
        return (YES);
    }
}

FILE* btfd; // global file descriptor for "btree.dat"

int receive_input(){ // colocar cursor aqui
    FILE* infd = fopen("insere.bin", "rb");
    FILE* cadastro =fopen("cadastro.dat", "r+b");
    if (!cadastro)
        cadastro = fopen("cadastro.dat", "w+b");

    seg segurado;
    char buffer [sizeof(seg)];

     if (fread(&segurado, sizeof(seg), 1, infd) == 0){
        printf("Nao ha mais seguradoras no arquivo de insercao.");
        fclose(infd);
        fclose(cadastro);
        return ;
    }

    int res = insert(ftell(cadastro), segurado.codigo, NULL, NULL); //conferir se é isso mesmo

    if(res)
        fwrite(&segurado, 1, sizeof(seg), cadastro);

    fclose(infd);
    fclose(cadastro);
    return res;

}

FILE* btopen() { 
    FILE *btfd = fopen("btree.dat", "r+b");
    if (!btfd)
        btfd = fopen("btree.dat", "w+b");
    return btfd;
}

void btclose()
{
    fclose(btfd);
}

short getroot() {
    short root;
    fseek(btfd, 0, 0); //talvez tenha q olhar isso aqui por causa de cabeçalho
    if (fread(&root, sizeof(short), 1, btfd) == 0){
        printf("Erro: Nao foi possivel encontrar a raiz. \007\n");
        exit(1);
    }
    return (root);
}

void putroot(short root) {
    fseek(btfd, 0, 0);
    fwrite(&root, sizeof(short), 1, btfd);
}

short create_tree() {
    char key;
    btfd = fopen("btree.dat", "w+b"); //botar cabeçalho posição -1 e os caralho
    fclose(btfd);
    btopen();
    key = getchar();
    return (create_root(key, NIL, NIL));
}

short getpage() {
    long addr;
    addr = fseek(btfd, 0, 2) - 2L; //2L cabeçalho?
    //usar ftell
    return ((short)addr / PAGESIZE);
}

int btread(short rrn, BTPAGE *page_ptr){
    long addr;
    addr = (long)rrn * (long)PAGESIZE + 2;
    fseek(btfd, addr, 0);
    return (fread(page_ptr, PAGESIZE, 1, btfd));
}

int btwrite(short rrn, BTPAGE *page_ptr){
    long addr;
    addr = (long)rrn * (long)PAGESIZE + 2L;
    fseek(btfd, addr, 0);
    return (fwrite(page_ptr, 1, PAGESIZE, btfd));
}

/* btutil.c
Contains utility function for btree program
*/

int create_root(char key, short left, short right){
    BTPAGE page;

    short rrn;
    rrn = getpage();
    pageinit(&page);
    page.key[0] = key;
    page.child[0] = left;
    page.child[1] = right;
    page.keycount = 1;
    btwrite(rrn, &page);
    putroot(rrn);
    return (rrn);
}

void pageinit(BTPAGE *p_page){
    int j;
    for (j = 0; j < MAXKEYS; j++)
    {
        p_page->key[j] = NOKEY;
        p_page->child[j] = NIL;
    }
    p_page->child[MAXKEYS] = NIL;
}

int search_node(char key, BTPAGE *p_page, short *pos){
    int i;
    for (i = 0; i < p_page->keycount && key > p_page->key[i]; i++)
        ;
    *pos = i;
    if (*pos < p_page->keycount && key == p_page->key[*pos])
    {
        return (YES);
    }
    else
    {
        return (NO);
    }
}

void ins_in_page(char key, short r_child, BTPAGE *p_page){
    int j;
    for (j = p_page->keycount; key < p_page->key[j - 1] && j > 0; j--)
    {
        p_page->key[j] = p_page->key[j - 1];
        p_page->child[j + 1] = p_page->child[j];
    }
    p_page->keycount++;
    p_page->key[j] = key;
    p_page->child[j + 1] = r_child;
}


//somente pra ordem impar
void split(char key, short r_child, BTPAGE *p_oldpage, char *promo_key, short *promo_r_child, BTPAGE *p_newpage){
    int j;
    short mid;
    char workkeys[MAXKEYS + 1];

    short workchil[MAXKEYS + 2];
    for (j = 0; j < MAXKEYS; j++) {

        workkeys[j] = p_oldpage->key[j];
        workchil[j] = p_oldpage->child[j];
    }

    workchil[j] = p_oldpage->child[j];
    for (j = MAXKEYS; key < workkeys[j - 1] && j > 0; j--){

        workkeys[j] = workkeys[j - 1];
        workchil[j + 1] = workchil[j];
    }

    workkeys[j] = key;
    workchil[j + 1] = r_child;
    *promo_r_child = getpage();
    pageinit(p_newpage);

    for (j = 0; j < MINKEYS; j++) {
        p_oldpage->key[j] = workkeys[j];
        p_oldpage->child[j] = workchil[j];
        p_newpage->key[j] = workkeys[j + 1 + MINKEYS];
        p_newpage->child[j] = workchil[j + 1 + MINKEYS];
        p_oldpage->key[j + MINKEYS] = NOKEY;
        p_oldpage->child[j + 1 + MINKEYS] = NIL;
    }

    p_oldpage->child[MINKEYS] = workchil[MINKEYS];
    p_newpage->child[MINKEYS] = workchil[j + 1 + MINKEYS];
    p_newpage->keycount = MAXKEYS - MINKEYS;
    p_oldpage->keycount = MINKEYS;
    *promo_key = workkeys[MINKEYS];
}