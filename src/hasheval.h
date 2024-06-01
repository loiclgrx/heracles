#ifndef HASHEVAL_H
#define HASHEVAL_H

#include "type.h"
#include "boolean.h"
#include "bitboard.h"


	/*---------------------------------------------------------------*/
	/* Definition de la hash table pour stocker la note de l'eval	 */
	/*---------------------------------------------------------------*/


		/* La hache table est un tableau de PositionHash */

typedef struct {
  int64 cle;		
  int	score; 
} PositionHashEval;


		/* Table de hachage pour stocker les positions rencontrees 
  		   lors de la recherche .Pour chaque position on stocke sa note */

typedef struct {
  PositionHashEval *ph;	    	/* Liste des positions */
  int	 	 taille;    		/* Taile de la hache table ( puissance de 2 ) */
  int64		 maskIndex; 		/* Mask qui permet de determiner l'index de la position
			       					 à partir de la cle */
} HashEval;



#define positionHashEval_getCle( ph ) 	( ph->cle )
#define positionHashEval_getScore( ph )	( ph->score )

#define hasheval_getCle( h , i ) 	( (h->ph)[i].cle )
#define hasheval_getScore( h , i ) 	( (h->ph)[i].score )
#define hasheval_getTaille( h )	 	  h->taille 
#define hasheval_getMaskIndex( h )	  h->maskIndex 
#define hasheval_setCle(  h ,i , c ) 	( (h->ph)[i].cle = c )
#define hasheval_setScore( h , i , s ) 	( (h->ph)[i].score = s )
#define hasheval_setMaskIndex( h , m ) 	( h->maskIndex = m )
#define hasheval_setTaille(  h , t ) 	( h->taille = t )



	/* Return Vrai si pas de problème lors de la création */
extern Boolean hasheval_creer( HashEval *he , int tailleKB );

	/* Desalloue la zone memoire de la table de hachage */
extern void hasheval_free( HashEval *he );

	/* Return Vrai si pas de problème lors de l'effacement */
extern Boolean hasheval_effacer( HashEval *he );

	/* Return l'index de la position dans la table */
extern int hasheval_put( HashEval *he , int64 cle , int score );

extern void hasheval_get( HashEval *he , int64 cle , PositionHashEval **ph );

	/*-----------------------------*/
	/* Declaration des hash tables */
	/*-----------------------------*/

extern HashEval hashPion[ 2 ];  // Une table pour chaque couleur
extern HashEval hashPionMixte;  // Une table pour les 2 camps 

#endif
