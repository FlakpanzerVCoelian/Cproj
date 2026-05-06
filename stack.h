#ifndef _STACK_H
#define _STACK_H

#define MAXDIM 2

#include "strutture.h"



elem* push(elem* cima, el* oggetto);
elem* dupNONUNISTD(elem*cima);
elem* over(elem*cima);
elem* pop(elem* cima);
elem* swap(elem* cima);
void stampa_corrente(elem* corrente);
void stampaSTACK(elem* pavimento);

#endif