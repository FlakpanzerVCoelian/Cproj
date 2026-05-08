#ifndef _SUPPORTO_H
#define _SUPPORTO_H

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "strutture.h"

el* inizializza_oggetto(int altezza, int lunghezza, float* vett, char* path, int tipo, uint8_t *sudisco);
void saltaPGMcommenti(FILE *fp);
int numSIoNO(char *x);
int leggitensore(FILE* fd, el* ogg);

#endif