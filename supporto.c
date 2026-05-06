#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>


#include "strutture.h"

int numSIoNO(char *x) {  //per un carattere mi dice se e` un numero, un . o qualcos'altro
    if (isdigit(*x)) {
        return 1;
    }
    else if (*x == '.') {
        return 2;
    }
    else {
        return -1;
    }
}


void leggitensore(FILE* fd, el* ogg) { 
    char* buffer = malloc(sizeof(char) * BUFLEN);
    int sizeINIT = 10; //scelta arbitraria
    ogg->altezza = 1; //SEMPRE 1 QUANDO LEGGO
    ogg->tipo = numerico;
    ogg->lunghezza = 0;
    float* vettTEMP = (float*) malloc(sizeINIT * sizeof(float));
    int fb = floatbinario;

    if (vettTEMP == NULL) {
        printf("Errore malloc vettore (errore GRAVE (segmentation fault possibile))\n");
        exit(1);
    }

    while (fscanf(fd, "%255s", buffer) == 1) {
        if (strcmp(buffer, "]") == 0) { //fine tensore (termina ciclo)
            break;
        }
        
        float numeroTROVATO = strtof(buffer, NULL); //str->float
        if (numeroTROVATO != 0.0f && numeroTROVATO != 1.0f) fb = numerico   ;
        if (ogg->lunghezza >= sizeINIT) {
            sizeINIT *= 2;
            vettTEMP = (float*) realloc(vettTEMP, sizeINIT * sizeof(float));
            if (vettTEMP == NULL) {
                printf("Errore realloc vettore (errore GRAVE (segfault intercettato))\n");
                exit(1);
            }
        }
        
        vettTEMP[ogg->lunghezza] = numeroTROVATO;

        //printf("DEBUG: token='%s' -> convertito=%f -> salvato=%f\n", buffer, numeroTROVATO, vettTEMP[ogg->lunghezza]); //DEBUG

        ogg->lunghezza++;
    }

    float* vettoreDEF = (float*) realloc(vettTEMP, ogg->lunghezza * sizeof(float)); //ridimensiono al numero esatto di elementi trovati 
    //NOTA: REALLOC NON MALLOC TU CERVELLO DISLESSICO (30 minuti persi...)
    if (vettoreDEF == NULL) {
        printf("Errore malloc vettore (errore GRAVE (segmentation fault possibile))\n");
        exit(1);
    }

    ogg->v = vettoreDEF;
    
    printf("Trovato un vettore!! Altezza: %d, Lunghezza: %d\n", ogg->altezza, ogg->lunghezza); // debug

    return fb;
}
