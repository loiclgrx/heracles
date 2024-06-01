#include <stdio.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include "hasheval.h"
#include "boolean.h"
#include "math.h"
#include "trace.h"
#include "bitboard.h"


		/* Table de hachage pour stocker le score de positions de pions  */
HashEval hashPion[ 2 ];		
HashEval hashPionMixte;			// Structure noir et blanc ( pour l'evaluation des pions passees )



Boolean hasheval_effacer( HashEval *he ) {
	int i;
	
	for(i=0 ; i< hasheval_getTaille( he ); i++ ) {
	  hasheval_setScore( he , i , 0 );
	  hasheval_setCle( he , i , 0 );
	}
	
	return VRAI;
}

/*-------------------------------------------------------------------------------------*/

void hasheval_free( HashEval *he ) {
  free( he->ph );
}

/*-------------------------------------------------------------------------------------*/

		/* Return Vrai si pas de problème lors de la création */

Boolean hasheval_creer( HashEval *he , int tailleKB ) {
	int nbPos;
	int nbBits;
	int p=1,i;

	
	nbPos = (tailleKB*1024)/sizeof(PositionHashEval);
	nbBits = (int) (floor ( log(nbPos)/log(2)) );
	
		/* Arrondir nbPos a la puissance de 2 la plus proche */
	hasheval_setTaille( he , pow( 2 , nbBits ) );
	
	hasheval_setMaskIndex( he , 0 );
	
	for( i=0; i <nbBits ; i++ ) {
		he->maskIndex |= p;
		p <<= 1;
	}
	
	//printBitBoardLigne( he->maskIndex );

	he->ph = ( PositionHashEval * ) malloc( he->taille * sizeof( PositionHashEval) );
  
	if ( he->ph ) {
		hash_effacer();
		return VRAI;
	}
	else
		return FAUX;
}

/*-------------------------------------------------------------------------------------*/

	/* Return l'index de la position dans la table */

int hasheval_put( HashEval *he , int64 cle , int score ) {
  int32 index;

	
  index = cle & hasheval_getMaskIndex( he );

  hasheval_setCle( he , index , cle );
  hasheval_setScore( he ,index , score );

  return index;
	
}

/*-------------------------------------------------------------------------------------*/

		/* Return l'index de la position dans la table 
		   Return -1 si pas dans la table de hachage */

void hasheval_get( HashEval *he , int64 cle , PositionHashEval **ph ) {
	int32 index;


	index = cle & hasheval_getMaskIndex( he );

		/* Quelque chose mais ce n'est pas la même position */

	if ( hasheval_getCle( he , index )   != cle ) {
	  *ph = NULL;
	  return;
	}

		/* La position est dans la table de hachage */

	*ph = (he->ph)+index;
}


