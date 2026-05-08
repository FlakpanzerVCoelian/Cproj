#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#include "strutture.h"
#include "stack.h"
#include "checks.h"
#include "scrivibin.h"
#include "supporto.h"


#define buflen 1000


enum operandi {
    SOMMA = 3,
    SOTTRAZIONE,
    MOLTIPLICAZIONE,
    MINORE,
    MAGGIORE,
    UGUAGLIANZA,
    AND_LOGICO,
    OR_LOGICO,
    NOT,
    DOLLARO, //SELEZIONA TRA A E B IN BASE AL VETTORE DI "BOOLEANI"
    AT,
    DOT,
    CONV,
    RAVEL,
    FLAT, //simbolo UNDERSCORE
    SIZE,
    RAND,
    NEGZERO,
    MINOP,
    MAXOP,
    SOMMAB,
    FILL,
    PRINT,
    DUP,
    SWAP,
    OVER,
    DROP,
    PGMLEGGI, //30
    PGMSCRIVI,
    LOAD,
    STORE   //33
};
//                                0    1    2    3    4    5    6    7    8    9    10   11  12    13   14  15   16   17    18   19  20    21   22   23   24   25   26  27   28   29   30   31   32   33
const char* simboliOPERANDI[] = {"/", "/", "/", "+", "-", "*", "<", ">", "=", "&", "|", "!", "$", "@", ".", "c", "r", "_", "#", "?", "R", "m", "M", "S", "f", "p", "d", "s", "o", "D", "(", ")", "{", "}"};
//                               X     X    X    X    X    X    X    X    X    X    X    X    X    X    X    X    X    X    X    X     X    X    X   X    X    X     X   X    X    X    X    X    X    X
//tutti operandi risolti

float andlogico (float a, float b) {
    return (a == 1.0f && b == 1.0f) ? 1.0f : 0.0f;
}
float orlogico (float a, float b) {
    return (a == 1.0f || b == 1.0f) ? 1.0f : 0.0f;
}

float maggiore(float a, float b) {
    return a > b ? 1.0f : 0.0f;
}
float uguaglianza(float a, float b) {
    return a == b ? 1.0f : 0.0f;
}
float minore(float a, float b) {
    return a < b ? 1.0f : 0.0f;
}


float somma(float a, float b) {
    return a + b;
}

float sottrazione(float a, float b) {
    return a - b;
}

float moltiplicazione(float a, float b) {
    return a * b;
}

float mini(float a, float b) {
    return a < b ? a : b;
}

float maxi(float a, float b) {
    return a > b ? a : b;
}

float dotproduct(float* v1, float*v2, int len) {
    float totale = 0;
    #pragma omp parallel for simd reduction(+:totale)
    for (int i = 0; i < len; i++) {
        totale = v1[i] + v2[i];
    }
    return totale;
}


elem* CONVOLUZIONE(elem* cima) {
    if (checkstack(cima, 2) == 0) {
        elem* kernel = cima;
        elem* matrice = cima->quello_sotto;
        int len = matrice->ogg->lunghezza;
        int alt = matrice->ogg->altezza;
        int lenk = kernel->ogg->lunghezza;
        int altk = kernel->ogg->altezza;
        int ancoraVERT = altk / 2;
        int ancoraOR = lenk / 2;
        float *vettore = malloc(sizeof(float) * (len * alt));
        if (vettore == NULL) exit(1);
        #pragma omp parallel for
        for(int i = 0; i < len * alt; i++) {
            int i_riga = i / len; // dove sono nel ciclo for esterno in riferimento all' immagine
            int i_colonna = i % len;
            float somma = 0.0f;
            for (int j = 0; j < lenk * altk; j++) {
                int n_riga = j / lenk; //dove sono nel ciclo for interno in riferimanto all'immagine
                int n_colollna = j % lenk;
                int target_riga= i_riga + (n_riga - ancoraVERT);
                int target_colonna = i_colonna + (n_colollna - ancoraOR);
                if (target_riga >= 0 && target_riga < alt && 
                    target_colonna >= 0 && target_colonna < len) {
                    int target_1d = (target_riga * len) + target_colonna;
                    somma += (matrice->ogg->v[target_1d] * kernel->ogg->v[j]);
                }
            }
            vettore[i] = somma;
        }
        int tipoo = matrice->ogg->tipo;
        cima = pop(cima); 
        cima = pop(cima); 
        el* risultato = inizializza_oggetto(alt, len, vettore, NULL, tipoo, NULL);
        return push(cima, risultato);
    }
    printf("Errore: Impossibile eseguire convoluzione\n");
    exit(1);
}


elem* exproct(elem* cima) { //operazione @ colonna 1A per Riga 1B, ecc
    if (checkstack(cima, 2) == 0 && checktipo(cima, 2, numerico, 0) == 0) {
        elem* a = cima;
        elem* b = cima->quello_sotto;
        if (a->ogg->lunghezza == b->ogg->altezza) { //dimensioni compatibili
            int len = b->ogg->lunghezza;
            int alt = a->ogg->altezza;
            int shareddim = a->ogg->lunghezza;
            float* vettore = malloc(sizeof(float) * (len * alt));
            int tipoo;
            if (a->ogg->tipo == floatbinario && b->ogg->tipo == floatbinario) tipoo = floatbinario;
            else tipoo = numerico;
            el *ogge = inizializza_oggetto(alt, len, vettore, NULL, tipoo, NULL);
            #pragma omp parallel for collapse(2)
            for (int i = 0; i < alt; i++) {
                for (int j = 0; j < len; j++) {
                    float dotprod = 0.0f;
                    for (int k = 0; k < shareddim; k++) {
                        dotprod = dotprod + (a->ogg->v[(i*shareddim) + k] * b->ogg->v[(k*len) + j]);
                    }
                    ogge->v[(i * len) + j] = dotprod;
                }
            }
            cima = pop(cima);
            cima = pop(cima);
            cima = push(cima, ogge);
        } else {
            printf("Dimensioni incompatifili per il prodotto matriciale (errore)");
            exit(1);
        }
    } else {
        printf("Errore durante il prodotto matriciale");
        exit(1);
    }
    return cima;
}



elem* dotprod(elem* cima) { // dot product
    if (checkstack(cima, 2) == 0 && checktipo(cima, 2, numerico, 0) && checklen(cima, 2) == 0 && cima->ogg->altezza == 1) {
        int len = cima->ogg->lunghezza;
        float* num = malloc(sizeof(float));
        elem* a = cima;
        elem* b = cima->quello_sotto;
        int tipoo;
        if (a->ogg->tipo == floatbinario && b->ogg->tipo == floatbinario) tipoo = floatbinario;
        else tipoo = numerico;
        num[0] = dotproduct(a->ogg->v, b->ogg->v, len);
        cima = pop(cima);
        cima = pop(cima);
        el* ogge = inizializza_oggetto(1, 1, num, NULL, tipoo, NULL);
        cima = push(cima, ogge);
    } else {
        printf("Errore durante l'operazione .\n");
        exit(1);
    }
    return cima;
}



elem* riempi(elem* cima) { //fa l'operazione FILL
    if (checkstack(cima, 2) == 0) {
        elem* sizevett = cima->quello_sotto;
        if (sizevett->ogg->lunghezza == 1) { //trasformo l'array size in un formato piu` decente
            float* vett = malloc(sizeof(float) * 2);
            vett[1] = sizevett->ogg->v[0]; //lunghezza
            vett[0] = 1; //altezza
            free(sizevett->ogg->v);
            sizevett->ogg->v = vett;
            sizevett->ogg->lunghezza = 2;
        }
        int len = (int)sizevett->ogg->v[1];
        int alt = (int)sizevett->ogg->v[0];
        int size = len*alt;
        int sizeb = cima->ogg->altezza * cima->ogg->lunghezza;
        //printf("Da riempire %d elementi con %d elementi\n", size, sizeb); //debug
        //fflush(stdout); //debug
        float* vettore = malloc(sizeof(float) * size);
        if (vettore == NULL) {
            printf("Errore durante l'allocazione per la memoria del vettore\n");
            exit(1);
        }
        #pragma omp parallel for simd //va bene perche non scrivo nella stessa posizione
        for (int i = 0; i < size; i++) {
            //printf("i: %d, posvettore %d\n", i, i%sizeb);
            //fflush(stdout);
            vettore[i] = cima->ogg->v[i%sizeb]; //idea
        }
        //printf("uscito dal ciclo %d", cima->ogg->tipo); fflush(stdout);
        int tipo = cima->ogg->tipo;
        cima = pop(cima);
        //printf("uscito dal ciclo"); fflush(stdout);
        cima = pop(cima);
        //printf("uscito dal ciclo"); fflush(stdout);
        el* nuovogg = inizializza_oggetto(alt, len, vettore, NULL, tipo, NULL);
        cima = push(cima, nuovogg);
    } else {
        printf("Errore avvenuto durante fill\n");
        exit(1);
    }
    return cima;
}

elem* shape(elem* cima) { //esegue l'operazione # (shape)
    el* vecchio = cima->ogg;
    el* nuovo = malloc(sizeof(el));
    nuovo->tipo = numerico;
    nuovo->altezza = 1; 
    nuovo->refcount = 1;
    nuovo->path = NULL;
    nuovo->sudisco = NULL;
    if (vecchio->altezza == 1) {
        nuovo->lunghezza = 1; 
        nuovo->v = (float*) malloc(1 * sizeof(float));
        nuovo->v[0] = (float)vecchio->lunghezza; 
    } else {

        nuovo->lunghezza = 2;
        nuovo->v = (float*) malloc(2 * sizeof(float));
        nuovo->v[0] = (float)vecchio->altezza;
        nuovo->v[1] = (float)vecchio->lunghezza;
    }
    cima = pop(cima);
    return push(cima, nuovo);
}


elem* ravel (elem* cima) { //esegue RESHAPE
    if (checkstack(cima, 2) == 0 && checktipo(cima, 2, numerico, 0) == 0 ) {
        if (cima->ogg->lunghezza == 1) { //trasformo l'array size in un formato piu` decente
            float* vett = malloc(sizeof(float) * 2);
            vett[1] = cima->ogg->v[0]; //lunghezza
            vett[0] = 1; //altezza
            free(cima->ogg->v);
            cima->ogg->v = vett;
            cima->ogg->lunghezza = 2;
        }
        int len = (int)cima->ogg->v[0];
        int altezza = (int)cima->ogg->v[1];
        cima = pop(cima);
            if (len * altezza == cima->ogg->lunghezza * cima->ogg->altezza) {
                cima->ogg->altezza = altezza;
                cima->ogg->lunghezza = len;
            } else {
                printf("Errore, impossibile eseguire reshape sulla matrice (dimensioni incompatibili)\n");
                exit(1);
            }
            return cima;
        }
        else {
            printf("Errore avvenuto durangte reshape\n");
            exit(1);
        }
}


elem* MINMAX (elem* cima, int opFLAG) {
    if (checkstack(cima, 2) == 0 && checklen(cima, 2) == 0 && checktipo(cima, 2, numerico, 0) == 0) {
        elem* a = cima;
        elem* b = cima->quello_sotto;
        int len = a->ogg->lunghezza;
        int altezza = a->ogg->altezza;
        float* risultato = malloc(sizeof(float) * (len * altezza));
        #pragma omp parallel for simd //uso parallel for simd per paralelizzare
        for (int i = 0; i < len * altezza; i++) {
            risultato[i] = (opFLAG == MINOP) ? mini(a->ogg->v[i], b->ogg->v[i]) : maxi(a->ogg->v[i], b->ogg->v[i]);
        }
        cima = pop(cima);
        cima = pop(cima);
        el* ogg = inizializza_oggetto(altezza, len, risultato, NULL, numerico, NULL);
        cima = push(cima, ogg);
    }
    else {
        printf("Errore avventuto nell'esecuzione dell'operazione %s\n", simboliOPERANDI[opFLAG]);
        exit(1);
    }
    return cima;
}

elem* OP_QUESTIONMARK (elem* cima) { //non un riferimento all' "operazione" questionmark
    srand(time(NULL)); //genera numeri a caso in base alla specifica del vettore
    if (cima->ogg->lunghezza == 1) { //trasformo l'array size in un formato piu` decente
        float* vett = malloc(sizeof(float) * 2);
        vett[1] = cima->ogg->v[0]; //lunghezza
        vett[0] = 1; //altezza
        free(cima->ogg->v);
        cima->ogg->v = vett;
        cima->ogg->lunghezza = 2;
    }
    if (checkstack(cima, 1) == 0 && checktipo(cima, 1, numerico, 0) == 0 && cima->ogg->lunghezza == 2 && cima->ogg->altezza == 1) {
        float* vettore = malloc(sizeof(float) * cima->ogg->v[0] * cima->ogg->v[1]);
        cima->ogg->altezza = (int)cima->ogg->v[0];
        cima->ogg->lunghezza = (int)cima->ogg->v[1];
        for (int i = 0; i < cima->ogg->altezza * cima->ogg->lunghezza; i++) {
            vettore[i] = (float)rand() / (float)RAND_MAX;
        }
        el* ogge = inizializza_oggetto((int)cima->ogg->v[0], (int)cima->ogg->v[1], vettore, NULL, numerico, NULL);
        cima = pop(cima);
        cima = push(cima, ogge);
    } else {
        printf("Elemento di formato non valido per l'OPERAZIONE QUESTIONMARK\n");
        exit(1);
    }
    return cima;
}


elem * dollaro(elem* cima) { //esegue l'operazione di dollaro
    if (checkstack(cima, 3) == 0 && checklen(cima, 3) == 0 && checktipo(cima, 1, floatbinario, 1) == 1){
        float* vettore = malloc(sizeof(float) * cima->ogg->altezza * cima->ogg->lunghezza);
        //elem* m = cima->quello_sotto; //array di "bools"
        elem *a = cima->quello_sotto;
        elem *b = a->quello_sotto;
        int tipoo;
        if (a->ogg->tipo == floatbinario && b->ogg->tipo == floatbinario) tipoo = floatbinario;
        else tipoo = numerico;
        int len = cima->ogg->lunghezza;
        int altezza = cima->ogg->altezza;
        for (int i = 0; i < len * altezza; i++) { //valore 1 > da A, 0 > da B
            vettore[i] = (cima->ogg->v[i] == 1) ? a->ogg->v[i] : b->ogg->v[i];
        }
        cima = pop(cima);
        cima = pop(cima);
        cima = pop(cima);
        el* ogg = inizializza_oggetto(altezza, len, vettore, NULL, tipoo, NULL);
        cima = push(cima, ogg);
    } else {
        printf("Errore avventuto nell'esecuzione dell'operazione\n");
        exit(1);
    }
    return cima;
}


elem* NOTlogico (elem* cima) {
    if (checkstack(cima, 1) == 0 && checktipo(cima, 1, floatbinario, 1) == 0) {
        float* vett = malloc(sizeof(float) * cima->ogg->lunghezza * cima->ogg->altezza);
        int altezza = cima->ogg->altezza;
        int len = cima->ogg->lunghezza;
        #pragma omp parallel for simd
        for (int i = 0; i < cima->ogg->lunghezza * cima->ogg->altezza; i++) {
            vett[i] = (cima->ogg->v[i] == 1.0f ? -1.0f : 0.0f) + 1; //se [i] == 1, allora lo metto a -1 e gli sommo 1 e diventa 0
                                                                       //se [i] == 0, allora lo metto a 0, gli sommo 1 e diventa 1
        }
        cima = pop(cima);
        el* ogge = inizializza_oggetto(altezza, len, vett, NULL, floatbinario, NULL);
        cima = push(cima, ogge);
    }
    return cima;
}

elem* sommaB(elem* cima) {
    float sommab = 0.0f;
    int len = cima->ogg->lunghezza;
    int altezza = cima->ogg->altezza;

    // parallel for simb reduction per fare la somma  
    #pragma omp parallel for simd reduction(+:sommab)
    for (int i = 0; i < len * altezza; i++) {
        sommab += cima->ogg->v[i];
    }
    free(cima->ogg->v);
    cima = pop(cima);  //piuttosto poppo e poi riaggungo per evitare eventuali conflitti con dup
    float* vettorino = malloc(sizeof(float));
    vettorino[0] = sommab;
    el* nuovogg = inizializza_oggetto(1, 1, vettorino, NULL, numerico, NULL);
    cima = push(cima, nuovogg);
    return cima;
}


float (*FMAT1D[])(float, float) = {somma, sottrazione, moltiplicazione, maggiore, minore, uguaglianza, andlogico, orlogico}; // +3 = 3; 8 elem



elem* opvett(elem* cima, int FLAGop) { //esegue le operazioni tra vettori 1o2D generali tra due operandi
    printf("Eseguo operazione tra vettori semplici\n");
    if (checkstack(cima, 2) == 0 && checklen(cima, 2) == 0 && ((((checktipo(cima, 2, numerico, 0) == 0) && FLAGop <= UGUAGLIANZA)) || ((checktipo(cima, 2, floatbinario, 1) == 0) && FLAGop > UGUAGLIANZA))) {
        elem* a = cima;
        elem* b = cima->quello_sotto;
        int len = a->ogg->lunghezza;
        int altezza = a->ogg->altezza;
        float* risultato = malloc(sizeof(float) * (len * altezza));
        #pragma omp parallel for simd //uso parallel for simd per paralelizzare
        for (int i = 0; i < len * altezza; i++) {
            risultato[i] = FMAT1D[FLAGop-3](a->ogg->v[i], b->ogg->v[i]);
            //printf("DEBUG: operazione %d tra %f e %f -> risultato=%f\n", FLAGop, a->ogg->v[i], b->ogg->v[i], risultato[i]); //DEBUG
        }
        cima = pop(cima);
        cima = pop(cima);
        int tipo;
        if (FLAGop > MOLTIPLICAZIONE) tipo = floatbinario;
        else tipo = numerico;
        el* ogg = inizializza_oggetto(altezza, len, risultato, NULL, tipo, NULL);
        cima = push(cima, ogg);
    }
    else {
        printf("Errore avventuto nell'esecuzione dell'operazione\n");
        exit(1);
    }
    return cima;

}


elem* stampa(elem * xx) { //operazione PRINT P
    stampa_corrente(xx);
    return pop(xx);
}

elem* determina_operando (char * x, elem *cima) { //determina l'operazione da fare
    int operando = 0;
    printf("Determino operando per simbolo: %s\n", x); //DEBUG
    if (strcmp(x, "\\|") == 0) operando = 10; //l' OR e` speciale...
    for (int i = 3; i <= 33; i++) {
        if (strcmp(x, simboliOPERANDI[i]) == 0) {
            operando = i;
            break;
        }
    }
    //printf("Operando determinato: %d\n", operando); //DEBUG
    if (operando == 0){
        printf("Simbolo (%s) sconosciuto (errore)", x);
        exit(0);
    } else if (strcmp(x, simboliOPERANDI[PRINT]) == 0) {
        return stampa(cima);
    } else if (operando >= SOMMA && operando <= OR_LOGICO) {
        return opvett(cima, operando);
    } if (operando == NOT) {
        return NOTlogico(cima);
    } else if (operando == SOMMAB) {
        return sommaB(cima);
    } else if (operando == RAVEL) {
        return ravel(cima);
    } else if (operando == FLAT && checklen(cima, 1) == 0) { //operazione banalissima e percio la metto nel corpo
        int len = cima->ogg->lunghezza * cima->ogg->altezza;
        cima->ogg->altezza = 1;
        cima->ogg->lunghezza = len;
    } else if (operando == SIZE && checkstack(cima, 1) == 0) { //operazione banalissima (non proprio) e percio la metto nel corpo
        return shape(cima);
    } else if (operando == RAND) {
        return OP_QUESTIONMARK(cima);
    } else if (operando == MINOP || operando == MAXOP) {
        return MINMAX(cima, operando);
    } else if (operando == DOLLARO) {
        return dollaro(cima);
    } else if (operando == DUP) {
        return dupNONUNISTD(cima);
    } else if (operando == OVER) {
        return over(cima);
    } else if (operando == SWAP) {
        return swap(cima);
    } else if (operando == NEGZERO && checkstack(cima, 1) == 0) { //operazione banalissima e percio la metto nel corpo
        int len = cima->ogg->altezza * cima->ogg->lunghezza;
        #pragma omp parallel for simd
        for (int i = 0; i < len; i++) {
            cima->ogg->v[i] = (cima->ogg->v[i] > 0) ? cima->ogg->v[i] : 0;
        }
    } else if (operando == FILL) {
        return riempi(cima);
    } else if (operando == DOT) {
        return dotprod(cima);
    } else if (operando == AT) {
        return exproct(cima);
    } else if (operando == CONV) {
        return CONVOLUZIONE(cima);
    } else if (operando == LOAD) {
        return(leggibin(cima));
    } else if  (operando == STORE){
        return(scrivibin(cima));
    } else if (operando == PGMLEGGI) {
        return(leggiPGM(cima));
    } else if (operando == PGMSCRIVI) {
        return (scriviPGM(cima));
    } else { //in questo branch non dovrebbe mai entrare
        printf("Operando non riconosciuto (errore)");
        exit(1);
    }
    return cima;
}