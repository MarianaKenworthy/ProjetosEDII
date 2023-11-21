#include <stdio.h>
#define MAXKEYS 3
#define MINKEYS MAXKEYS/2
#define NIL (-1)
#define NOKEY '@'
#define NO 0
#define YES 1


typedef struct {
    short keycount; // number of keys in page
    char key[MAXKEYS]; // the actual keys
    short child[MAXKEYS+1]; // ptrs to rrns of descendants
} BTPAGE;

typedef struct segurado{
    char codigo[4];
    char nome[50];
    char seguradora[50];
    char tipo[30];
} seg;



#define PAGESIZE sizeof(BTPAGE)
extern short root; // rrn of root page
extern FILE* btfd; // file descriptor of btree file
extern FILE* infd; // file descriptor of input file
/* prototypes */

void btclose ();
FILE* btopen (); 
int btread (short rrn, BTPAGE *page_ptr);
int btwrite (short rrn, BTPAGE *page_ptr);
int create_root (char key, short left, short right);
int receive_input();
short create_tree();
short getpage ();
short getroot ();
int insert (short rrn, char key, short *promo_r_child, char *promo_key);
void ins_in_page (char key,short r_child, BTPAGE *p_page);
void pageinit (BTPAGE *p_page);
void putroot(short root);
int search_node (char key, BTPAGE *p_page, short *pos);
void split(char key, short r_child, BTPAGE *p_oldpage, char *promo_key, short *promo_r_child, BTPAGE *p_newpage);