#ifndef _CHECKS_H
#define _CHECKS_H

#include "strutture.h"

int checkstack(elem* cima, int profondita);
int checklen (elem* cima, int profondita);
int checktipo(elem* cima, int profondita, int tipo, int strict);

#endif