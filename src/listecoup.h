#ifndef LISTECOUP_H
#define LISTECOUP_H

#include "boolean.h"
#include "coup.h"
#include "plateau.h"
#include "constante.h"
#include "hash.h"
#include "historytable.h"
#include "killer.h"

#define NB_TRIE 6
/*-----------------------------------------------------*/

typedef struct {
  Coup coup;
  long note;
  int64 cle;	/* Cle zobrist de la position */
} CoupNote;

typedef struct {
	CoupNote listeCoup[MAX_COUPS];
	unsigned char nbCoup;
} ListeCoup;

typedef struct {
	int score;			/* Score de la variation */
	int profondeur;		/* Profondeur de recherche de la variante */
	Boolean exact;		/* Indique si la liste de coups correspond a une recherche exacte */
	long nbNoeud;		/* Nombre de noeuds parcouru pour cette varainte */
	ListeCoup lc;		/* Liste des coups de la variation */
	short position;		/* Emplacement du curseur dans la variation */
} Pv;

/*-----------------------------------------------------*/

void printListeCoup( ListeCoup * , char *);
void listecoup_trie(  ListeCoup *lc , short aPartirDe , short nbTrie);
void listecoup_note( Plateau *pl , short prof , ListeCoup *lc , PositionHash *ph ,
	             HistoryTable historyTable , Killer killer , Pv *PvCourant); 

/*-----------------------------------------------------*/

#define listecoup_getNbCoup( lc )	((lc)->nbCoup)

/* Ajoute le coup sans verifier sa validite ( Utilise pour la comparaison de perf avec crafty ) */


#define listecoup_addCoup( lc ,cao,cad,f,co )	((lc)->listeCoup[ (lc)->nbCoup ]).coup = coup_newCoup(cao,cad,f) ;\
						((lc)->nbCoup)++ 
						 
#define listecoup_addCoup2( lc ,c )	((lc)->listeCoup[ (lc)->nbCoup ]).coup = c ;\
					((lc)->nbCoup)++ 

#define listecoup_setCoup( lc , p , c )	((lc)->listeCoup[ p ]).coup = c
#define listecoup_setNote( lc , p , n )	((lc)->listeCoup[ p ]).note = n
#define listecoup_setCle( lc , p , c )	((lc)->listeCoup[ p ]).cle = c


#define listecoup_supprimeDernierCoup( lc )	((lc)->nbCoup)--
#define listecoup_getCoup( lc , n )		(((lc)->listeCoup[ n ]).coup)
#define listecoup_getNote( lc , n )		(((lc)->listeCoup[ n ]).note)
#define listecoup_getCle( lc , n )		(((lc)->listeCoup[ n ]).cle)
#define listecoup_videListeCoup( lc )		((lc)->nbCoup)=0
#define listecoup_copie( lcori , lcdest ) 	memcpy( (lcdest)->listeCoup , (lcori)->listeCoup , \
							sizeof(CoupNote) * ((lcori)->nbCoup) );      \
						(lcdest)->nbCoup = (lcori)->nbCoup

#define listecoup_copieN( lcori , lcdest , i) 	memcpy( &((lcdest)->listeCoup[i]) , (lcori)->listeCoup , \
							sizeof(CoupNote) * ((lcori)->nbCoup) );      \
						(lcdest)->nbCoup = (lcori)->nbCoup+i;
/*-----------------------------------------------------*/
#define pv_getScore( p )					( (p)->score )
#define pv_getProfondeur( p )				( (p)->profondeur )
#define pv_getExact( p )				( (p)->Exact )
#define pv_getNbNoeud( p )				( (p)->nbNoeud )
#define pv_setScore( p  , s)				( ((p)->score) = s )
#define pv_setPosition( p  , s)				( ((p)->position) = s )
#define pv_setProfondeur( p , prof )		( ((p)->profondeur) = prof )
#define pv_setExact( p , b )		( ((p)->exact) = b )
#define pv_setNbNoeud( p , n )		( ((p)->nbNoeud) = n )
#define pv_getListeCoup( p ) 		( &((p)->lc) )
#define pv_allerAuDebut( p )		( ((p)->position)=0 )
#define pv_coupSuivant( p )		 if ( ((p)->position)<listecoup_getNbCoup(&((p)->lc))) \
						((p)->position)++ 
#define pv_coupPrecedent( p )		( if ( ((p)->position)>0 ) ((p)->position)-- )
#define pv_getCleCourante( p )		( listecoup_getCle( &((p)->lc) , (p)->position ) )
#define pv_getCoupCourant( p )		( listecoup_getCoup( &((p)->lc) , (p)->position ) )


#endif /* LISTECOUP_H*/

