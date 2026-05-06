#ifndef _OPERANDI_H
#define _OPERANDI_H

#include "strutture.h"

float andlogico (float a, float b);
float orlogico (float a, float b);
float maggiore(float a, float b);
float uguaglianza(float a, float b);
float minore(float a, float b);
float somma(float a, float b);
float sottrazione(float a, float b);
float moltiplicazione(float a, float b);
elem* riempi(elem* cima);
elem* shape(elem* cima);
elem* ravel (elem* cima);
elem* MINMAX (elem* cima, int opFLAG);
elem* OP_QUESTIONMARK (elem* cima);
elem* NOTlogico (elem* cima);
elem* sommaB(elem* cima);
elem* opvett(elem* cima, int FLAGop);
elem* stampa(elem * xx);
elem* determina_operando (char * x, elem *cima) ;

#endif
