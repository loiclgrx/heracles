#include <stdio.h>
#include "multipv.h"
#include "listecoup.h"
#include "constante.h"

MultiPv mpv;

/* Allocation memoire des PV */
void multipv_alloc( MultiPv *mpv ) {
  int i;

  for ( i=0 ; i < MAX_COUPS ; i++ )
	  (mpv->multiPv)[i] = ( Pv * ) malloc( sizeof(Pv) );
}	

/*-----------------------------------------------------------------------------------*/

/* creer autant de PV qu'il y a de coup dans lc */ 
void multipv_init( MultiPv *mpv , ListeCoup *lc ) {
	int i;

	multipv_setNbPv( mpv , listecoup_getNbCoup( lc ) );

	for ( i=0 ; i<listecoup_getNbCoup( lc) ; i++ ) {
		multipv_setScore( mpv , i , i );
		multipv_setPosition( mpv , i , 0 );
		multipv_setProfondeur( mpv , i , 0 );
		listecoup_videListeCoup( pv_getListeCoup( multipv_getPv( mpv , i ) ) );
		listecoup_addCoup2( pv_getListeCoup( multipv_getPv(mpv,i) ) ,
			            listecoup_getCoup( lc , i ) );
	}

}	
/*----------------------------------------------------------------------------------------------*/
/* Trie suivant la prondeur puis le score.
   La recherche la plus profonde avec le meilleur score en premier */
int comparePV( const void *pv1 , const void *pv2 ) {

		/* Même profondeur */
	if ( pv_getProfondeur( *((Pv **)pv1) ) == pv_getProfondeur( *((Pv **)pv2) ) ) {
			/* et score identique */
	   if ( pv_getScore( *((Pv **)pv1) ) == pv_getScore( *((Pv **)pv2) ) ) {
		 return ( pv_getNbNoeud( *((Pv **)pv2) ) - pv_getNbNoeud( *((Pv **)pv1) ) );
		  // return 0;
	   }
		   /* score different */
	   else {
	      return ( pv_getScore( *((Pv **)pv2) ) - pv_getScore( *((Pv **)pv1) ) );
	   }
	}

  return ( pv_getProfondeur( *((Pv **)pv2) ) - pv_getProfondeur( *((Pv **)pv1) ) );
}

/*----------------------------------------------------------------------------------------------*/

void multipv_trie( MultiPv *mpv ) {
  qsort( mpv->multiPv , multipv_getNbPV( mpv ) , sizeof( Pv* ) , comparePV );
}

/*----------------------------------------------------------------------------------------------*/
void multipvPrint( MultiPv *mpv ) {
  int i;
  char lCoup[1000];

  for( i = 0 ; i < multipv_getNbPV(mpv) ; i++ ) {
    printListeCoup( pv_getListeCoup( multipv_getPv( mpv , i) ) , lCoup );
    printf("cp %d depth %d node %ld : %s\n" ,   multipv_getScore( mpv , i ) , 
			  		        multipv_getProfondeur(mpv,i)  , 
		          			multipv_getNbNoeud(mpv , i ) , lCoup );
  }
}
