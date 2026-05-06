#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "stack.h"
#include "strutture.h"
#include "supporto.h"
#include "checks.h"
#include "operandi.h"
#include "scrivibin.h"


#define MAXDIM 2
#define buflen 1000 //100 mila byte limite


int main(int argc, char * argv[]){

    elem* cima = NULL; 
    //int altezzapila = 0;
    if (argc != 2) {
        printf("Filepath assente (errore)");
        return -1;
    }
    FILE *fd = fopen(argv[1], "r");
    if (fd == NULL) {
        printf("Errore nell'apertura del file\n");
        return -1;
    }
    char* buffer = malloc(sizeof(char) * buflen);

    while (fscanf(fd, "%255s", buffer) == 1) { //note to self: fsacnf legge una parola alla volta (primo spazio)
        //printf("Letto: %s in main()\n", buffer);  //DEBUG
        if (strcmp(buffer, "[") == 0) { //trovato un tensore, lo leggo
            el* ogg = malloc(sizeof(el));
            cima = push(cima, ogg);
            ogg->refcount = 1;
            ogg->altezza = 1; //giusto per inizializzare
            ogg->lunghezza = 1;
            ogg->tipo = leggitensore(fd, ogg);
            


        } else if (buffer[0] == '"') { //trovato un filepath 
            //da finire 
        } else { //sara` un operando o un \ (che ignoro)
            cima = determina_operando(buffer, cima);
        }
    }
   
    
    fclose(fd);
    printf("Stato finale dello stack:\n");
    stampaSTACK(cima);
    return 0;
}

