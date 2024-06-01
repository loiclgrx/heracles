#ifndef PIECE_H
#define PIECE_H

#include "couleur.h"

/* Attention si modification de la valeur associe à une piece il faut egalement modifier les FLAG dans coup.h */
#define PASDEPIECE 0
#define PION 1
#define CAVALIER 2
#define FOU 3
#define TOUR 4
#define DAME 5
#define ROI 6


typedef struct {
  unsigned char nom;
  Couleur couleur;
} Piece;

		/************/
		/* GETEUR  */
		/************/		

#define piece_getNom( p ) 	(p.nom)
#define piece_getCouleur( p ) (p.couleur)

		/************/
		/* SETEUR  */
		/************/

#define piece_setNom( p , nomP ) 		p.nom=nomP
#define piece_setCouleur( p , couleurP ) 	p.couleur=couleurP

#endif /* PIECE_H */
