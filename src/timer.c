#include <stdio.h>
#include "timer.h"
#include "couleur.h"
#include "util.h"
#include "uci.h"
#include "boolean.h"



void timer_initialise( GestionTemps *gt ) {
  timer_setPonder( gt , FAUX );
  timer_setInfinite( gt , FAUX );
  timer_setTempsBlanc( gt , 0 );
  timer_setTempsNoir( gt , 0 );
  timer_setIncNoir( gt , 0 );
  timer_setIncBlanc( gt , 0 );
  timer_setNbCoupTemps( gt , 0 );
  timer_setProfondeur( gt , 0 );
  timer_setNoeud( gt , 0 );
  timer_setMat( gt , 0 );
  timer_setTempsParCoup( gt , 0 );
  timer_setStopRecherche( gt , VRAI );
}

/*------------------------------------------------------------------------------------*/

void timer_print( GestionTemps *gt ) {
  printf("Ponder %d\n", timer_getPonder( gt ) );
  printf("Infinite %d\n", timer_getInfinite( gt ) );
  printf("Temps blanc %d\n", timer_getTempsBlanc( gt ));
  printf("Temps noir %d\n", timer_getTempsNoir( gt ));
  printf("Inc Noir %d\n", timer_getIncNoir( gt ));
  printf("Inc blanc %d\n", timer_getIncBlanc( gt ));
  printf("Nb coups temps %d\n", timer_getNbCoupTemps( gt ));
  printf("Profondeur %d\n", timer_getProfondeur( gt ));
  printf("Noeud %d\n", timer_getNoeud( gt ));
  printf("Mat %d\n", timer_getMat( gt ));
  printf("Temps par coups %d\n", timer_getTempsParCoup( gt ));
  printf("Stop %d\n", timer_getStopRecherche( gt ));
  printf("Temps moyen par coup %d\n", timer_getTempsMoyenTheorique( gt ) );
  fflush( stdout );
}

/*------------------------------------------------------------------------------------*/

	/* Calcul le temps moyen theorique par coup ..., demarre le chronometre ... */
void	timer_initGestionTemps( GestionTemps *gt , Couleur co ) {
	int tempsJoueur,incJoueur,nbRestant,nbCoupsTemps;
	float coef;

//	timer_reset( timer_getTimer(gt) ); 
//	timer_start( timer_getTimer(gt) );

	timer_setStopRecherche( gt , FAUX );

	if ( co == BLANC ) {
		tempsJoueur = timer_getTempsBlanc( gt );
		incJoueur = timer_getIncBlanc( gt );
	}
	else {
		tempsJoueur = timer_getTempsNoir( gt );
		incJoueur = timer_getIncNoir( gt );
	}

	if ( timer_getNbCoupTemps(gt) != 0 ) 
		nbCoupsTemps =  timer_getNbCoupTemps(gt);
	else
		nbCoupsTemps =  32; // TODO ATTENTION A MODIFIER IMPERATIVEMENT

    //TODO A SUPPRIMER
//	tempsJoueur = 120100;
//	nbCoupsTemps = 1;
//	incJoueur = 0;

	if ( nbCoupsTemps >1 )coef = 0.94; else coef = 0.80;

	/* TODO : tempsMoyenTheorique est en ms verifier que incJoueur et bien ausi en ms et pas en s */
	if (  timer_getProfondeur(gt) != 0  ) {
	   timer_setTempsMoyenTheorique( gt , TEMPS_INFINI );  
	}
	else {
		if ( timer_getTempsParCoup( gt ) != 0 ) {
			 timer_setTempsMoyenTheorique( gt , timer_getTempsParCoup(gt) * coef );
		}
		else {
		   timer_setTempsMoyenTheorique( gt , (int)(((tempsJoueur * coef )/nbCoupsTemps)+incJoueur) );
		}
	}

}

/*------------------------------------------------------------------------------------*/

Boolean timer_arreteRecherche( Plateau *p , GestionTemps *gt ) {

   if ( timer_getStopRecherche( gt ) ) return VRAI; 

   timer_pause( timer_getTimer(gt) );
		
   //printf("%ld\n", timer_delta( timer_getTimer(gt) ) ); fflush(stdout);
   if ( ( timer_getPonder( gt ) == VRAI ) || (timer_getInfinite( gt )== VRAI )  ) return FAUX;

		/* Si recherche à une profondeur donnée : encore utile ? */
   if ( timer_getTempsMoyenTheorique(gt) == TEMPS_INFINI ) return FAUX;

	//printf("%ld\n", timer_delta( timer_getTimer(gt) ) ); fflush(stdout);
   if ( timer_delta( timer_getTimer(gt) ) > timer_getTempsMoyenTheorique(gt) ) {
	return VRAI;
   }
   else
	return FAUX;
}

