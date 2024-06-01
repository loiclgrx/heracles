#ifndef HISTORYTABLE_H
#define HISTORYTABLE_H

#include "coup.h"
#include "plateau.h"

#define TAILLE_HISTOTABLE	4096  /* 12 bits 6 pour la case de depart et 6 pour la case dest. */

typedef long HistoryTable[2][TAILLE_HISTOTABLE];

#define historytable_note( h , c , p , prof ) h[plateau_getCoteCourant(p)][coup_getCaseOriDest(c) ]+=\
	       									(prof*prof)

#define historytable_reset( h )   for( i=0 ; i<2 ; i++ ) for( k=0; k<4096; k++) {h[i][k] = 0; }		

#define historytable_get( h , p , c )  h[ plateau_getCoteCourant(pl) ][ coup_getCaseOriDest(c) ]

#endif

