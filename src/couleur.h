#ifndef COULEUR_H
#define COULEUR_H

#define NOIR     0
#define BLANC  1

typedef unsigned char Couleur;

#define couleur_getCouleur( c  ) 	c
#define couleur_setCouleur( c , co )  	c=co
#define couleur_oppose(c)		(1-c)

#endif /* COULEUR_H */
