#ifndef COUP_H
#define COUP_H

#include "type.h"
#include "boolean.h"

unsigned long long int rand64();
Boolean lireLigneFichier(FILE * file, char string[], int size);
void strupr( char *s , int len );
Boolean inputDisponible();

#endif

