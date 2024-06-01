#include <stdio.h>
#include <stddef.h>
#include "hash.h"
#include "boolean.h"
#include "math.h"
#include "trace.h"
#include "bitboard.h" // A SUPPRIMER CAR UTILISE POUR LE DEBUG


/* Table de hachage pour l'alphaBeta : Le malloc est réalisé dans la fonction hash_creer */
Hash hash;

/* Table de hachage pour l'alphaBeta : Le malloc est réalisé dans la fonction hash_creer */
HashRepetition hashRepet;

/*  Tableau pour tester la version Hash de la repetition de positions */
DebugRepetition debugRepet;

	/*-----------------------------------------------*/
	/* Definition de la hash table pour l'alphabeta	 */
	/*-----------------------------------------------*/

/* Return Vrai si pas de problème lors de l'effacement */
Boolean hash_effacer( ) {
	int i;
	
	for(i=0 ; i<hash.taille ; i++ ) {
	  hash_setProfondeur( i , -1 );
	  hash_setFlag( i , HASH_INDEFINI );
	}
	
 	hash_setTauxRemplissage( 0 );
	hash_setNbCollisions( 0 );
	hash_setNbRejet( 0 );

	return VRAI;
}

/*-------------------------------------------------------------------------------------*/
void hash_free() {
  free( hash.ph );
}
/*-------------------------------------------------------------------------------------*/

/* Return Vrai si pas de problème lors de la création */
Boolean hash_creer( int tailleMB ) {
	int nbPos;
	int nbBits;
	int p=1,i;

	
	nbPos = (tailleMB*1024*1024)/sizeof(PositionHash);
	nbBits = (int) (floor ( log(nbPos)/log(2)) );
	
		/* Arrondir nbPos a la puissance de 2 la plus proche */
	hash_setTaille( pow( 2 , nbBits ) );
	
	hash_setMaskIndex( 0 );
	
	for( i=0; i <nbBits ; i++ ) {
		hash.maskIndex |= p;
		p <<= 1;
	}
	
	hash.ph = ( PositionHash * ) malloc( hash.taille * sizeof( PositionHash) );
  
	/* TODO : la taille de la structure est plus important si plusieurs type <> 
			  Si uniquement des type char sizeof(struct) = n n=nb type char 
			  A etudier pour minimiser la taile de la structure */
  
	
	if ( hash.ph ) {
		hash_effacer();
		return VRAI;
	}
	else
		return FAUX;
}

/*-------------------------------------------------------------------------------------*/

/* Return l'index de la position dans la table 
   Return -1 si position non stockée dans la table
*/
int hash_put( int64 cle , Coup co , sint8 prof , int score , int flag ) {
	int32 index;
	
	index = cle & hash_getMaskIndex;
	//printf("PUT index %d\n",index);fflush(stdout);
	if (  prof > hash_getProfondeur( index )   ) {  /* On remplace par la nouvelle */

		if ( hash_getFlag( index ) == HASH_INDEFINI ) 
			hash_incrementeTauxRemplissage;
		else
		if ( hash_getCle( index )   != cle ) 
			  hash_incrementeNbCollisions;
		
	//	printf("Taux de remplissage %d\n",hash.tauxRemplissage);fflush(stdout);
		hash_setCle( index , cle );
		hash_setMeilleurCoup( index , co ); 
		hash_setProfondeur( index , prof );
		hash_setScore( index , score );
		hash_setFlag( index , flag );
		return index;
	}
	else {
		/* position rejetee car moins profonde que celle présente dans la table */
		if ( hash_getCle( index )   != cle ) 
			  hash_incrementeNbRejet;
	}
	
	return -1;
}

/*-------------------------------------------------------------------------------------*/

/* Return l'index de la position dans la table 
	Return -1 si pas dans la table de hachage
*/
int hash_get( int64 cle , int alpha , int beta , short profondeur ,PositionHash **ph ) {
	int32 index;
	
	index = cle & hash_getMaskIndex;

		/* Rien dans la Hash Table à cette position */
	if ( hash_getFlag( index ) == HASH_INDEFINI ) {
		*ph = NULL;
		return HASH_NOTHING;
	}

		/* Quelque chose mais ce n'est pas la même position */
	if ( hash_getCle( index )   != cle ) {
			*ph = NULL;
			return HASH_NOTHING;
	}

		/* La position est dans la table de hachage */

	*ph = hash.ph+index;

		/* La position provoque t'elle une coupure */

	 if ( positionHash_getProfondeur( (*ph) ) >= profondeur ) {

		 switch ( positionHash_getFlag( (*ph) ) ) {
			 case HASH_FEUILLE :
			 case HASH_EXACT   : return HASH_CUT;
					     break;
			 case HASH_ALPHA   : if ( positionHash_getScore( (*ph) ) <= alpha )
						   return HASH_CUT;
					     else
						   return HASH_ALPHA;
					     break;
			 case HASH_BETA    : if ( positionHash_getScore( (*ph) ) >= beta )
						  return HASH_CUT;
					     else
						  return HASH_BETA;
		 }
	 }

	 return HASH_SORT;
}

/*-------------------------------------------------------------------------------------*/

int hash_getHash( int64 cle , short profondeur ) {
	int32 index;
	
	index = cle & hash_getMaskIndex;

		/* Rien dans la Hash Table à cette position */
	if ( hash_getFlag( index ) == HASH_INDEFINI ) return 0;

		/* Quelque chose mais ce n'est pas la même position */
	if ( hash_getCle( index )   != cle ) return 0;

		/* La position est dans la table de hachage */

	 if ( positionHash_getProfondeur( (hash.ph+index) ) == profondeur ) 
		 return positionHash_getScore( (hash.ph+index) );
	 else
		 return 0;
}


/*-------------------------------------------------------------------------------------*/

	/*---------------------------------------------------------------*/
	/* Definition de la hash table pour la detection des repetitions */
	/*---------------------------------------------------------------*/

/* Return Vrai si pas de problème lors de l'effacement */
Boolean hashRepet_effacer( ) {
	int i;

	for( i=0 ;  i < TAILLE_HASH_REPET ; i++ ) {
		hashRepet.posRepet[i].cle = 0;
		hashRepet.posRepet[i].nb = 0;
	}

	hashRepet.maskRepet = MASK_INDEX_REPET;
	return VRAI;
}

/*-------------------------------------------------------------------------------------*/

/* Stocke la cle dans la hash table. Si elle existe deja incremente de 1 sa valeur nb.
 * retourne l'index de la cle dans la hash table */
int hashRepet_put( int64 cle ) {
	register int i,debut;

	i = (cle & hashRepet.maskRepet) << NB_POS_MAX_BIN;
	debut=i;

	/* recherche le premier emplacement libre ou la position si elle existe deja */
	while ( ( hashRepet.posRepet[i].nb != 0 ) && 
		    ( hashRepet.posRepet[i].cle != cle ) &&
		    ( i < debut+NB_POS_MAX ) ) 
		i++;
/*
	if ( (i-debut)>3 ) {
		fprintf( stderr , "### HASHREPET  %d collisions\n" , i-debut );
		fflush(stderr);
	}
	if ( i >=  debut+NB_POS_MAX  ) {
		fprintf( stderr , "############# + de 128 COLLISIONS !!! ##########\n");
		fflush(stderr);
		exit(1);
	}
*/	
	hashRepet.posRepet[i].cle = cle;
	hashRepet.posRepet[i].nb++;

	return i;
}
/*-------------------------------------------------------------------------------------*/

/* Decremente de 1 le nombre de fois que la position a ete rencontree */
extern void hashRepet_remove( int64 cle ) {
	register int i,debut;

	i = (cle & hashRepet.maskRepet) << NB_POS_MAX_BIN;
	debut=i;

	/* recherche l'emplacement de la cle */
	while ( ( hashRepet.posRepet[i].cle != cle ) && 
		  ( i < debut+NB_POS_MAX ) ) i++;

/*
	if ( i >=  debut+NB_POS_MAX  ) {
		fprintf( stderr , "############# Je ne retrouve pas la position dans HashRepet ##########\n");
		fflush(stderr);
		exit(1);
	} 
	else */
	 hashRepet.posRepet[i].nb--;
}

/*-------------------------------------------------------------------------------------*/

/* Return le nombre de fois que la cle a ete rencontre lors de la partie. */
short hashRepet_get( int64 cle ) {
	register int i,debut;

	i = (cle & hashRepet.maskRepet) << NB_POS_MAX_BIN;
    debut=i;

	/* recherche l'emplacement de la cle */
	while ( ( hashRepet.posRepet[i].nb != 0 ) && 
	        ( hashRepet.posRepet[i].cle != cle ) && 
			( i < debut+NB_POS_MAX ) ) i++;

/*
	if ( i >=  debut+NB_POS_MAX  )  {
		fprintf( stderr , "############# + de 128 COLLISIONS !!! ##########\n");
		fflush(stderr);
		return 0;
	}
	else */
	  return hashRepet.posRepet[i].nb;
}
/*-------------------------------------------------------------------------------------*/

extern void hashRepet_test( int profondeur ) {
	int i;

	for( i=0 ;  i < TAILLE_HASH_REPET ; i++ ) 
		if ( (hashRepet.posRepet[i].nb < 0) ||
		     (hashRepet.posRepet[i].nb > profondeur ) )
			printf("BUG cle %lld nb %d\n",hashRepet.posRepet[i].cle,hashRepet.posRepet[i].nb);

}

/*-------------------------------------------------------------------------------------*/
void debugRepet_clear( ) {
   register int i;

   for( i=0 ; i<500 ; i++ ) 
		debugRepet.posRepet[ i ].cle = 0;
   
   debugRepet.nbPos = 0;
}

/*-------------------------------------------------------------------------------------*/

void debugRepet_put( int64 cle ) {
	int i,j,nb;

	debugRepet.posRepet[ debugRepet.nbPos ].cle = cle;
	debugRepet.nbPos++;

	if ( debugRepet.nbPos > 499 ) {
                   fprintf( stderr, "************** + 500 POS *************\n") ;
		   fflush( stderr );
	}

	for ( i=0 ; i<debugRepet.nbPos ; i++ ) {
		nb =0;
		for( j=0 ; j<debugRepet.nbPos ; j++ ) {
			if ( debugRepet.posRepet[ j ].cle == debugRepet.posRepet[ i ].cle ) 
				 nb++;
		}

		if ( nb != hashRepet_get( debugRepet.posRepet[ i ].cle ) ) {
           fprintf( stderr, "************** ERREUR REPET *************\n") ;
		   fflush( stdout );
		}
	}
	
}

/*-------------------------------------------------------------------------------------*/

void debugRepet_remove( int64 cle ) {

	debugRepet.nbPos--;
	debugRepet.posRepet[ debugRepet.nbPos ].cle = 0;

}

/*-------------------------------------------------------------------------------------*/

Boolean debugRepet_get( int64 cle )  {
   register int i,j;
   int nb = 0;
   int nbHashRepet;

   for( i=0 ; i<debugRepet.nbPos ; i++ ) {
      if ( cle == debugRepet.posRepet[ i ].cle ) nb++;
   }


   if ( nb != hashRepet_get( cle ) ) {
	 printf("######## Nb repet %d <-> %d #######\n", nb , hashRepet_get( cle ) );
	 fflush( stdout);
    nbHashRepet = 0;

	for( j=0 ;  j < TAILLE_HASH_REPET ; j++ ) {
		if ( hashRepet.posRepet[ j ].nb != 0 ) nbHashRepet++;
	}

    printf("######## Nb PosHash %d    Nb Pos %d #######\n", nbHashRepet , debugRepet.nbPos );
	 fflush( stdout);

	 return FAUX;
   }
   
  return VRAI;

  
}
