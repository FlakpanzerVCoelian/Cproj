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
            ogg->path = NULL;
            ogg->sudisco = NULL;

        } else if (buffer[0] == '"') { //trovato un filepath
            printf("Trovato un filepath\n");
            el* ogg = malloc(sizeof(el));
            cima = push(cima, ogg);
            ogg->tipo = filepath;
            size_t lungh = strlen(buffer);
            char* path = malloc(sizeof(char) * (lungh - 1)); //strlen ritorna fino a prima del null terminator, percio mi serve uno in piu e due in meno per le apici
            if (lungh > 2 && buffer[0] == '"' && buffer [lungh-1] == '"') {
                strncpy(path, buffer + 1, lungh - 2);
                path[lungh-2] = '\0'; //aggiunta del null terminator
            } 
            else {
                printf("Filepath letto invalido (errore)\n");
                exit(1);
            }
            ogg->path = path;
            ogg->v = NULL;
            ogg->sudisco = NULL;
        } else { //sara` un operando
            cima = determina_operando(buffer, cima);
        }
    }
   
    
    fclose(fd);
    printf("Stato finale dello stack:\n");
    stampaSTACK(cima);
    return 0;
}

