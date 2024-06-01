#include <stdio.h>
#include "coup.h"
#include "piece.h"
#include "case.h"
#include "boolean.h"
#include "bitboard.h"
#include "plateau.h"
#include "init.h"



void printCoup( Coup co , char *scoup ) {
  Piece pDep,pCap,pPro;
  Case ori,dest;
  Boolean echec,enpassant,capture,roque,promotion;

  scoup[0] = '\0';
  
  piece_setNom( pDep , coup_getPieceDeplace(co) );
  piece_setNom( pCap , coup_getPieceCapture(co) );
  piece_setNom( pPro , coup_getPiecePromotion(co) );

  ori = coup_getCaseOrigine( co );
  dest = coup_getCaseDestination( co );

  echec = coup_isEchec( co ) ? VRAI : FAUX;
  enpassant = coup_isEnPassant( co ) ? VRAI : FAUX;
  capture = coup_isCapture( co ) ? VRAI : FAUX;
  roque = coup_isRoque( co ) ? VRAI : FAUX;
  promotion = coup_isPromotion( co ) ? VRAI : FAUX;
/*
  if ( roque ) {
	if ( dest==G1 || dest ==G8 ) printf("O-O"); else printf("O-O-O");
	if (echec) printf("+\n"); else printf("\n");
	return;
  }

  if ( enpassant ) {
	printf("%sx%s" , case_getNom(ori) , case_getNom(dest) );
	if (echec) printf("+\n"); else printf("\n");
	return;
  }


  switch (piece_getNom(pDep)) {
	case PION:	break;
	case CAVALIER: 	printf("C"); break;
	case FOU :	printf("F"); break;
	case TOUR: 	printf("T"); break;
	case DAME:	printf("D"); break;
	case ROI:	printf("R"); break;
  }
*/
  strcat( scoup , case_getNom(ori) );
  //printf("%s",case_getNom(ori) );
 // if (capture) printf("x");  else printf("-");
  strcat( scoup , case_getNom(dest) );
  //printf("%s",case_getNom(dest) );

  if (promotion ) {
	  switch (piece_getNom(pPro)) {
		case TOUR : 	strcat(scoup,"r"); break;
		case CAVALIER: 	strcat(scoup,"n"); break;
		case FOU :	strcat(scoup,"b"); break;
		case DAME:	strcat(scoup,"q"); break;
		case ROI:	break;
		case PION:	break;
	  }
  }
  
 // if (echec) printf("+"); 
/*
  if (capture) {
	  printf(" capture ");
	  switch (piece_getNom(pCap)) {
		case TOUR :	printf("Tour"); break;
		case CAVALIER: 	printf("Cavallier"); break;
		case FOU :	printf("Fou"); break;
		case DAME:	printf("Dame"); break;
		case ROI:	printf("Roi"); break;
		case PION:	printf("Pion"); break;
	  }
  }
*/
  
}

/*----------------------------------------------------------------------------------------------*/

Coup chaineToCoup( Plateau *p , char *pco ) {
  Coup flag = 0,c=0;
  Case ori,dest;
  Piece pieceDep,pieceCap;
  short len;
  char sav;
  char co[10];

  strcpy( co , pco );


  len = strlen( pco );

  sav = co[2]; 
  co[2] = '\0';

  ori = case_getNomToCase( co );
  co[2] = sav;

  sav = co[4]; 
  co[4] = '\0';
  dest = case_getNomToCase( co+2 );
  co[4] = sav;
  
  pieceDep = plateau_getPiece( p , ori );
  pieceCap = plateau_getPiece( p , dest );
  /* indique la piece deplacee */
  switch ( piece_getNom( pieceDep ) ) {
	  case TOUR :
		  flag |= FLAG_TOUR;
		  break;
	  case CAVALIER :
		  flag |= FLAG_CAVALIER;
		  break;
	  case FOU :
		  flag |= FLAG_FOU;
		  break;
	  case DAME :
		  flag |= FLAG_DAME;
		  break;
	  case ROI :
		  flag |= FLAG_ROI;
		  break;
	  case PION :
		  flag |= FLAG_PION;
		  break;
		  
  }

  /* indique la piece capturee */
  flag |= getFlagCapture( piece_getNom( pieceCap ) );

  if ( dest == plateau_getCaseEp( p )  && piece_getNom(pieceDep) == PION ) {
	  /* prise en passant */
	  flag |= FLAG_PION_ENPASSANT;
  }


  if ( ori == E1 && dest == G1 && piece_getNom(pieceDep) == ROI ) {
	  /* Petit Roque BLANC */
	  flag |= FLAG_ROI_ROQUE;
  }

  if ( ori == E1 && dest == C1 && piece_getNom(pieceDep) == ROI ) {
	  /* Grand Roque BLANC*/
	  flag |= FLAG_ROI_ROQUE;
  }

  if ( ori == E8 && dest == G8 && piece_getNom(pieceDep) == ROI ) {
	  /* Petit Roque NOIR */
	  flag |= FLAG_ROI_ROQUE;
  }

  if ( ori == E8 && dest == C8 && piece_getNom(pieceDep) == ROI ) {
	  /* Grand Roque NOIR*/
	  flag |= FLAG_ROI_ROQUE;
  }

  if ( len > 4 ) {
	  /* c'est une promotion */
	if ( co[4] == 'r' ) flag |= FLAG_PION_PROMOTION_TOUR;
	if ( co[4] == 'n' ) flag |= FLAG_PION_PROMOTION_CAVALIER;
	if ( co[4] == 'b' ) flag |= FLAG_PION_PROMOTION_FOU;
	if ( co[4] == 'q' ) flag |= FLAG_PION_PROMOTION_DAME;
  }

  c = coup_newCoup( ori , dest , flag );

  return c;
  
}
