#ifndef MULTIPV_H
#define MULTIPV_H

#include "constante.h"
#include "listecoup.h"


typedef struct {
	Pv* 	multiPv[MAX_COUPS];
	short 	nbPV;		/* Nombre de variantes */
} MultiPv;

extern MultiPv  mpv;

void multipv_trie( MultiPv *mpv );
void multipv_init( MultiPv *mpv , ListeCoup *lc );  /* creer autant de PV qu'il y a de coup dans lc */
void multipv_alloc( MultiPv *mpv );  /* Allocation memoire des PV */
void multipvPrint( MultiPv *mpv );

#define multipv_effacer( mpv )		( ((mpv)->nbPV) = 0 )

#define multipv_addPV( mpv , lc , s  )	 listecoup_copie( lc , \
					   pv_getListeCoup(multipv_getPv(mpv, multipv_getNbPV(mpv)) )); \
					 pv_setScore( multipv_getPv( mpv , multipv_getNbPV(mpv) ) ,s );\
					 pv_setPosition( multipv_getPv( mpv , multipv_getNbPV(mpv) ) ,0);\
					 ( (mpv)->nbPV)++;  

#define multipv_setPV(mpv , i , lc , s)  listecoup_copie(lc,pv_getListeCoup(multipv_getPv(mpv,i))); \
					 (((mpv)->multiPv)[i])->score = s; \
					 (((mpv)->multiPv)[i])->position =0 

#define multipv_setPVN(mpv ,i,lc,n,s)  listecoup_copieN(lc,pv_getListeCoup(multipv_getPv(mpv,i)),n); \
				       (((mpv)->multiPv)[i])->score = s; \
				       (((mpv)->multiPv)[i])->position =0 

#define multipv_setScore( mpv , i , s )		( ((mpv)->multiPv)[i]->score = s )
#define multipv_setPosition( mpv , i , p )	( ((mpv)->multiPv)[i]->position = p )
#define multipv_setProfondeur( mpv , i , p )	( ((mpv)->multiPv)[i]->profondeur = p )
#define multipv_setExact( mpv , i , b )		 pv_setExact( ((mpv)->multiPv)[i] , b )
#define multipv_setNbNoeud( mpv , i , n )	 pv_setNbNoeud( ((mpv)->multiPv)[i] , n )
#define multipv_setNbPv( mpv , n )				(( (mpv)->nbPV ) = n )
#define multipv_getPv( mpv , i )				( ((mpv)->multiPv)[i] )
#define multipv_getScore( mpv , i )				( (((mpv)->multiPv)[i])->score )
#define multipv_getPosition( mpv , i )			( (((mpv)->multiPv)[i])->position )
#define multipv_getProfondeur( mpv , i )		( (((mpv)->multiPv)[i])->profondeur )
#define multipv_getExact( mpv , i )		pv_getExact(  ((mpv)->multiPv)[i] )
#define multipv_getNbNoeud( mpv , i )		pv_getNbNoeud(  ((mpv)->multiPv)[i] )
#define multipv_getNbPV( mpv )					( (mpv)->nbPV )
#define multipv_getPremierCoup( mpv , i )	listecoup_getCoup( pv_getListeCoup( multipv_getPv(mpv,i) ) ,0 )

#endif /* MULTIPV_H */
