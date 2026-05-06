#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "strutture.h"


#define buflen 1000



int scrivibin(elem* x){ //scrive un tensore in binario sul disco
    int zero = 0;
    FILE *fd = fopen(x->ogg->path, "wb");
    fwrite(&x->ogg->altezza, sizeof(int), 1,fd );
    fwrite(&zero, sizeof(int), 1,fd );
    fwrite(&x->ogg->lunghezza, sizeof(int), 1,fd );
    x->ogg->offset = 10;
    fwrite(&zero, sizeof(int), (x->ogg->offset),fd );
    for (int i = 0; i < x->ogg->altezza * x->ogg->lunghezza; i++){
        fwrite( &(x->ogg->v[i]), sizeof(float), 1, fd);
    }
    free(x->ogg->v);
    free(x);
    return 0;
    fclose(fd);
}


//non vale
int leggibin(elem* x) { //legge un tensore binario dal disco
    FILE *fd = fopen(x->ogg->path, "wb");
    int numero;
    fread(&numero, sizeof(int), 1, fd);
    x->ogg->tipo = numerico;
    x->ogg->altezza = numero;
    fread(&numero, sizeof(int), 1, fd);
    fread(&numero, sizeof(int), 1, fd);
    x->ogg->lunghezza = numero;
    fread(&numero, sizeof(int), 1, fd);
    fread(&numero, sizeof(int), 1, fd);
    x->ogg->offset = numero;
    for (int i = 0; i < x->ogg->offset; i++) fread(&numero, sizeof(int), 1, fd);
    //ora siamo dai dati
    float* ep = (float*) (malloc(sizeof(float) * (x->ogg->altezza * x->ogg->lunghezza)));
    for (int i = 0; i < x->ogg->altezza * x->ogg->lunghezza; i++) fread(&ep[i], sizeof(int), 1, fd);
    x->ogg->v = ep;
    return 0;
    fclose(fd);
}