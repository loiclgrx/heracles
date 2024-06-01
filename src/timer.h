#ifndef TIMER_H
#define TIMER_H

#include <sys/timeb.h>
#include "boolean.h"
#include "couleur.h"
#include "plateau.h"

#define CONTROLE_TEMPS	2000	/* controle le temps toutes les 2s. Le temps est converti
								   en controleTempsNoeud pour des raisons de performance */
typedef struct {
   struct timeb debut; 
   struct timeb fin; 
} MonTimer;

typedef struct {
  int 	tempsBlanc;		/* en ms */
  int 	tempsNoir;
  short incBLanc;		/* en ms */
  short incNoir;	
  short nbCoupsTemps;	/* Nombre de coups avant la prochaine definition du temps */
  short profondeur;		/* recherche limitee a profondeur */
  long  noeuds;			/* rechereche limitee a noeuds */
  short mat;			/* recherche un mat en x coups */
  int 	tempsParCoups;	/* recherche chaque coups en tempsParCoups ms */
  Boolean infinite;		/* recherhce infinie */
  Boolean ponder;		/* recherche pendant que l'adversaire cherche son coup */

  int	tempsMoyenTheorique; 	/* calcul en fonction des donnees du haut un temps moyen theorique
								   par coup en ms */
  int	tempsMoyenReel; 		/* temps moyen reel pour jouer les coups. Si tres different
								   de tempsMoyenTheoriqueCoup alors il faut rectifier les reglages */
  long	controleTempsNoeud;		/* le temps est controle tous les controleTempsNoeud noeuds */
  MonTimer	tempsDuCoup;		/* chronometre le temps mis pour le coup actuel */
  Boolean	stopRecherche;		/* commande stop reçu */
} GestionTemps;

	/* retourne VRAI si la recherche doit s'arreter */
Boolean timer_arreteRecherche( Plateau *p, GestionTemps *gt );

	/* Calcul le temps moyen theorique par coup ..., demarre le chronometre ... */
void	timer_initGestionTemps( GestionTemps *gt , Couleur co );

void timer_initialise( GestionTemps *gt ) ;
void timer_print( GestionTemps *gt );

#define timer_reset( t )	t.debut.time = 0;t.debut.millitm = 0;t.fin.time = 0; t.fin.millitm = 0
#define timer_start( t )	ftime(&(t.debut))
#define timer_stop( t )		ftime(&(t.fin))
#define timer_delta( t )	(((t.fin.time*1000) + (t.fin.millitm)) - \
				((t.debut.time*1000) + (t.debut.millitm)) )
#define timer_pause( t ) 	 timer_stop( t )

#define timer_setPonder( g , b )	( g->ponder = b )
#define timer_setInfinite( g , b )	( g->infinite = b )
#define timer_setTempsBlanc( g , t )	( g->tempsBlanc = t )
#define timer_setTempsNoir( g , t )	( g->tempsNoir = t )
#define timer_setIncBlanc( g , t )	( g->incBLanc = t )
#define timer_setIncNoir( g , t )	( g->incNoir = t )
#define timer_setNbCoupTemps( g , n )	( g->nbCoupsTemps = n )
#define timer_setProfondeur( g , n )	( g->profondeur = n )
#define timer_setNoeud( g , n )		( g->noeuds = n )
#define timer_setMat( g , n )		( g->mat = n )
#define timer_setTempsParCoup( g , t )	( g->tempsParCoups = t )
#define timer_setTempsMoyenTheorique( g , t )	( g->tempsMoyenTheorique= t )
#define timer_setStopRecherche( g , b )	( g->stopRecherche= b )


#define timer_getPonder( g )		( g->ponder )
#define timer_getInfinite( g )		( g->infinite )
#define timer_getTempsBlanc( g )	( g->tempsBlanc )
#define timer_getTempsNoir( g )		( g->tempsNoir )
#define timer_getIncBlanc( g )		( g->incBLanc )
#define timer_getIncNoir( g )		( g->incNoir )
#define timer_getNbCoupTemps( g )	( g->nbCoupsTemps )
#define timer_getProfondeur( g )	( g->profondeur )
#define timer_getNoeud( g )			( g->noeuds )
#define timer_getMat( g )			( g->mat )
#define timer_getTempsParCoup( g )	( g->tempsParCoups )
#define timer_getTimer( g )			( g->tempsDuCoup )
#define timer_getTempsMoyenTheorique( g ) ( g->tempsMoyenTheorique )
#define timer_getStopRecherche( g )		  ( g->stopRecherche )

#define TEMPS_INFINI	-1
#endif /* TIMER_H */

