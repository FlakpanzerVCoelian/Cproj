#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define buflen 100000


#include "strutture.h"


int checkstack(elem* cima, int profondita) { //controllo che ci siano elementi sufficienti nello stack
    elem* tmp = cima;
    for (int i = 0; i < profondita; i++){
        if (tmp == NULL) {
            printf("Elementi insufficienti nella pila per eseguire l' operazione (errore), ne servono %d ma ce ne sono solo %d\n", profondita, i);
            exit(1);
        };
        tmp = tmp->quello_sotto;
    }
    return 0;
}

int checksegflt (float* vett) {
    if (vett == NULL) {
        printf("Errore allocazione memoria dinamica (segfault intercettato (GRAVE))");
        exit(-1);
    }
    return 0;
}

int checklen (elem* cima, int profondita) { //controllo la compatibilita di grandezza tra elementi
    elem *tmp = cima;
    elem *sotto = cima->quello_sotto;
    for (int i = 1; i < profondita; i++) {
    if (tmp->ogg->lunghezza != sotto->ogg->lunghezza) {
        printf("Errore: i due vettori hanno lunghezze diverse, non posso eseguire l'operazione (errore), (%d != %d)\n", tmp->ogg->lunghezza, sotto->ogg->lunghezza);
        exit(1);
    }
    if (tmp->ogg->altezza != sotto->ogg->altezza) {
        printf("Errore: i due vettori hanno altezze diverse, non posso eseguire l'operazione (errore), (%d != %d)\n", tmp->ogg->altezza, sotto->ogg->altezza);
        exit(1);
    }
    }
    return 0;
}

int checktipo(elem* cima, int profondita, int tipo, int strict) { //controllo la compatibilita di tipo tra gli elementi
    elem *tmp = cima;
    for (int i = 0; i < profondita; i++){
        if (tmp->ogg->tipo != tipo) {
            if (tipo == numerico && tmp->ogg->tipo == floatbinario && strict == 0) continue; //le operazioni tra float e binari in teoria vanno bene
            else if (tipo == floatbinario && tmp->ogg->tipo == numerico && strict == 0) continue; //se non e` STRICT
            else {
                printf("Mismatch tra tipi per l' operazione (errore)");
                exit(1);
            }
        }
        tmp = tmp->quello_sotto;
    }
    return 0;
}
