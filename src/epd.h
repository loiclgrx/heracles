#ifndef EPD_H
#define EPD_H

#include <stdio.h>
#include "piece.h"
#include "couleur.h"
#include "case.h"
#include "boolean.h"
#include "coup.h"

typedef struct {
	Piece echiquier[8][8];
	Couleur cote;				/* Indique si c'est aux Blancs ou aux Noirs de jouer */
	Case ep;					/* indique l'eventuelle case en passant */
	Boolean petitRoque[2];		/* indique si le petit roque est possible */
	Boolean grandRoque[2]; 	 	/* indique si le grand roque est possible */	
	Coup	meilleurCoup;		/* meilleur coup pour la position */
} Epd;


#define epd_getPiece( e , c , l )	e.echiquier[ c ][ l ]
#define epd_getCoteCourant( e )		e.cote
#define epd_getEnPassant( e )		e.ep
#define epd_isPetitRoque( e , c )	e.petitRoque[ c ]
#define epd_isGrandRoque( e , c )	e.grandRoque[ c ]
#define epd_getMeilleurCoup( e )	e.meilleurCoup

/* initialise un EPD à partir d'une chaine de caractères contenu l'EPD */

extern void initEpd( Epd *, char * );
extern FILE*	    loadEpd( char* fichier );
extern int			nextEpd( FILE *hdl , Epd *e );
extern int			unloadEpd( FILE *) ;

/* Rempalce les espaces d'une chaine par \0
 * Retourne le nombre de mots presents dans la chaine
 * TODO : Deplacer cette fonction dans util.h */

int espaceToFinChaine( char * s );

#endif /* EPD_H */
