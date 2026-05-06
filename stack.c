#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "strutture.h"
#include "checks.h"



#define MAXDIM 2
#define buflen 100000


 /*
struct el { 
    int tipo; //definisce il tipo dell'oggetto che viene letto
    int lunghezza; //N, Num. colonne in una matrice MxN
    int altezza; //M, 1 se 1D
    float *v;
    char path[128];
    int offset; //offset nel file
};

struct ele {
    struct el *ogg;
    struct elem* quello_sotto;
    struct elem* quello_sopra;
};
typedef struct ele elem;
typedef struct el elm;  */





elem* push(elem* cima, el* oggetto) { //pusha un oggetto in cima
    elem* nuovo = malloc(sizeof(elem));
    if (nuovo == NULL) {
        perror("Errore malloc stack");
        exit(1);
    }
    nuovo->ogg = oggetto;
    nuovo->quello_sotto = cima;
    nuovo->quello_sopra = NULL;

    if (cima != NULL) {
        cima->quello_sopra = nuovo;
    }
    return nuovo;
}

elem* dupNONUNISTD(elem*cima) { //duplica l'elemento in cima PS. GIA` ESISTE DUP in unistd AAARGH (5 minuti persi)
    if (cima == NULL) {
        printf("Stack vuoto... (errore)");
        exit(1);
    }
    cima->ogg->refcount++;
    return push(cima, cima->ogg);
}

elem* over(elem*cima) { //fa l'operazione di over
    if (checkstack(cima, 2) == 0) {
        elem* secondo = cima->quello_sotto;
        secondo->ogg->refcount++;
        return push(cima, secondo->ogg);
    } else {
        printf("Elementi insufficienti nello stack per eseguire l'operazione OVER (quale operazione?)\n"); //Per i walkie-talkie (o radio portatili),
        exit(1); // over indica che ho finito la mia comunicazione e che libero la linea
    }
}

elem* pop(elem* cima) { //poppo l'elemento in cima
    if (cima == NULL) return NULL;

    elem* tmp = cima;
    elem* nuovo_cima = tmp->quello_sotto;

    if (nuovo_cima != NULL) {
        nuovo_cima->quello_sopra = NULL;
    }

    if (tmp->ogg != NULL) {
        if (tmp->ogg->refcount <= 1) { //se minore o uguale a 1, elimino (libero) i dati
            if (tmp->ogg->v != NULL) free(tmp->ogg->v);
            free(tmp->ogg);
        } else {

            tmp->ogg->refcount--; 
        }
    }
    
    free(tmp);
    return nuovo_cima;
}

elem* swap(elem* cima) { // scambia quello in cima con quello sotto
    if (checkstack(cima, 2) == 0) {
        elem* primo = cima;
        elem* sotto = cima->quello_sotto;
        primo->quello_sopra = sotto; 
        sotto->quello_sopra = NULL; // quello sotto punta al potenziale terzo elemento della pila
        if (sotto->quello_sotto != NULL) { //se 3 (o piu) 
            elem* sottosotto = sotto->quello_sotto;
            sotto->quello_sotto = primo;
            primo->quello_sotto = sottosotto; 
            sottosotto->quello_sopra = primo;
        }
        else { //se solo 2 elementi
            sotto->quello_sotto = primo;
            primo->quello_sotto = NULL;
        }
        return sotto;
    } else {
        printf("E` avvenuto un errore durante SWAP\n");
        exit(1);
    }

}

void stampa_corrente(elem* corrente) {
    if (corrente->ogg->tipo == filepath) {
            printf("Oggetto salvato in un file; Path: %s", corrente->ogg->path);
        }
    else if (corrente->ogg->altezza * corrente->ogg->lunghezza < 150){
        if (corrente->ogg->altezza == 1) {
                printf("Oggetto 1D di lunghezza %d\n[ ", corrente->ogg->lunghezza);
                for (int i = 0; i < corrente->ogg->lunghezza; i++) {
                    printf("%f ", corrente->ogg->v[i]);
                }
                printf("]\n");
            }
        else{
                printf("Oggetto 2D con lunghezza %d ed altezza %d\n[ ", corrente->ogg->lunghezza, corrente->ogg->altezza);
                for (int i = 0; i < corrente->ogg->altezza; i++) {
                    printf("[ ");
                    for (int j = 0; j < corrente->ogg->lunghezza; j++) {
                        printf("%f ", corrente->ogg->v[(i*corrente->ogg->lunghezza) + j ]);
                    }
                    printf("] ");
                }
                printf("]\n");
        }
    }
    else {
            printf("Oggetto con lunghezza %d ed altezza %d (molto grande)", corrente->ogg->lunghezza, corrente->ogg->lunghezza);
    }
}

void stampaSTACK(elem* cima) { //stampa lo stato dello stack a cominciare dalla cima
    elem* corrente = cima;
    printf("Stampa stack dall'alto verso il basso: \n");
    if (corrente == NULL) printf("Stack svuotato\n");
    while (corrente != NULL){
        stampa_corrente(corrente);
        corrente = corrente->quello_sotto;
    }
}
