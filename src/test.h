#ifndef TEST_H
#define TEST_H

#include "plateau.h"
#include "couleur.h"
#include "coup.h"
#include "timer.h"
#include "boolean.h"

extern long nbCapture;

long perft( Plateau *p, short profondeur );
long perftDivise( Plateau *p, short profondeur );
long perftHash( Plateau *p, short profondeur );
void testGenerateurCoup( Plateau * );
void testAlphaBeta( Plateau * , GestionTemps * );
void compteNbNoeud( Plateau * , GestionTemps * );
void lanceTest( Plateau *p,GestionTemps* , char * , Boolean , void (*f)( Plateau * ,GestionTemps*)  );

void perf( Plateau *p );

#endif /* TEST_H */
