//#include coup.h>
#include <stdio.h>
#include "listecoup.h"

/* Trie les coups en fonction de :
 * 	- Si la position est dans la table de hachage
 * 	- Des variations principales
 * 	- Killer move
 * 	- .... 
 *
 * 	l'orde conseille est : ( source TalkChess.com )
 * 		- Null move
 * 		- HashTable
 * 		- Good capture ( SSE )
 * 		- Killer move
 * 		- History Table ?
 */
void listecoup_trie(  ListeCoup *lc , short aPartirDe , short nbTrie) {
	register int i,j;
	long bestScore1,bestScore2;
	short indiceBestScore1 , indiceBestScore2;
	Coup co;
	long note;
#if 0
	int p,q,r,d,n,p2,i;
	Boolean fin;
	CoupNote tmp;
	n = listecoup_getNbCoup( lc );
	p = 1;

	while (p<=n) p <<= 1;
	p2 = p >> 1;
	while (p>=2) {    	
    		p >>= 1;
	    	q = p2;
    		r = 0;
	    	d = p;
    		fin = FAUX;
	    	while (! fin) {
    			for (i=0;i<n-d;i++) {
    			if ((i & p) == r) {    			
		  	  if ( (lc->listeCoup)[i+d].note > (lc->listeCoup)[i].note ) {
					// Echange de i et i+d
			tmp = (lc->listeCoup)[i];
			(lc->listeCoup)[i] = (lc->listeCoup)[i+d];
			(lc->listeCoup)[i+d] = tmp;
    				}
    			}
    		}
    		if (q != p) {
    			d = q-p;
    			q = q >> 1;
    			r = p;
    			fin = FAUX;
    		} else fin=VRAI;
    	}
    }
#endif
#if 0
	if ( aPartirDe >= listecoup_getNbCoup(lc)-1 ) return;

	bestScore1 = bestScore2 = -1000000000;
	
	for( i=aPartirDe ; i<listecoup_getNbCoup( lc ) ; i++ ) {
		if ( listecoup_getNote( lc , i ) > bestScore2 )  {
			if ( listecoup_getNote( lc , i ) > bestScore1 ) {
				indiceBestScore2 = indiceBestScore1;
				bestScore2	 = bestScore1;
				indiceBestScore1 = i;
				bestScore1	 = listecoup_getNote( lc , i );
			}
			else {
				indiceBestScore2 = i;
				bestScore2	 = listecoup_getNote( lc , i );

			}
		}
	}

	co = listecoup_getCoup( lc , aPartirDe );
	note = listecoup_getNote( lc , aPartirDe );

	listecoup_setCoup( lc , aPartirDe , listecoup_getCoup( lc , indiceBestScore1 ) );
	listecoup_setNote( lc , aPartirDe , bestScore1 );
	listecoup_setCoup( lc , indiceBestScore1 , co );
	listecoup_setNote( lc , indiceBestScore1 , note );

	co = listecoup_getCoup( lc , aPartirDe+1 );
	note = listecoup_getNote( lc , aPartirDe+1 );

	listecoup_setCoup( lc , aPartirDe+1 , listecoup_getCoup( lc , indiceBestScore2 ) );
	listecoup_setNote( lc , aPartirDe+1 , bestScore2 );
	listecoup_setCoup( lc , indiceBestScore2 , co );
	listecoup_setNote( lc , indiceBestScore2 , note );
#endif
#if 0
	for( i=aPartirDe ; i< aPartirDe+nbTrie ; i++ ) {
		if ( i >= listecoup_getNbCoup( lc )-1 ) break;
		for( j=aPartirDe+1 ; j< listecoup_getNbCoup( lc ) ; j++ ) {
			if ( listecoup_getNote( lc , j ) > listecoup_getNote( lc , i ) ) {
				co = listecoup_getCoup( lc , i );
				note = listecoup_getNote( lc , i );
				listecoup_setCoup( lc , i , 
						   listecoup_getCoup( lc , j ) );
				listecoup_setNote( lc , i , listecoup_getNote( lc , j ) );
				listecoup_setCoup( lc , j , co );
				listecoup_setNote( lc , j , note );
			}
		}
	}
#endif
	indiceBestScore1 = aPartirDe;

	for( i= aPartirDe+1 ; i< listecoup_getNbCoup( lc ) ; i++ ) {
	  if ( listecoup_getNote( lc , i ) > listecoup_getNote( lc , indiceBestScore1 ) ) 
		  indiceBestScore1 = i;
	}

	if ( aPartirDe != indiceBestScore1 ) {
		co = listecoup_getCoup( lc , aPartirDe );
		note = listecoup_getNote( lc , aPartirDe );
		listecoup_setNote( lc , aPartirDe , listecoup_getNote( lc , indiceBestScore1 ) );
		listecoup_setCoup( lc , aPartirDe , listecoup_getCoup( lc , indiceBestScore1 ) );
		listecoup_setNote( lc , indiceBestScore1 , note );
		listecoup_setCoup( lc , indiceBestScore1 , co );
	}

}	

/*----------------------------------------------------------------------------------*/
void listecoup_note( Plateau *pl , short prof , ListeCoup *lc , PositionHash *ph ,
	             HistoryTable historyTable , Killer killer , Pv *pvCourant) {

	int i,j;

	for (i=0 ; i < listecoup_getNbCoup( lc ) ; i++ ) {
			/* Coup = coup present dans la hashTable */
		if ( ph != NULL ) {
		  if ( listecoup_getCoup( lc , i ) == ph->meilleurCoup ) {
			listecoup_setNote( lc , i , 100000000 );
			/* TODO : SIMPLIFIER POUR PASSER AU COUP SUIVANT DANS UN PV */
			if ( plateau_getCle(pl) == pv_getCleCourante( pvCourant ) ) 
			  if ( listecoup_getCoup( lc , i ) == pv_getCoupCourant( pvCourant ) ) 
				pv_coupSuivant( pvCourant );
			continue;
		  }
		 }
			/* PV */
	/*	if ( plateau_getCle(pl) == pv_getCleCourante( pvCourant ) ) {
			if ( listecoup_getCoup( lc , i ) == pv_getCoupCourant( pvCourant ) ) {
			listecoup_setNote( lc , i , 90000000 );
			pv_coupSuivant( pvCourant );
			continue;
			}
		}*/
			/* Le coup est une capture */
		if ( coup_isCapture( listecoup_getCoup( lc , i ) ) ) {
			if ( (coup_getPieceCapture( listecoup_getCoup(lc , i)) -
			     coup_getPieceDeplace( listecoup_getCoup(lc , i)) ) >= 0 ) 
			   listecoup_setNote( lc , i , 
					      5000000 + coup_getPieceCapture( listecoup_getCoup(lc , i) )  -
					           coup_getPieceDeplace( listecoup_getCoup(lc , i)  )
					   ) ; 
		        else
			   listecoup_setNote( lc , i , 
					       coup_getPieceCapture( listecoup_getCoup(lc , i) )  -
					       coup_getPieceDeplace( listecoup_getCoup(lc , i) +5000000 )
					   ) ; 

			   continue;
		}
			/* le coup est un roque */
	  	if ( coup_isRoque( listecoup_getCoup( lc , i ) ) ) {
			listecoup_setNote( lc , i , 2000000 );
			continue;
		}

		/* Killer Move 1*/
	  	if ( listecoup_getCoup( lc , i ) ==killer_get(killer,prof,0) ) {
			listecoup_setNote( lc , i , 1500000 );
			continue;
		}

		/* Killer Move 2*/
	  	if ( listecoup_getCoup( lc , i ) ==killer_get(killer,prof,1) ) {
			listecoup_setNote( lc , i , 1300000 );
			continue;
		}

		/* Killer Move 3*/
	  	if ( listecoup_getCoup( lc , i ) ==killer_get(killer,prof,2) ) {
			listecoup_setNote( lc , i , 1000000 );
			continue;
		}
			/* joue les grosses pieces qui se rapprochent du centre en premier */
	/*	listecoup_setNote( lc , i , coup_getPieceDeplace( listecoup_getCoup(lc , i)) +
			       		    32- abs( coup_getCaseDestination( listecoup_getCoup(lc , i) ) - 32 )
					);   */
		    listecoup_setNote( lc , i , 
				       historytable_get(historyTable , p,listecoup_getCoup( lc , i ) )
				       - 10000000 );
	}
	
}


/*----------------------------------------------------------------------------------------------*/
void listecoup_noteCapture( ListeCoup *lc ) {
  register int i;
  
  for (i=0 ; i < listecoup_getNbCoup( lc ) ; i++ ) {
    listecoup_setNote( lc , i ,   
		       coup_getPieceCapture( listecoup_getCoup(lc , i) ) - 
		       coup_getPieceDeplace( listecoup_getCoup(lc , i)) );
  }
}
/*----------------------------------------------------------------------------------------------*/

void printListeCoup( ListeCoup *lc , char *slc ) {
  int i;
  char co[10];

  slc[0] = '\0';

  for( i=0 ; i<listecoup_getNbCoup(lc) ; i++ ) {
	  printCoup( listecoup_getCoup( lc , i ) , co );
	  strcat( slc , co );
	  strcat( slc , " " );
  }  
}

/*----------------------------------------------------------------------------------------------*/
