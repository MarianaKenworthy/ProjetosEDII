#include <stdio.h>
#include "teste.h"

int main() {

    int promoted; // boolean: tells if a promotion from below
    short root, // rrn of root page
    promo_rrn; // rrn promoted from below
    char promo_key, // key promoted from below
    key; // next key to insert in tree

    if (btopen()){
        root = getroot();
    }
    else {
        root = create_tree();
    }







   


    //vai ter q adaptar isso aqui pra receber dados, provavelmente uma funçãozinha extra pra receber os dados do txt e jogar nesse insert
    while ((key = getchar()) != 'q'){
        promoted = insert(root, key, &promo_rrn, &promo_key);
        if (promoted)
            root = create_root(promo_key, root, promo_rrn);
    }
    btclose();
}