#ifndef _STRUTTTURE_H
#define _STRUTTTURE_H
#include <stdint.h> 


#define BUFLEN 256

typedef struct el { 
    int tipo; //definisce il tipo dell'oggetto che viene letto
    int lunghezza; //N, Num. colonne in una matrice MxN
    int altezza; //M, 1 se 1D
    float *v;
    char *path;
    int refcount;
    uint8_t *sudisco;
} el;

typedef struct ele {
    struct el *ogg;
    struct ele *quello_sotto;
    struct ele *quello_sopra;
} elm, elem;

enum {
    floatbinario = 0,
    filepath = 1,
    numerico = 2
};


#endif