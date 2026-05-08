#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h> 
#include <sys/mman.h>

#include "strutture.h"
#include "stack.h"
#include "checks.h"
#include "supporto.h"


elem* leggibin(elem* cima) { //legge il tensore da file
    int fd = open(cima->ogg->path, O_RDONLY);
    if (fd == -1) {
        printf("Errore lettura tensore sul disco");
        exit(1);
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Lettura grandezza file fallita");
        close(fd);
        exit(1);
    }
    uint8_t *filedat = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (filedat == MAP_FAILED) {
        printf("mmap fallita");
        exit(1);
    }

    uint32_t altezza = *(uint32_t *)(filedat + 0);
    uint32_t lunghezza  = *(uint32_t *)(filedat + 8);
    uint32_t offset = *(uint32_t *)(filedat + 16);

    printf("Tensore sul disco letto: Altezza=%u, Lunghezza=%u, Offset dati=%u\n", altezza, lunghezza, offset);

    // Safety check: ensure the offset doesn't point past the end of the file!
    if (offset >= sb.st_size) {
        printf("Errore intercettato, mismatch offset e lunghezza tensore (segfault)");
        exit(1);
    }
    uint8_t *datiVETTORE = filedat + offset;
    float *vettore = (float *)datiVETTORE;
    cima = pop(cima);
    el* ogge = malloc(sizeof(el));
    cima = push(cima, ogge);
    cima->ogg->refcount = 1;
    cima->ogg->tipo = numerico;
    cima->ogg->sudisco = filedat;
    cima->ogg->v = vettore;
    cima->ogg->altezza = (int) altezza;
    cima->ogg->lunghezza = (int) lunghezza;

    close(fd);

    return cima;
}



elem* scrivibin(elem* cima) { //scrive il tensore su file
if (checkstack(cima, 2) == 0) { //ho dimenticato di farlo in precedenza...
    elem* cimaa = cima->quello_sotto; //cima il tensore sta sotto il filename
    uint32_t lunghezza = (u_int32_t) cimaa->ogg->lunghezza;
    uint32_t altezza =  (u_int32_t) cimaa->ogg->altezza;
    uint32_t offset = 64; 
    size_t lunghezzatot = offset + lunghezza * altezza * sizeof(float);
    int fd = open(cima->ogg->path, O_RDWR | O_CREAT | O_TRUNC , 0666); //nooo il numero del diavolo
    if (fd == -1) {
        printf("Errore scrittura tensore su disco\n");
        exit(1);
    }

    if (ftruncate(fd, lunghezzatot) == -1) {
        printf("Errore allocazione memoria per la scrittura del file");
        close(fd);
        exit(1);
    }
    uint8_t *mmapaperta = mmap(NULL, lunghezzatot, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd); 
    if (mmapaperta == MAP_FAILED) {
        printf("mmap fallita");
        exit(1);
    }
    *(uint32_t *)(mmapaperta + 0)  = altezza;
    *(uint32_t *)(mmapaperta + 8)  = lunghezza;
    *(uint32_t *)(mmapaperta + 16) = offset;
    float *vettore = (float*) (mmapaperta + offset);
    for (int i = 0; i < (lunghezza * altezza); i++) {
        vettore[i] = cimaa->ogg->v[i];
    }
    printf("Scritto %zu byte su %s\n", lunghezzatot, cima->ogg->path);
    if (msync(mmapaperta, lunghezzatot, MS_SYNC) == -1) {
        printf("msync fallita (non e` un errore), il file potrebbe non essere presente in memoria in questo dato istante");
    }
    munmap(mmapaperta, lunghezzatot);

    cima = pop(cima);
    cima = pop(cima);
    } else {
        printf("Errore in checkstack\n");
        exit(1);
    }
    return cima;
}

elem* leggiPGM(elem* cima) {
    if (checkstack(cima, 1) == 0 && checktipo(cima, 1, filepath, 0) == 0) {
        FILE *fd = fopen(cima->ogg->path, "rb");
        if (!fd) {
            printf("Errore nell' apertura dell'immagine PGM\n");
            exit(0);
        }
        char id[3];
        fscanf(fd, "%2s", id);
        if (id[0] != 'P' || id[1] != '5') {
            printf("Formato PGM non valido (solo P5 binario si accetta)\n");
            fclose(fd);
            exit(1);
        }
        saltaPGMcommenti(fd);
        cima = pop(cima);
        el* img = calloc(1, sizeof(el));
        cima = push(cima, img);
        int lunghezza;
        int altezza;
        fscanf(fd, "%d %d", &lunghezza, &altezza);
        img->altezza = altezza;
        img->lunghezza = lunghezza;
        img->tipo = numerico;
        float* vettore =malloc(sizeof(float) * (img->altezza * img->lunghezza));
        img->v = vettore;
        img->sudisco = NULL;
        img->path = NULL;
        saltaPGMcommenti(fd);
        int maxval;
        fscanf(fd, "%d",&maxval);
        /*
        if (maxval != 1 || maxval < 255) {
            printf("File PGM potenzialmente corrotto (errore)\n");
            printf("%d\n", maxval);
            exit(1);
        } */
        fgetc(fd); //di solito dietro in MAXVAL c'e` un carattere da scartare
        unsigned char *pixelchar = (unsigned char*)malloc(lunghezza * altezza);
        if (pixelchar == NULL) {
            printf("Errore durante l'allocazione della memoria per l'immagine PGM\n");
            exit(1);
        }
        if (fread(pixelchar, 1, altezza*lunghezza, fd) !=  altezza*lunghezza) { //leggo l'intero file, e detetto se la lettura va storta
            printf("Errore lettura file PGM\n");
            exit(1);
        }
        fclose(fd);
        #pragma omp parallel for simd
        for(int i = 0; i < lunghezza * altezza; i++) {
            vettore[i] = (float)pixelchar[i] /255.0f; //salvati come floats 0 <= valore <= 1
        }
        free(pixelchar);
        img->refcount = 1;
    } else {
        printf("Errore avvenuto durante la lettura del file PGM\n");
        exit(1);
    }

    return cima;
}

elem* scriviPGM(elem* cima) { //scrivo il file PGM
if (checkstack(cima, 2) == 0) {
    FILE* fd = fopen(cima->ogg->path, "wb");
    if (!fd) {
        printf("Errore apertura file\n");
        exit(1);
    }
    elem* sottostante = cima->quello_sotto;
    int altezza = sottostante->ogg->altezza;
    int lunghezza = sottostante->ogg->lunghezza;
    size_t tot = altezza * lunghezza;
    fprintf(fd, "P5\n%d %d\n255\n", lunghezza, altezza);
    unsigned char* pixelchars = (unsigned char*) malloc(tot);
    if (pixelchars == NULL) {
        printf("Errore durante l'allocazione della memoria per l'immagine PGM\n");
        exit(1);
    }
    #pragma omp parallel for
    for (size_t i = 0; i < tot; i++) {
        float valore = sottostante->ogg->v[i];
        if (valore < 0.0f) valore = 0.0f;
        if (valore > 1.0f) valore = 1.0f;
        pixelchars[i] = (unsigned char)(valore * 255.0f); //trasformazione da valori tra 0 e 1 -> 0 <= valore <= 255
    }
    free(sottostante->ogg->v);
    sottostante->ogg->v = NULL;
    sottostante->ogg->sudisco = NULL;
    cima->ogg->sudisco = NULL;
    fwrite(pixelchars, 1, tot, fd);
    free(pixelchars);
    free(cima->ogg->path);
    cima->ogg->path = NULL;
    fclose(fd);
    //printf("Arrivato al primo pop\n"); //debug
    fflush(stdout); //debug
    cima = pop(cima);
    //printf("Arrivato al secondo pop\n"); //debug
    fflush(stdout); //debug
    cima = pop(cima);
} else {
    printf("Errore avvenuto durante la scrittura del file PGM\n");
    exit(1);
}
return cima;
}