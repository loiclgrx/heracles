#include <stdio.h>
#include <assert.h>
#include "listecoup.h"
#include "generecoup.h"
#include "couleur.h"
#include "init.h"
#include "constante.h"
#include "evaluation.h"
#include "case.h"
#include "bitboard.h"
#include "trace.h"
#include "hasheval.h"
#include "valeur.h"

	/* BONUS TOUR */
#define B_TOUR_COLONNE_OUVERTE	10
#define B_TOUR_2_7_LIGNE	10
#define B_TOUR_ALIGNEES		10

	/* BONUS ROQUE */
#define B_ROQUE			50	

	/* MALUS PION ROQUE ABSENT */
#define B_PION_ROQUE		25	

	/* Score pour chaque piece qui attaque les cases voisines du roi 
	 * Si une piece attaque une case alors score de 10 par exemple
	 * Si 2 pieces attaquent la meme case voisine alors score de 30 par exemple...*/
short scoreAttaqueCaseVoisineRoi[] = { 0 , 5 , 30 , 60 , 90 , 100 , 110 , 120 , 130 ,140,150,160,170,180,190,200} ;	

		/* Score du roi BLANC en fonction de sa position sur l'echiquier */

		/* BASE STATISTIQUE SUR LES FICHIERS TWIC */
								  //  A    B     C     D     E     F    G     H	
short	positionRoiDebutMilieu[] = { 1  , 10  , 15  ,  3  ,  2  ,  8  , 36 , 12  ,	// 1
									 0  ,  2  ,  3  ,  4  ,  4  ,  8  , 10 , 10  ,	// 2
									-1  ,  0  ,  1  ,  2  ,  2  ,  2  ,  2 ,  1  ,	// 3
									-2  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0 ,  0  ,	// 4
									-11 , -9  , -7  , -5  , -5  , -7  , -9 , -11 ,	// 5
									-13 , -11 , -9  , -7  , -7  , -9  , -11, -13 ,	// 6
									-15 , -13 , -11 , -9  ,  -9 , -11 , -13, -15 ,	// 7
									-17 , -15 , -13 , -11 , -11 , -13 , -15, -17 }; 	// 8

		/* AU PIFOMETRE */
						  //  A    B    C   D    E     F    G   H	
short	positionRoiFin[] = { -10, -5 , -5 , -5 , -5 , -5 , -5 , -10,	// 1
							 -5 ,  0 ,  0 , 0  , 0  ,  0 ,  0 , -5 ,	// 2
							 -5 ,  0 ,  5 , 5  , 5  ,  5 ,  0 , -5 ,	// 3
							 -5 ,  0 ,  5 , 15 , 15 ,  5 ,  0 , -5 ,	// 4
							 -5 ,  0 ,  5 , 15 , 15 ,  5 ,  0 , -5 ,	// 5
							 -5 ,  0 ,  5 ,  5 ,  5 ,  5 ,  0 , -5 ,	// 6
							 -5 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -5 ,	// 7
							 -10, -5 , -5 , -5 , -5 , -5 , -5 , -10 }; 	// 8

		/* BASE STATISTIQUE SUR LES FICHIERS TWIC */
							  	  //  A     B     C    D    E    F    G      H	
short	positionCavalierDebut[] = { -30  ,  -8 ,  -8,  -6,  -2,  -1, -25 , -30 ,	// 1
									  -8 ,  -22,  -3,  8 ,   5,  -8,  -22, -23 ,	// 2
									  1  ,   2 ,  30,  -6,  -6,  37,   0 ,   -1,	// 3
									 -20 ,  -17,   0,  9 ,   2,  -1,  -17,    0,	// 4
									 -20 ,   1 ,  -7,   1,   3,  -6,   0 ,   -8,	// 5
									 -20 ,   -8,   1,  -8, -19,  -2,   -5,  -20,	// 6
									 -22 ,  -21, -20,  -7, -20,  -7, -21 ,  -22,	// 7
									 -25 , -24 , -23, -23, -23, -23, -24 , -25 }; 	// 8

		/* BASE STATISTIQUE SUR LES FICHIERS TWIC pour toutes les cases */
					  	          //  A     B   C   D   E   F    G    H	
short	positionCavalierMilieu[] = {  0 ,   3,  5,  4,  6, 10,   2,   0,	// 1
									   1,   2,  5, 30, 30,  3,   1,   4,	// 2
									   5,  12, 24, 10, 13, 26,  16,   2,	// 3
									  12,   4, 19, 31, 35, 13,   6,   8,	// 4
									   5,  15, 14, 26, 29, 14,  15,   4,	// 5
									   0,   5, 12, 11,  8,  9,   4,   1,	// 6
									   2,   2,  1,  5,  4,  3,   1,   0,	// 7
									   0,   0,  1,  1,  0,  1,   0,   0  }; 	// 8
#if 0
		// les '*' sont des valeurs statistiques
					  	     //  A     B     C    D    E    F    G      H	
short	positionFouDebut[] = { -20 ,  -15, *-1, *-1, -15,  *0, -15 , -20 ,	// 1
								*-1,  *7 ,  *2,  *9, *20, *-1,  *35, *-2 ,	// 2
								*0 ,   *8,  *1, *31, *28,  *2,   *3,  *-2,	// 3
								*5 ,   *0, *28,  *0,  *1, *11,   -8,   *5,	// 4
							   -15 ,  *17, *-2,  *1,  *0, *-1,  *27,  -15,	// 5
							   *-1 ,  -10,  *4,  *0,  *0,  *4,  *-2,  *-1,	// 6
							   -22 ,  -21, -20,  *2,  *1, -20, -21 ,  -22,	// 7
							   -20 ,  -15, -13, -12, -12, -13, -15 ,  -20 }; 	// 8
#endif
					  	     //  A     B     C    D    E    F    G      H	
short	positionFouDebut[] = { -20 ,  -15,  -1,  -1, -15,   0, -15 , -20 ,	// 1
								 -1,   7 ,   2,   9,  20,  -1,   35,  -2 ,	// 2
								 0 ,    8,   1,  31,  28,   2,    3,   -2,	// 3
								 5 ,    0,  28,   0,   1,  11,   -8,    5,	// 4
							   -15 ,   17,  -2,   1,   0,  -1,   27,  -15,	// 5
								-1 ,  -10,   4,   0,   0,   4,   -2,   -1,	// 6
							   -22 ,  -21, -20,   2,   1, -20, -21 ,  -22,	// 7
							   -20 ,  -15, -13, -12, -12, -13, -15 ,  -20 }; 	// 8

		/* BASE STATISTIQUE SUR LES FICHIERS TWIC pour toutes les cases */
					  	      //  A     B    C    D    E    F    G      H	
short	positionFouMilieu[] = {  -1 ,   4 ,  6 ,  3 ,  2 ,  8 ,   0 ,  -1 ,	// 1
								  1 ,  14 ,  9 , 19 , 17 ,  5 ,   7 ,   0 ,	// 2
								  6 ,  11 , 10 , 21 , 35 , 17 ,   7 ,   9 ,	// 3
								  4 ,   4 , 13 , 19 , 13 , 24 ,   5 ,   5 ,	// 4
								  2 ,  11 , 11 , 11 , 10 ,  6 ,  21 ,   3 ,	// 5
								  4 ,   4 ,  7 ,  8 ,  7 , 14 ,   3 ,   7 ,	// 6
								  1 ,   4 ,  1 ,  3 ,  7 ,  1 ,   6 ,   2 ,	// 7
								  1 ,  -1 ,  0 ,  0 , -1 ,  0 ,  -2 ,  -1  }; 	// 8
			#if 0
		// les '*' sont des valeurs statistiques
					  	      //  A     B    C    D    E    F    G      H	
short	positionDameDebut[] = {  -1 ,   4 ,  *1, *-2,  *1,  8 ,   0 ,  -1 ,	// 1
								  *0,  *-2,  14, *9 , *10, *7 , *-1,   7 , 	// 2
								  *3,  *5 , *1 , *2 , *0 , *3 ,   *0,   9 ,	// 3
								  4 ,   4 , *1 , *2 , *0 , *0 ,   *0,  *-1,	// 4
								  2 ,  11 , 11 , *0 , *-1, *-1,  *-2,   *1,	// 5
								  4 ,  *-1,  7 ,  8 ,  7 , 14 ,   3 ,   7 ,	// 6
								  1 ,  *-1,  1 ,   0, *-2,  1 ,   6 ,   2 ,	// 7
								  1 ,  -1 ,  0 ,  *0, -1 ,  0 ,  -2 ,  -1  }; 	// 8
			#endif
		/* BASE STATISTIQUE SUR LES FICHIERS TWIC pour toutes les cases */
				  	  	       //  A     B    C     D      E     F     G      H	
short	positionDameDebut[] = {  -20 , -15 ,  1  , -2  ,   1 ,  -10,  -15,  -20,	// 1
								  0  ,  -2 , 14  ,  9  ,  10 ,   7 ,   -1, -15 ,   	// 2
								  3  ,   5 ,  1  ,  2  ,   0 ,   3 ,    0,  -13,	// 3
								 -12 , -10 ,  1  ,  2  ,   0 ,   0 ,    0,   -1,	// 4
								 -12 , -10 , -10 ,  0  ,   -1,   -1,   -2,    1,	// 5
								 -12 ,  -1 , -10 , -10 ,  -10,  -10,  -12,  -12,	// 6
								 -12 ,  -1 , -10 , -10 ,   -2, -10 ,  -12,  -12,	// 7
								  -15, -15 , -15 ,   0 ,   -15, -15,  -15,  -15  }; 	// 8

	/* Convertie une case d'une piece noir par une symetrie axiale pour obtenir son score */

						  //  A    B    C   D    E     F    G   H	
short symetrieAxiale[]	=  { A8 , B8 , C8 , D8 , E8 , F8 , G8 , H8 ,	// 1 
       						 A7 , B7 , C7 , D7 , E7 , F7 , G7 , H7 ,	// 2
		     				 A6 , B6 , C6 , D6 , E6 , F6 , G6 , H6 ,	// 3
							 A5 , B5 , C5 , D5 , E5 , F5 , G5 , H5 ,	// 4
							 A4 , B4 , C4 , D4 , E4 , F4 , G4 , H4 ,	// 5
							 A3 , B3 , C3 , D3 , E3 , F3 , G3 , H3 ,	// 6
							 A2 , B2 , C2 , D2 , E2 , F2 , G2 , H2 ,	// 7
							 A1 , B1 , C1 , D1 , E1 , F1 , G1 , H1 };	// 8	     

int phase;		// Indique si je suis en debut , milieu ou fin de partie


/*-----------------------------------------------------------------------------------------------*/

/* Retourne la difference de mobilite entre les pieces noires et blanches. cp est un coefficient de pond�ration qui peut
 * varier entre le debut , le milieu et la fin de partie */

int eval_mobilite( Plateau *p , short cp ) {
 ListeCoup lc;
 int delta;
 Plateau p1;

 p1=*p;

 listecoup_videListeCoup( &lc );
 genereCoup( &lc , p , BLANC ); 
 delta = listecoup_getNbCoup( &lc );
 

 listecoup_videListeCoup( &lc );
 genereCoup( &lc , p , NOIR ); 
 delta -= listecoup_getNbCoup( &lc );

 return delta*cp;
}




/*-----------------------------------------------------------------------------------------------*/

/* Donne un malus important aux fous et , pions centraux toujours sur leur case d'origine 
 * Un malus si la dame voyage de trop.
 */

int eval_developpement( Plateau *p ) {
  BitBoard bb;
  int score;
  Case ca;

  score = 0;

	/* Developpement des BLANCS */

  	/* Score des cavaliers BLANC en debut de partie */

  bb = p->bbCavalier[ BLANC ];
  while ( bb ) {
	ca = getFirstBit( bb );
  	score += positionCavalierDebut[ ca ];
	videBit( bb , ca );
  }
  
  	/* Score des cavaliers NOIR en debut de partie */

  bb = p->bbCavalier[ NOIR ];
  while ( bb ) {
	ca = getFirstBit( bb );
  	score -= positionCavalierDebut[ symetrieAxiale[ca] ];
	videBit( bb , ca );
  }

  	/* Score des fou BLANC en debut de partie */

  bb = p->bbFou[ BLANC ];
  while ( bb ) {
	ca = getFirstBit( bb );
  	score += positionFouDebut[ ca ];
	videBit( bb , ca );
  }
  
  	/* Score des fou NOIR en debut de partie */

  bb = p->bbFou[ NOIR ];
  while ( bb ) {
	ca = getFirstBit( bb );
  	score -= positionFouDebut[ symetrieAxiale[ca] ];
	videBit( bb , ca );
  }


  	/* Score de la dame BLANC en debut de partie */

  bb = p->bbDame[ BLANC ];
  while ( bb ) {
	ca = getFirstBit( bb );
  	score += positionDameDebut[ ca ];
	videBit( bb , ca );
  }
  
  	/* Score de la dame NOIR en debut de partie */

  bb = p->bbDame[ NOIR ];
  while ( bb ) {
	ca = getFirstBit( bb );
  	score -= positionDameDebut[ symetrieAxiale[ca] ];
	videBit( bb , ca );
  }

  /* Nombre de pions centraux sur leurs cases d'origines */
  score -= nbBits[(p->bbPion[BLANC] &  CASE_INIT_PION_CENTRE_BLANC )]*30;


	/* Developpement des NOIRS */

  
  /* Nombre de pions centraux sur leurs cases d'origines */
  score += nbBits[(p->bbPion[NOIR] &  CASE_INIT_PION_CENTRE_NOIR )>>48]*30;


  return score;
}

/*-----------------------------------------------------------------------------------------------*/
/* Bonus pour tous pions situes au centre
 * Bonus pour chaque piece attaquant le centre */

int eval_controleCentre( Plateau *p ) {
	int centreBlanc,centreNoir;

	centreBlanc = centreNoir = 0;

	/* Controle du centre par les BLANCS */

	/* Bonus pour chaque pion situes sur une case centrale */

	centreBlanc += nbBits[(p->bbPion[BLANC] & CASE_CENTRALE)>>24]*20;

	/* TODO : Bonus pour chaque piece attaquant le centre */
	
	/* Controle du centre par les NOIRS */
	
	/* Bonus pour chaque pion situes sur une case centrale */

	centreNoir += nbBits[(p->bbPion[NOIR] & CASE_CENTRALE)>>24]*20;

	/* TODO : Bonus pour chaque piece attaquant le centre */

	return centreBlanc - centreNoir;
}

/*-----------------------------------------------------------------------------------------------*/
int eval_securiteRoi( Plateau *p ) {
  int score =0;	
  Case ca;
  BitBoard nbPion = 0 , bbDeplacementRoi;


  /* Bonus si le joueur a roquer */

  if ( !plateau_isEndGame( p ) ) {
    			
    			/* SCORE ROI BLANC */

				/* LE ROQUE */
				
  	if ( (p->bbRoi[ BLANC ] & CASE_F1G1H1) &&
       	     (p->bbTour[ BLANC ] & CASE_E1F1) ) {		/* petit roque BLANC */
	  	score += B_ROQUE;
  	  	score -= (3 - getNbBits( p->bbPion[BLANC] & CASE_F2G2H2 )) * B_PION_ROQUE;
  	}
  	else if ( (p->bbRoi[ BLANC ] & CASE_A1B1C1 ) &&	/* grand roque BLANC */	
	          (p->bbTour[ BLANC ] & CASE_C1D1E1)  ) {
	  	score += B_ROQUE;
  	  	score -= (3 - getNbBits( p->bbPion[BLANC] & CASE_A2B2C2 )) * B_PION_ROQUE;
  	} else {						/* Malus si les BLANC ont deroque */
	  	if ( !plateau_isPetitRoque( p , BLANC ) && !plateau_isGrandRoque( p , BLANC ) )
		  score -= B_ROQUE;
  	}


  			/* SCORE ROI NOIR */

	if ( (p->bbRoi[ NOIR ] & CASE_F8G8H8) &&
       	       (p->bbTour[ NOIR ] & CASE_E8F8) ) {		/* petit roque NOIR */
	  	score -= B_ROQUE;
  	  	score += (3 - getNbBits( p->bbPion[NOIR] & CASE_F7G7H7 )) * B_PION_ROQUE;
  	}
  	else if ( (p->bbRoi[ NOIR ] & CASE_A8B8C8 ) &&	/* grand roque NOIR */	
	    	  (p->bbTour[ NOIR ] & CASE_C8D8E8)  ) {
	  	score -= B_ROQUE;
  	  	score += (3 - getNbBits( p->bbPion[NOIR] & CASE_A7B7C7 )) * B_PION_ROQUE;
  	} else {						/* BOnus si les NOIR ont deroque */
	  	if ( !plateau_isPetitRoque( p , NOIR ) && !plateau_isGrandRoque( p , NOIR ) )
		  score += B_ROQUE;
  	}


			/* Score de l'emplacement du roi en debut et milieu de partie */
	score += positionRoiDebutMilieu[ plateau_getEmplacementRoi( p , BLANC ) ];
	score -= positionRoiDebutMilieu[ symetrieAxiale[plateau_getEmplacementRoi( p , NOIR  )] ];
	
	 TRACE_EVAL1("score roi : %d\n", score );
  }
  else {	// Evaluation du roi en fin de partie
	  score += positionRoiFin[ plateau_getEmplacementRoi( p , BLANC ) ];
	  score -= positionRoiFin[ plateau_getEmplacementRoi( p , NOIR  ) ];
  }
  TRACE_EVAL1("score roi : %d\n", score );
  return score;
}


/*------------------------------------------------------------------*/

int eval_pionBis( Plateau *p ) { 
	//BitBoard pionObstacle;
	//BitBoard pionPasse;
	BitBoard bbPion,bbPionColonne;
	int scorePionBlanc,scorePionNoir;
	int scoreBasePionPasse,scoreCasePionPasse;
	Case casePion;


	
		/* Definition du score d'un pion passe en fonction de l'avancement de la partie 
		   et de la distance entre le pion et la 8(ou 1) rangee */
	if ( plateau_isOpening(p) ) {
			scoreBasePionPasse = 5;
			scoreCasePionPasse = 10;
	}
	else {
		if ( plateau_isEndGame(p) ) {
			scoreBasePionPasse =  50;
			scoreCasePionPasse =  20;
		}
		else {	/* Milieu de partie */
			scoreBasePionPasse =  15;
			scoreCasePionPasse =  10;
		}
	}


	scorePionBlanc = scorePionNoir = 0 ;

		/* SCORE DES PIONS BLANCS */

	bbPion = p->bbPion[ BLANC ];

	while ( bbPion ) {
		casePion = getFirstBit( bbPion );
		
			/* recupere les pions situes sur la meme colonne */
		bbPionColonne = p->bbPion[ BLANC ] & 
						getDroiteContenantCases(  casePion , casePion + 8 );

		if ( getNbBits( bbPionColonne ) > 1 ) {
			/* Pion double */	
			scorePionBlanc -= 10;
					}
		
		
		if ( (getPionAcolyteBlanc( casePion ) & p->bbPion[BLANC]) == 0 ) {
			/* pion arriere */
			scorePionBlanc -= 10;

		   if ( (getPionAcolyteNoir( casePion ) & p->bbPion[BLANC]) == 0 ) {
			/* pion isole */
			scorePionBlanc -= 10;
		   }
		}
		if ( (getPionPasseBlanc( casePion ) & p->bbPion[NOIR] ) == 0 ) {
			/* pion passe */
			/* plus le pion est proche de la 8eme rangee plus son score est important */
			scorePionBlanc += scoreBasePionPasse + (( casePion >> 3 )* scoreCasePionPasse);
		}

		videBit( bbPion , casePion );
	}

		/* SCORE DES PIONS NOIRS */

	bbPion = p->bbPion[ NOIR ];

	while ( bbPion ) {
		casePion = getFirstBit( bbPion );
		
			/* recupere les pions situes sur la meme colonne */
		bbPionColonne = p->bbPion[ NOIR ] & 
				getDroiteContenantCases(  casePion , casePion + 8 );

		if ( getNbBits( bbPionColonne ) > 1 ) {
			/* Pion double */	
			scorePionNoir -= 10;
		}
		if ( ( getPionAcolyteNoir( casePion ) & p->bbPion[ NOIR ] ) == 0 ) {
			/* pion arriere */
			scorePionNoir -= 10;

		   if ( ( getPionAcolyteBlanc( casePion ) & p->bbPion[ NOIR ] ) == 0 ) {
			/* pion isole */
			scorePionNoir -= 10;
					   }
		}
		if ( ( getPionPasseNoir( casePion ) & p->bbPion[ BLANC ] ) == 0 ) {
			/* pion passe */
			/* plus le pion est proche de la 8eme rangee plus son score est important */
			scorePionNoir += scoreBasePionPasse + ( (63 - casePion) >> 3 ) * scoreCasePionPasse;
		}

		videBit( bbPion , casePion );
	}


	return scorePionBlanc - scorePionNoir;
}

/*-----------------------------------------------------------------------------------------------*/
int eval_pion( Plateau *p ) { 
	int scorePionBlanc,scorePionNoir,scorePionMixte;
	PositionHashEval *pheBlanc = NULL ,*pheNoir = NULL , *pheMixte = NULL;

	BitBoard bbPion,bbPionColonne;
	int scoreBasePionPasse,scoreCasePionPasse;
	Case casePion;

			/*---------------------------*/
			/* Si pas de pion c'est fini */
			/*---------------------------*/
	
	if ( p->bbPion[ BLANC ] == 0 && p->bbPion[ NOIR ] == 0 ) return 0; 
	
	scorePionBlanc =  scorePionNoir = scorePionMixte = 0 ;	

			/*-------------------------------------------------*/
			/* Si pas de pion Blanc score null pour les blancs */
			/*-------------------------------------------------*/

	if ( p->bbPion[ BLANC ] == 0 ) 
  		scorePionBlanc = 0;
	else {
		hasheval_get( &hashPion[BLANC] , plateau_getClePion( p , BLANC ) ,  &pheBlanc );
		if ( pheBlanc != NULL )  scorePionBlanc = positionHashEval_getScore( pheBlanc ); 
	}
			/*-----------------------------------------------*/
			/* Si pas de pion Noir score null pour les noirs */
			/*-----------------------------------------------*/		

	if ( p->bbPion[ NOIR ] == 0 ) 
		scorePionNoir = 0;
	else {
		hasheval_get( &hashPion[NOIR] , plateau_getClePion( p , NOIR ) ,  &pheNoir );
		if ( pheNoir != NULL )  scorePionNoir = positionHashEval_getScore( pheNoir ); 
	}

			/*-------------------------------------------------------------*/
			/* Recupere le score des pions passes dans la table de hachage */
			/*-------------------------------------------------------------*/		

	hasheval_get( &hashPionMixte , 
							  plateau_getClePionMixte( p )^getZobristPhase(phase) , &pheMixte );

	if ( pheMixte != NULL )  scorePionMixte = positionHashEval_getScore( pheMixte ); 

			/*----------------------------------------------------------------------------*/
			/* Si tous est present dans la table de Hash on retourne le score directement */ 
			/*----------------------------------------------------------------------------*/		

	if ( (pheNoir  != NULL || p->bbPion[ NOIR ]  == 0 )  && 
	     (pheBlanc != NULL || p->bbPion[ BLANC ] == 0 )  &&    pheMixte != NULL )  {
 		return scorePionBlanc - scorePionNoir + scorePionMixte;
	}
	

			/*-------------------------------------------*/
			/*  Fixe le score des differentes situations */
			/*-------------------------------------------*/

		/* Definition du score d'un pion passe en fonction de l'avancement de la partie 
		   et de la distance entre le pion et la 8(ou 1) rangee */

	if ( phase == DEBUT ) {
			scoreBasePionPasse = 5;
			scoreCasePionPasse = 10;
	}
	else {
		if ( phase == FIN ) {
			scoreBasePionPasse =  50;
			scoreCasePionPasse =  20;
		}
		else {	/* Milieu de partie */
			scoreBasePionPasse =  15;
			scoreCasePionPasse =  10;
		}
	}


	if ( (pheBlanc != NULL && pheMixte != NULL) || p->bbPion[ BLANC ] == 0 ) 
			goto eval_score_noir;


			/*-----------------------------------*/
			/*  Evalue le score des pions Blancs */
			/*-----------------------------------*/

	bbPion = p->bbPion[ BLANC ];

	while ( bbPion ) {
		casePion = getFirstBit( bbPion );
		
		if ( pheBlanc == NULL ) {


				/* recupere les pions situes sur la meme colonne */
			bbPionColonne = p->bbPion[ BLANC ] & 
					getDroiteContenantCases(  casePion , casePion + 8 );
	
			if ( getNbBits( bbPionColonne ) > 1 ) {
				/* Pion double */	
				scorePionBlanc -= 10;
			}
			
			
			if ( (getPionAcolyteBlanc( casePion ) & p->bbPion[BLANC]) == 0 ) {
				/* pion arriere */
				scorePionBlanc -= 10;
	
			   if ( (getPionAcolyteNoir( casePion ) & p->bbPion[BLANC]) == 0 ) {
				/* pion isole */
				scorePionBlanc -= 10;
			   }
			}
		}

		if ( pheMixte == NULL ) {	// Je pense que le test n'est pas necessaire.
			if ( (getPionPasseBlanc( casePion ) & p->bbPion[NOIR] ) == 0 ) {
				/* pion passe */
				/* plus le pion est proche de la 8eme rangee plus son score est important */
				scorePionMixte += scoreBasePionPasse + (( casePion >> 3 )* scoreCasePionPasse);
			}
		}

		videBit( bbPion , casePion );
	}

	
	hasheval_put( &hashPion[BLANC] , plateau_getClePion( p,BLANC ) ,  scorePionBlanc );


eval_score_noir:

	if ( (pheNoir != NULL && pheMixte != NULL) || p->bbPion[ NOIR ] == 0 ) {
		return scorePionBlanc - scorePionNoir + scorePionMixte ;
	}

			/*----------------------------------*/
			/*  Evalue le score des pions Noirs */
			/*----------------------------------*/

	bbPion = p->bbPion[ NOIR ];

	while ( bbPion ) {
		casePion = getFirstBit( bbPion );
		if ( pheNoir == NULL ) {
				/* recupere les pions situes sur la meme colonne */
			bbPionColonne = p->bbPion[ NOIR ] & 
					getDroiteContenantCases(  casePion , casePion + 8 );
	
			if ( getNbBits( bbPionColonne ) > 1 ) {
				/* Pion double */	
				scorePionNoir -= 10;
			}
			if ( ( getPionAcolyteNoir( casePion ) & p->bbPion[ NOIR ] ) == 0 ) {
				/* pion arriere */
				scorePionNoir -= 10;
	
			   if ( ( getPionAcolyteBlanc( casePion ) & p->bbPion[ NOIR ] ) == 0 ) {
				/* pion isole */
				scorePionNoir -= 10;
			   }
			}
	}

	if ( pheMixte == NULL ) {	// Je pense que le test n'est pas necessaire.
			if ( ( getPionPasseNoir( casePion ) & p->bbPion[ BLANC ] ) == 0 ) {
				/* pion passe */
				/* plus le pion est proche de la 8eme rangee plus son score est important */
				scorePionMixte -= scoreBasePionPasse + ( (63 - casePion) >> 3 ) * scoreCasePionPasse;
			}
		}

		videBit( bbPion , casePion );
	}

	hasheval_put( &hashPion[NOIR] , plateau_getClePion( p , NOIR ) ,  scorePionNoir );

	hasheval_put( &hashPionMixte , plateau_getClePionMixte( p )^getZobristPhase(phase) ,  
								scorePionMixte );


	return scorePionBlanc - scorePionNoir + scorePionMixte;
}
/*-----------------------------------------------------------------------------------------------*/

int eval_dame( Plateau *p ) {
  BitBoard pionBloqueur,emplacementDame;
  Case caseDame,casePion;
  int score ,malus;
  Couleur couleur;

  score = 0;

  if ( plateau_isOpening(p) ) malus = 10;
  if ( plateau_isEndGame(p) ) malus = 20; else malus = 15;

  	/* malus pour chaque pion bloquant une fou de meme couleur */

  for ( couleur = NOIR ; couleur <= BLANC ; couleur ++ ) {
    emplacementDame =  p->bbDame[ couleur ];

    while ( emplacementDame ) { 
      caseDame = getFirstBit( emplacementDame );
      pionBloqueur =( getDeplacementTourLigne( caseDame , plateau_getLigne( p , caseDame ) )  | 
		      getDeplacementTourColonne( caseDame , plateau_getColonne( p , caseDame ) ) |
		      getDeplacementFouA1H8( caseDame , plateau_getDiagonaleA1H8( p , caseDame ) )  |
		      getDeplacementFouA8H1( caseDame , plateau_getDiagonaleA8H1( p , caseDame ) )  ) &
	  	      p->bbPion[ couleur ];
	      
      while ( pionBloqueur ) {
	  casePion = getFirstBit( pionBloqueur );
 	  score += ( ( couleur == BLANC )? -malus : malus );		
	  videBit( pionBloqueur , casePion );
      }
    
      videBit( emplacementDame , caseDame );
    } 
  }

 return score;
}
/*-----------------------------------------------------------------------------------------------*/

int eval_fou( Plateau *p ) {
  BitBoard bb;
  Case ca;
  int score;


  score = 0;

  if ( plateau_isMiddleGame( p ) ) {

    bb = p->bbFou[ BLANC ];
    while ( bb ) {
	ca = getFirstBit( bb );
  	score += positionFouMilieu[ ca ];
	videBit( bb , ca );
    }

    bb = p->bbFou[ NOIR ];
    while ( bb ) {
	ca = getFirstBit( bb );
  	score -= positionFouMilieu[ symetrieAxiale[ca] ];
	videBit( bb , ca );
    }
  }

  return score;
}
/*-----------------------------------------------------------------------------------------------*/

int eval_tour( Plateau *p ) {
  int score ;
  BitBoard pionBloqueur,emplacementTour;
  Case caseTour,casePion;
  Couleur couleur;

  score = 0;

  if ( !plateau_isEndGame( p ) ) {
	  /* en debut et milieu
	   * Bonus pour les tours sur les colonnes ouvertes
	   * Bonus pour les tours placees sur la 7�me rang�e ( ou 2eme )
	   * Bonus pour les tours alignes en colonne */

	  /* TOUR sur une colonne semi ouverte */
  	for ( couleur = NOIR ; couleur <= BLANC ; couleur++ ) {
    		emplacementTour =  p->bbTour[ couleur ];

    		while ( emplacementTour ) { 
		      caseTour = getFirstBit( emplacementTour );
		      pionBloqueur = getDeplacementTourColonne( 
				           	caseTour , 
				           	plateau_getColonne( p , caseTour ) )  &
	  	      		     p->bbPion[ couleur ];
      		      
		      if ( !pionBloqueur ) {
			      (couleur==BLANC)?  (score += B_TOUR_COLONNE_OUVERTE):
				     		 (score -= B_TOUR_COLONNE_OUVERTE); 
		      }
		      videBit( emplacementTour , caseTour );
		}
	}

	TRACE_EVAL1("Score tour colonne semi ouverte %d\n", score );

  		/* BONUS TOUR SUR 7eme rangee pour les BLANC et 2eme pour les NOIRS */
   	score += getNbBits_ligne78( p->bbTour[ BLANC ] & LIGNE7 ) * B_TOUR_2_7_LIGNE;
   	score -= getNbBits_ligne12( p->bbTour[ NOIR ]  & LIGNE2 ) * B_TOUR_2_7_LIGNE;

   	TRACE_EVAL1("Score tour 2 et 7eme ligne %d\n",  
		   	(getNbBits( p->bbTour[ BLANC ] & LIGNE7 ) * B_TOUR_2_7_LIGNE)-
	      	   	(getNbBits( p->bbTour[ NOIR ]  & LIGNE2 ) * B_TOUR_2_7_LIGNE));

		/* BONUS POUR TOUR ALIGNEES EN COLONNE */
  	for ( couleur = NOIR ; couleur <= BLANC ; couleur++ ) {
    	   emplacementTour =  p->bbTour[ couleur ];
           caseTour = getFirstBit( emplacementTour );
           videBit( emplacementTour , caseTour );

	   if ( getDeplacementTourColonne( caseTour , plateau_getColonne(p,caseTour) ) & 
	        emplacementTour ) 
	        	(couleur==BLANC)?  (score += B_TOUR_ALIGNEES) : (score -= B_TOUR_ALIGNEES); 
	}
  }

  TRACE_EVAL1("Score tour %d\n",  score );

 return score;
}

/*-----------------------------------------------------------------------------------------------*/

int eval_cavalier( Plateau *p ) {
  BitBoard bb;
  Case ca;
  int score;


  score =0;

  if ( plateau_isMiddleGame( p ) ) {

    bb = p->bbCavalier[ BLANC ];
    while ( bb ) {
	ca = getFirstBit( bb );
  	score += positionCavalierMilieu[ ca ];
	videBit( bb , ca );
    }

    bb = p->bbCavalier[ NOIR ];
    while ( bb ) {
	ca = getFirstBit( bb );
  	score -= positionCavalierMilieu[ symetrieAxiale[ca] ];
	videBit( bb , ca );
    }
  }

  return score;

}


/*-----------------------------------------------------------------------------------------------*/

int evaluation( Plateau *p , Couleur c ,int alpha , int beta ) {
  int score;

	if ( plateau_isOpening(p) ) 
		phase = DEBUT; 
	else 
	if ( plateau_isEndGame(p) ) 
		phase = FIN; 
	else 
		phase = MILIEU; 
		

  score = eval_materiel(p);

  
		/* Bonus pour celui qui possede la paire de fous */
  if ( getNbBits( p->bbFou[ BLANC ] ) > 1 ) score += getValeurPaireFou;
  if ( getNbBits( p->bbFou[ NOIR ] ) > 1 ) score -= getValeurPaireFou;

  score += eval_pion( p ); 
  
  if ( plateau_isOpening( p ) ) score	+= eval_developpement(p);
  
  score +=   eval_controleCentre(p) + 
	     eval_securiteRoi( p ) +
	     eval_tour( p ) +
	     eval_cavalier( p ) +
	     eval_fou( p );

  return ((c==BLANC) ? score : -score);
}

