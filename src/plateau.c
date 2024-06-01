
#include <string.h>
#include "epd.h"
#include "piece.h"
#include "couleur.h"
#include "case.h"
#include "plateau.h"
#include "init.h"
#include "bitboard.h"
#include "boolean.h"
#include "trace.h"
#include "coup.h"
#include "annulecoup.h"
#include "valeur.h"
#include "hash.h"
#include "constante.h"

//Plateau *p;

/* TODO : Initialise le tableau aRoque dans la fonction setPosition */
/* TODO : Verifier le generateur de coup car modifie dans le cas ou l'on capture la tour adverse ce qui lui empeche de roque */
/* TODO : restaure dans annule coup la cle dans le cas du roque */

void plateau_setCle( Plateau *p ) {
	Case ca;
	Piece pi;
	
	p-> cle = 0;
	p-> clePion[ BLANC ] = 0;
	p-> clePion[ NOIR ] = 0;
	
	for ( ca=A1; ca<=H8 ; ca++ ) {
		pi = plateau_getPiece(p , ca);

		if ( piece_getNom(pi) == PASDEPIECE) 
			p->cle ^= getZobrist(PASDEPIECE , ca%2 , ca );
		else {
			p->cle ^= getZobrist( piece_getNom(pi) , piece_getCouleur(pi) , ca );

			if ( piece_getNom( pi ) == PION ) {
				p->clePion[ piece_getCouleur(pi) ]  ^= getZobrist( PION , piece_getCouleur(pi) , ca );
			}
		}
	}

	if ( plateau_getCoteCourant( p ) == BLANC ) 
		p->cle ^= zobristBlanc;
	
	p->cle ^= getZobristEnPassant( plateau_getCaseEp( p ) );
	
	if ( plateau_isPetitRoque( p , BLANC ) ) p->cle ^= zobristPetitRoque[BLANC];
	if ( plateau_isPetitRoque( p , NOIR ) )  p->cle ^= zobristPetitRoque[NOIR];	
	if ( plateau_isGrandRoque( p , BLANC ) ) p->cle ^= zobristGrandRoque[BLANC];
	if ( plateau_isGrandRoque( p , NOIR ) )  p->cle ^= zobristGrandRoque[NOIR];	
	

}


/*-------------------------------------------------------------------------------------------------*/
void plateau_joueCoup( Plateau *p , Coup co , AnnuleCoup *undo ) {
	Case ori,dest;
		
	ori = coup_getCaseOrigine( co );
	dest = coup_getCaseDestination( co );

	TRACE_JOUECOUP2("Joue %s%s\n", case_getNom(ori) , case_getNom(dest) );

	//p->nbCoup++;

	annulecoup_setPetitRoque( undo , plateau_isPetitRoque( p , plateau_getCoteCourant(p) ),
		       	  	  plateau_getCoteCourant(p));
	annulecoup_setGrandRoque( undo , plateau_isGrandRoque( p , plateau_getCoteCourant(p) ) ,
		      		  plateau_getCoteCourant(p));
	annulecoup_setPetitRoque( undo , plateau_isPetitRoque( p , 
				  couleur_oppose(plateau_getCoteCourant(p)) ),
		       	  	  couleur_oppose(plateau_getCoteCourant(p)));
	annulecoup_setGrandRoque( undo , plateau_isGrandRoque( p , 
				  couleur_oppose(plateau_getCoteCourant(p)) ) ,
		      		  couleur_oppose(plateau_getCoteCourant(p)));
	annulecoup_setEp( undo , plateau_getCaseEp(p) ); 

	TRACE_JOUECOUP1("Supprime la piece en %s\n", case_getNom(ori) );

	plateau_supprimerPieceEchiquier( p  , ori  , plateau_getCoteCourant( p ) );

			/* MAJ de la cle de pion : Supprime le pion deplace */

	if ( coup_getPieceDeplace( co ) == PION  ) 
		p->clePion[ plateau_getCoteCourant(p) ] ^= getZobrist( PION , plateau_getCoteCourant(p)  , ori );


	if ( coup_isEnPassant( co ) ) {
		/* TODO PAS TERRIBLE LA MANIERE DE RECUPERER LE PION A CAPTURER */
		TRACE_JOUECOUP1("Prise en passant.Supprime le pion en %s\n",
			       	case_getNom(plateau_getCaseEp(p) + ( plateau_getCaseEp(p)>39 ? -8 : 8)) );

		plateau_supprimerPieceEchiquier( p ,  
						plateau_getCaseEp(p) +
					       	( plateau_getCaseEp(p)>39 ? -8 : 8)  ,
					       	  couleur_oppose( plateau_getCoteCourant(p) ) 
						);

		p->clePion[ couleur_oppose( plateau_getCoteCourant(p) ) ] ^= 
			getZobrist( PION , 
				    couleur_oppose( plateau_getCoteCourant(p) )  , 
				    plateau_getCaseEp(p) + ( plateau_getCaseEp(p)>39 ? -8 : 8)  );

		p->materiel[ couleur_oppose( plateau_getCoteCourant(p) ) ] -= getValeurPiece( PION ); 
	}
	else {
	  if ( coup_isCapture( co ) ) {	
	    TRACE_JOUECOUP1("Supprime la piece capturee en %s\n", case_getNom(dest) );

	    plateau_supprimerPieceEchiquier( p , dest , 
					     couleur_oppose( plateau_getCoteCourant(p) ));		        
	    

			/* MAJ de la cle de pion : Supprime le pion capture */

	    if ( coup_getPieceCapture( co ) == PION  ) 
		p->clePion[ couleur_oppose( plateau_getCoteCourant(p) ) ] ^= 
			getZobrist( PION , couleur_oppose( plateau_getCoteCourant(p) ) , dest );

	    p->materiel[ couleur_oppose( plateau_getCoteCourant(p) ) ] -= getValeurPiece( coup_getPieceCapture( co ) ); 

	    /* Interdire le roque si la piece capture est une tour */
	    if ( coup_getPieceCapture( co ) == TOUR ) {
 		if ( ((plateau_getCoteCourant( p )==BLANC) ? (dest==H8) : (dest == H1)) ) {
		   if ( plateau_isPetitRoque( p , couleur_oppose(plateau_getCoteCourant( p )) ) )  {
		      plateau_setPetitRoque( p , couleur_oppose(plateau_getCoteCourant( p )) , FAUX );
		      p->cle ^= zobristPetitRoque[ couleur_oppose(plateau_getCoteCourant( p ))  ] ;
		   }
		}
		else
			if ( ((plateau_getCoteCourant( p )==BLANC) ? (dest==A8) : (dest == A1)) ) {
		   if ( plateau_isGrandRoque( p , couleur_oppose(plateau_getCoteCourant( p )) ) )  {
		      plateau_setGrandRoque( p , couleur_oppose( plateau_getCoteCourant( p ) ), FAUX );
		      p->cle ^= zobristGrandRoque[ couleur_oppose(plateau_getCoteCourant( p ))  ] ;
		   }
		}
	    } 
	  }
	}

	if ( coup_isPromotion( co ) ) {
	    TRACE_JOUECOUP1("Ajoute la piece promu en %s\n", case_getNom(dest) );

	    plateau_ajouterPieceEchiquier( p , coup_getPiecePromotion(co) , dest ,
			       		   plateau_getCoteCourant( p ));
	    p->materiel[ plateau_getCoteCourant(p) ] += getValeurPiece( coup_getPiecePromotion(co) ) -
		    					getValeurPiece( PION ); 
	}
	else {
	    TRACE_JOUECOUP2("Ajoute la piece %d deplace en %s\n",coup_getPieceDeplace(co), case_getNom(dest) );

	    plateau_ajouterPieceEchiquier( p , coup_getPieceDeplace(co) , dest , plateau_getCoteCourant( p ));

			/* MAJ de la cle de pion : Supprime le pion deplace */

	    if ( coup_getPieceDeplace( co ) == PION  ) 
		p->clePion[ plateau_getCoteCourant(p) ] ^= getZobrist( PION , plateau_getCoteCourant(p)  , dest );

	}

	if ( coup_isRoque( co ) ) {
		p->aRoque[ plateau_getCoteCourant( p ) ] = VRAI;
		if (dest==G1)  {
	          TRACE_JOUECOUP("Petit roque blanc\n" );
		  plateau_supprimerPieceEchiquier( p  , H1  , plateau_getCoteCourant( p ) );
		  plateau_ajouterPieceEchiquier( p , TOUR , F1 , 
			       			plateau_getCoteCourant( p ));
		}
		if (dest==C1)  {
	          TRACE_JOUECOUP("Grand roque blanc\n" );
		  plateau_supprimerPieceEchiquier( p  , A1  , plateau_getCoteCourant( p ) );
		  plateau_ajouterPieceEchiquier( p , TOUR , D1 , 
			       			plateau_getCoteCourant( p ));
		}

		if (dest==G8)  {
	          TRACE_JOUECOUP("Petit roque noir\n" );
		  plateau_supprimerPieceEchiquier( p  , H8  , plateau_getCoteCourant( p ) );
		  plateau_ajouterPieceEchiquier( p , TOUR , F8 , 
			       			plateau_getCoteCourant( p ));
		}
		if (dest==C8)  {
	          TRACE_JOUECOUP("Grand roque noir\n" );
		  plateau_supprimerPieceEchiquier( p  , A8  , plateau_getCoteCourant( p ) );
		  plateau_ajouterPieceEchiquier( p , TOUR , D8 , 
			       			plateau_getCoteCourant( p ));
		}

		if ( plateau_isPetitRoque( p , plateau_getCoteCourant( p )) ) 
			p->cle ^= zobristPetitRoque[ plateau_getCoteCourant( p ) ] ;

		if ( plateau_isGrandRoque( p , plateau_getCoteCourant( p )) ) 
			p->cle ^= zobristGrandRoque[ plateau_getCoteCourant( p ) ] ;

		plateau_setPetitRoque( p , plateau_getCoteCourant( p ) , FAUX );
		plateau_setGrandRoque( p , plateau_getCoteCourant( p ) , FAUX );

	}
	
	if ( plateau_getCaseEp( p ) != CASEVIDE ) {
  		p->cle ^= getZobristEnPassant( plateau_getCaseEp( p ) );
		plateau_setCaseEp( p , CASEVIDE );
	}

	if  ( coup_getPieceDeplace( co ) == PION ) {
	  TRACE_JOUECOUP("Test d'une eventuelle case en passant\n" );

	  if   ( ( ori>=A2 && ori<=H2) && ( dest>=A4 && dest<=H4) ) {
		  plateau_setCaseEp( p , dest-8 );
		  p->cle ^= getZobristEnPassant( plateau_getCaseEp( p ) );
	  }
	  if   ( ( ori>=A7 && ori<=H7) && ( dest>=A5 && dest<=H5) ) {
		  plateau_setCaseEp( p , dest+8 );
		  p->cle ^= getZobristEnPassant( plateau_getCaseEp( p ) );
	  }
	}


	/* Test si le roque est toujours possible : si le roi ou la tour a bouge desactiver le roque */
	if ( plateau_isPetitRoque( p , plateau_getCoteCourant(p) ) ) {
	  TRACE_JOUECOUP("Petit toujours possible ?\n" );
		if ( coup_getPieceDeplace(co)==ROI || 
		     (coup_getPieceDeplace(co)==TOUR && (ori==H1 || ori==H8 ) )) {
			plateau_setPetitRoque( p , plateau_getCoteCourant( p ) , FAUX );
			p->cle ^= zobristPetitRoque[ plateau_getCoteCourant( p ) ] ;
		}
	}
	
	if ( plateau_isGrandRoque( p , plateau_getCoteCourant(p) ) ) {
	  TRACE_JOUECOUP("Grand toujours possible ?\n" );
		if ( coup_getPieceDeplace(co)==ROI || 
		     (coup_getPieceDeplace(co)==TOUR && (ori==A1 || ori==A8 ) )) {
			plateau_setGrandRoque( p , plateau_getCoteCourant( p ) , FAUX );
			p->cle ^= zobristGrandRoque[ plateau_getCoteCourant( p ) ] ;
		}
	}

	plateau_setCoteCourant( p , couleur_oppose( plateau_getCoteCourant(p) ) );
	p->cle ^= zobristBlanc;

	hashRepet_put( p->cle );
//	debugRepet_put( p->cle );

	/* Memorise l'ensemble des coups qui ont été jouées sur le plateau */
	plateau_addCoupJoue( p , co , *undo );
}
/*-------------------------------------------------------------------------------------------------*/
void plateau_annuleCoup( Plateau *p , Coup co , AnnuleCoup *undo ){
  	Case ori,dest;

	  TRACE_JOUECOUP("fonction annuleCoup 1\n" );
	  TRACE_JOUECOUP1("fonction annuleCoup 1 : %d\n" ,plateau_getCoteCourant(p) );
		
	hashRepet_remove( p->cle );
//    debugRepet_remove( p->cle );

	plateau_setCoteCourant( p , couleur_oppose( plateau_getCoteCourant(p) ) );
	  TRACE_JOUECOUP("fonction annuleCoup 1-2\n" );

	p->cle ^= zobristBlanc;
	  TRACE_JOUECOUP("fonction annuleCoup 2\n" );

	ori = coup_getCaseOrigine( co );
	dest = coup_getCaseDestination( co );

	/* Supprime le coup de la liste des coups joues */
	plateau_supprimeCoupJoue( p );

	  TRACE_JOUECOUP("fonction annuleCoup 3\n" );

	plateau_supprimerPieceEchiquier( p  , dest  , plateau_getCoteCourant( p ) );
	  TRACE_JOUECOUP("fonction annuleCoup 4\n" );

	plateau_ajouterPieceEchiquier( p  , coup_getPieceDeplace(co) , ori , plateau_getCoteCourant( p ) );

			/* MAJ de la cle de pion : Supprime le pion deplace */

	if ( coup_getPieceDeplace( co ) == PION  ) 
		p->clePion[ plateau_getCoteCourant(p) ] ^= getZobrist( PION , plateau_getCoteCourant(p)  , ori );

	  TRACE_JOUECOUP("fonction annuleCoup 5\n" );

		/* Restaure la cle de hachage pour le roque et la case en passant */
	if ( plateau_getCaseEp( p ) != CASEVIDE ) {
	     p->cle ^= getZobristEnPassant( plateau_getCaseEp(p) );
	}
	  TRACE_JOUECOUP("fonction annuleCoup 6\n" );

	if ( annulecoup_getEp( undo ) != CASEVIDE ) {
	     p->cle ^= getZobristEnPassant( annulecoup_getEp( undo ) );
	}
	
	  TRACE_JOUECOUP("fonction annuleCoup 7\n" );

		/* Si changement de l'etat du roque a la suite du coup il faut appliquer le XOR */
	if ( annulecoup_isPetitRoque(undo, BLANC ) != plateau_isPetitRoque( p , BLANC ) )
	     p->cle ^= zobristPetitRoque[BLANC];

	if ( annulecoup_isPetitRoque(undo, NOIR ) != plateau_isPetitRoque( p , NOIR ) )
	     p->cle ^= zobristPetitRoque[NOIR];

	if ( annulecoup_isGrandRoque(undo, BLANC ) != plateau_isGrandRoque( p , BLANC ) )
	     p->cle ^= zobristGrandRoque[BLANC];

	if ( annulecoup_isGrandRoque(undo, NOIR ) != plateau_isGrandRoque( p , NOIR ) )
	     p->cle ^= zobristGrandRoque[NOIR];
	  TRACE_JOUECOUP("fonction annuleCoup 8\n" );

		/* restaure la case en passant */
	plateau_setCaseEp( p , annulecoup_getEp( undo ) );

		/* Restaure l'etat du petit et grand roque du joueur courant */
	plateau_setPetitRoque( p , plateau_getCoteCourant( p ) , 
			       annulecoup_isPetitRoque(undo,plateau_getCoteCourant( p )));
	plateau_setGrandRoque( p , plateau_getCoteCourant( p ) , 
			       annulecoup_isGrandRoque(undo,plateau_getCoteCourant( p )) );

		/* Restaure l'etat du petit et grand roque du joueur adverse */
	plateau_setPetitRoque( p , couleur_oppose(plateau_getCoteCourant( p )) , 
			       annulecoup_isPetitRoque(undo,couleur_oppose(plateau_getCoteCourant( p ))));
	plateau_setGrandRoque( p , couleur_oppose(plateau_getCoteCourant( p )) , 
			       annulecoup_isGrandRoque(undo,couleur_oppose(plateau_getCoteCourant( p ))) );

	  TRACE_JOUECOUP("fonction annuleCoup 9\n" );

	if ( coup_isEnPassant( co ) ) {
		/* TODO PAS TERRIBLE LA MANIERE DE RECUPERER LE PION A CAPTURER */
		plateau_ajouterPieceEchiquier( p ,  PION ,
						plateau_getCaseEp(p) +
					       	( plateau_getCaseEp(p)>39 ? -8 : 8)  ,
					       	  couleur_oppose( plateau_getCoteCourant(p) ) ); 

		p->clePion[ couleur_oppose( plateau_getCoteCourant(p) ) ] ^= 
			getZobrist( PION , 
				    couleur_oppose( plateau_getCoteCourant(p) )  , 
				    plateau_getCaseEp(p) + ( plateau_getCaseEp(p)>39 ? -8 : 8)  );

						
 	        p->materiel[ couleur_oppose( plateau_getCoteCourant(p) ) ] += getValeurPiece( PION ); 
	}
	else {
	  if ( coup_isCapture( co ) ) {	
		plateau_ajouterPieceEchiquier( p ,coup_getPieceCapture(co) , dest , 
					       couleur_oppose( plateau_getCoteCourant(p) ));		

			/* MAJ de la cle de pion : Supprime le pion capture */

	    if ( coup_getPieceCapture( co ) == PION  ) 
		p->clePion[ couleur_oppose( plateau_getCoteCourant(p) ) ] ^= 
			getZobrist( PION , couleur_oppose( plateau_getCoteCourant(p) ) , dest );

	   	 p->materiel[ couleur_oppose( plateau_getCoteCourant(p) ) ] += getValeurPiece( coup_getPieceCapture( co ) ); 
	  }
	}

	TRACE_JOUECOUP("fonction annuleCoup 10\n" );

	if ( coup_isPromotion( co ) ) {
	    p->materiel[ plateau_getCoteCourant(p) ] -= getValeurPiece( coup_getPiecePromotion(co) ) -
		    					getValeurPiece( PION ); 
	}
	else {
			/* MAJ de la cle de pion : Supprime le pion deplace */

	    if ( coup_getPieceDeplace( co ) == PION  ) 
		p->clePion[ plateau_getCoteCourant(p) ] ^= getZobrist( PION , plateau_getCoteCourant(p)  , dest );

	}

        TRACE_JOUECOUP("fonction annuleCoup 11\n" );

	if ( coup_isRoque( co ) ) {
		p->aRoque[ plateau_getCoteCourant( p ) ] = FAUX;
		if (dest==G1)  {
		  plateau_supprimerPieceEchiquier( p  , F1  , plateau_getCoteCourant( p ) );
		  plateau_ajouterPieceEchiquier( p , TOUR , H1 , 
			       			plateau_getCoteCourant( p ));
		}
		if (dest==C1)  {
		  plateau_supprimerPieceEchiquier( p  , D1  , plateau_getCoteCourant( p ) );
		  plateau_ajouterPieceEchiquier( p , TOUR , A1 , 
			       			plateau_getCoteCourant( p ));
		}

		if (dest==G8)  {
		  plateau_supprimerPieceEchiquier( p  , F8  , plateau_getCoteCourant( p ) );
		  plateau_ajouterPieceEchiquier( p , TOUR , H8 , 
			       			plateau_getCoteCourant( p ));
		}
		if (dest==C8)  {
		  plateau_supprimerPieceEchiquier( p  , D8  , plateau_getCoteCourant( p ) );
		  plateau_ajouterPieceEchiquier( p , TOUR , A8 , 
			       			plateau_getCoteCourant( p ));
		}
	}


}
/*-------------------------------------------------------------------------------------------------*/

/* Retourne VRAI si la case ca est attaque par une piece de couleur co */
Boolean plateau_caseAttaque( Plateau *p , Case ca , Couleur co) {

	if  ( getDeplacementCavalier( ca ) & p->bbCavalier[ co ] )  return VRAI;

	if  (( getDeplacementFouA1H8( ca , plateau_getDiagonaleA1H8( p , ca ) )  |
	      getDeplacementFouA8H1( ca , plateau_getDiagonaleA8H1( p , ca ) )  )
	     &
		(p->bbFou[ co ] | p->bbDame[ co ] )   ) return VRAI;

	if ( ( getDeplacementTourLigne( ca , plateau_getLigne( p , ca ) ) |
		getDeplacementTourColonne( ca , plateau_getColonne( p , ca ) ) ) 
		&
		(p->bbTour[ co ] | p->bbDame[ co ] )   ) return VRAI;		

	if  ( getDeplacementRoi( ca ) & p->bbRoi[ co ] )  return VRAI;
	if  ( getDeplacementPionCapture( ca , couleur_oppose(co) ) & p->bbPion[ co ] ) 
		return VRAI;

	return FAUX;
}

/*-------------------------------------------------------------------------------------------------*/
/* Retourne le nombre de pieces de couleur co qui attaque la case ca */
short plateau_nbPieceAttaque( Plateau *p , Case ca , Couleur co) {
	short nb_piece =0;
	BitBoard bbPiece;
	
	bbPiece = getDeplacementCavalier( ca ) & p->bbCavalier[ co ];
	if (bbPiece) nb_piece += getNbBits(bbPiece);


	bbPiece = ( getDeplacementFouA1H8( ca , plateau_getDiagonaleA1H8( p , ca ) )  |
	      		getDeplacementFouA8H1( ca , plateau_getDiagonaleA8H1( p , ca ) )  )
	     		&
			   (p->bbFou[ co ] | p->bbDame[ co ] );   
	if (bbPiece) nb_piece += getNbBits(bbPiece);		


	bbPiece =  ( getDeplacementTourLigne( ca , plateau_getLigne( p , ca ) ) |
				 getDeplacementTourColonne( ca , plateau_getColonne( p , ca ) ) ) 
				&
				(p->bbTour[ co ] | p->bbDame[ co ] );
	if (bbPiece) nb_piece += getNbBits(bbPiece);
				
				
	bbPiece = getDeplacementRoi( ca ) & p->bbRoi[ co ];
	if (bbPiece) nb_piece += getNbBits(bbPiece);
	
	
	bbPiece = getDeplacementPionCapture( ca , couleur_oppose(co) ) & p->bbPion[ co ] ;
	if (bbPiece) nb_piece += getNbBits(bbPiece);	

	return nb_piece;
}


/*-------------------------------------------------------------------------------------------------*/
/* Retourne un BitBoard contenant l'emplacement des pieces clouee par des piece de couleur co.Le clouage  
   est du  a lapiece placée en case CA. Si la piece en ca est le roi c'est un clouage absolue.
*/

BitBoard plateau_pieceClouee( Plateau *p ,Case ca , Couleur co ) {
	BitBoard b = 0;
	BitBoard bbPieceDiag,bbPieceColLig,bbEntre;
	Case c,c2;
	
	bbPieceDiag 	 = p->bbFou[co] | p->bbDame[co];
	bbPieceColLig 	 = p->bbTour[co] | p->bbDame[co];
	
	while (bbPieceDiag) {
		c = getFirstBit( bbPieceDiag );
		bbEntre = plateau_getAllPiece(p) & getCasesEntreDeuxCasesDiag( c , ca );
		if (bbEntre) {
			c2 = getFirstBit( bbEntre );
			videBit( bbEntre , c2 );
			if ( !bbEntre ) {	/* il existe q'une piece entre les cases c et ca */
				b |= p->bbAllPiece[couleur_oppose(co)] & getCasesEntreDeuxCasesDiag( c , ca );
			}
		}
		videBit( bbPieceDiag , c );
	}
	
	while (bbPieceColLig) {
		c = getFirstBit( bbPieceColLig );
		bbEntre = plateau_getAllPiece(p) & getCasesEntreDeuxCasesColLig( c , ca );
		if (bbEntre) {
			c2 = getFirstBit( bbEntre );
			videBit( bbEntre , c2 );
			if ( !bbEntre ) {	/* il existe q'une piece entre les cases c et ca */
				b |= p->bbAllPiece[couleur_oppose(co)] & getCasesEntreDeuxCasesColLig( c , ca );
			}
		}
		videBit( bbPieceColLig , c );
	}

	return b;
}

/*-------------------------------------------------------------------------------------------------*/

Boolean plateau_testCoup( Plateau *p ,Case ori, Case dest , Flag flag ,Couleur co) {
	Boolean isEchec;
	Piece pieceOri,pieceDest; 
	Case empRoi = plateau_getEmplacementRoi( p, co);

	pieceOri 	= plateau_getPiece( p , ori );
	pieceDest = plateau_getPiece( p , dest );	

	if ( piece_getNom( pieceOri ) == ROI ) empRoi = dest;
			
	plateau_supprimerPieceEchiquier( p  , ori  , co);
	

	
	if ( piece_getNom(pieceDest) != PASDEPIECE ) {	/* c'est une capture */
		plateau_supprimerPieceEchiquier( p , dest , couleur_oppose(co) );		
	}

/*	Je pense que cela ne change rien à la mise en echec le type de piece promu 
	if ( coup_isPromotion( flag ) ) 
		piece_setNom( pieceDest , coup_getPiecePromotion( flag ) ); */
	

	if ( flag == FLAG_PION_ENPASSANT ) {
		plateau_supprimerPieceEchiquier( p ,  plateau_getCaseEp(p) + ( plateau_getCaseEp(p)>39 ? -8 : 8)  , couleur_oppose(co) );
	}

	plateau_ajouterPieceEchiquier( p , piece_getNom( pieceOri ) , dest , co);

//	TRACE_GENCOUP("JOUE LE COUP\n");   
/*	printBitBoard( p->bbTour[BLANC] |  p->bbCavalier[BLANC] | p->bbFou[BLANC] | p->bbDame[BLANC] | p->bbRoi[BLANC] |p->bbPion[BLANC] |
					  p->bbTour[NOIR] |  p->bbCavalier[NOIR] | p->bbFou[NOIR] | p->bbDame[NOIR] | p->bbRoi[NOIR] | p->bbPion[NOIR]); */


	isEchec =  plateau_caseAttaque( p, empRoi , couleur_oppose(co)  );
		
	plateau_ajouterPieceEchiquier( p ,  piece_getNom(pieceOri) , ori , co );
	plateau_supprimerPieceEchiquier( p , dest , co);	
	
	if ( piece_getNom(pieceDest) != PASDEPIECE ) {	/* c'est une capture */
		plateau_ajouterPieceEchiquier( p , piece_getNom(pieceDest) , dest , couleur_oppose(co) );		
	}

	if ( flag == FLAG_PION_ENPASSANT ) {
		plateau_ajouterPieceEchiquier( p ,  PION , plateau_getCaseEp(p) + ( plateau_getCaseEp(p)>39 ? -8 : 8)  , couleur_oppose(co) );
	}

//	TRACE_GENCOUP("ANNULE LE COUP\n");   
 //    printBitBoard( p->bbAllPiece[BLANC] |  p->bbAllPiece[NOIR] );
		
//	if (isEchec) TRACE_GENCOUP("***ECHEC !!!!\n");
		
	return isEchec;	

}

/*---------------------------------------------------------------------------------------------------*/
void plateau_setPosition( Plateau *pl ,Epd e ) {
	short c,l;
	Case ca;
	Piece p;
	Couleur co;
	
	for( co=NOIR; co<=BLANC ; co++ ) {
	   pl->bbPion[ co ] = 0;
	   pl->bbTour[ co ] = 0;
 	   pl->bbCavalier[ co ] = 0;
	   pl->bbFou[ co ] = 0;
	   pl->bbDame[ co ] = 0;
	   pl->bbRoi[ co ] = 0;
	   pl->bbAllPiece[ co ] = 0;			
	}
	
	pl->bbAllPiece90 = 0;
	pl->bbAllPiece45 = 0;	
	pl->bbAllPiece315 = 0;	

	pl->materiel[BLANC] = 0;
	pl->materiel[NOIR] = 0;

	/* La fonction d'evaluatiopn ne doit pas se baser dessus
	 * car pas fiable si la position depart != position initiale du jeu */
	pl->aRoque[ BLANC ] = VRAI;
	pl->aRoque[ NOIR ] = VRAI;


	for( c=0 ; c < 8 ; c++ ) {
	  for( l=0 ; l < 8 ; l++ ) {
		ca = case_getCoordonneeToCase( c , l );
		p   = epd_getPiece( e , c , l );
		co = piece_getCouleur( p );
		
		if ( piece_getNom( p ) != PASDEPIECE )  {
		   pl->bbAllPiece[ co ] |= getCaseToBitBoard( ca );
		   pl->materiel[ co ] += getValeurPiece( piece_getNom( p ) );
		}
		
		pl->listePiece[ ca ] = p;
		
		switch ( piece_getNom( p ) ) {
			case PION :  pl->bbPion[ co ] |= getCaseToBitBoard( ca );
					break;
			case TOUR : pl->bbTour[ co ] |= getCaseToBitBoard( ca );
					break;
			case CAVALIER : pl->bbCavalier[ co ] |= getCaseToBitBoard( ca );
					break;
			case FOU : pl->bbFou[ co ] |= getCaseToBitBoard( ca );
					break;				
			case DAME : pl->bbDame[ co ] |= getCaseToBitBoard( ca );
					break;				
			case ROI :	pl->bbRoi[ co ] |= getCaseToBitBoard( ca );
					pl->emplacementRoi[ co ] = ca;
		}
	  }
	}
	
	pl->bbAllPiece90 = rotation90BitBoard( pl->bbAllPiece[ BLANC ]  | pl->bbAllPiece[ NOIR ] );
	pl->bbAllPiece45 = rotation45BitBoard( pl->bbAllPiece[ BLANC ]  | pl->bbAllPiece[ NOIR ] );
	pl->bbAllPiece315 = rotation315BitBoard( pl->bbAllPiece[ BLANC ]  | pl->bbAllPiece[ NOIR ] );	
	

	pl->cote = epd_getCoteCourant( e );
	pl->ep    = epd_getEnPassant( e );		
	pl->petitRoque[ BLANC ] = epd_isPetitRoque( e , BLANC );
	pl->petitRoque[ NOIR ] = epd_isPetitRoque( e , NOIR ); 
	pl->grandRoque[ BLANC ]= epd_isGrandRoque( e , BLANC );
	pl->grandRoque[ NOIR ]= epd_isGrandRoque( e , NOIR ); 		 

	pl->nbCoup = 0;
	
	plateau_setCle(pl);
	
}
/*---------------------------------------------------------------------------------------------------*/
void printPlateau( Plateau *p ) {
 int c,l;  
 Case ca;
 int delta;
       printBitBoard( p->bbAllPiece[BLANC] );

	printBitBoard( p->bbAllPiece[NOIR] );
		
	printBitBoard( p->bbAllPiece90 );	
	
	printBitBoard( p->bbAllPiece45 );		
	
	printBitBoard( p->bbAllPiece315 );		
	
	printf("Tour blanche\n");
	printBitBoard( p->bbTour[BLANC] );		
	
	printf("Tour noir\n");
	printBitBoard( p->bbTour[NOIR] );		
	
	printf("cavalier blanc\n");
	printBitBoard( p->bbCavalier[BLANC] );		
	
	printf("cavalier noir\n");
	printBitBoard( p->bbCavalier[NOIR] );		
	
	printf("Fou blanc\n");
	printBitBoard( p->bbFou[BLANC] );		
	
	printf("Fou noir\n");
	printBitBoard( p->bbFou[NOIR] );		
	
	printf("Dame blanche\n");
	printBitBoard( p->bbDame[BLANC] );		
	
	printf("Dame noir\n");
	printBitBoard( p->bbDame[NOIR] );		
	
	printf("Roi blanc\n");
	printBitBoard( p->bbRoi[BLANC] );		
	
	printf("Roi noir\n");
	printBitBoard( p->bbRoi[NOIR] );		
	
	printf("Pion blanc\n");
	printBitBoard( p->bbPion[BLANC] );		
	
	printf("Pion noir\n");
	printBitBoard( p->bbPion[NOIR] );		
	printf("%s joue\n", (p->cote == NOIR) ? "NOIR":"BLANC");
	printf("ROQUE BLANC petitRoque %d grandRoque %d\n",p->petitRoque[BLANC],p->grandRoque[BLANC]);
	printf("ROQUE NOIR petitRoque %d grandRoque %d\n",p->petitRoque[NOIR],p->grandRoque[NOIR]);
	printf("case en passant %s\n",case_getNom(p->ep));

	printf("Roi BLANC en : %s\n",case_getNom(p->emplacementRoi[BLANC]) );
	printf("Roi NOIR  en : %s\n",case_getNom(p->emplacementRoi[NOIR]) );

	for(l=7;l>=0;l--) {
		for(c=0;c<8;c++) {
			ca= case_getCoordonneeToCase(c,l);
			if (piece_getCouleur( plateau_getPiece(p,ca) )==NOIR ) delta=0;else delta='A'-'a';
			switch ( piece_getNom(plateau_getPiece(p,ca)) ) {
				case PION:	printf("%c ",'p'+delta);break;
				case CAVALIER: 	printf("%c ",'c'+delta); break;
				case FOU :	printf("%c ",'f'+delta); break;
				case TOUR: 	printf("%c ",'t'+delta); break;
				case DAME:	printf("%c ",'d'+delta); break;
				case ROI:	printf("%c ",'r'+delta); break;
				default :	printf("  ");
			}
		}
		printf("\n");
	}
}

Boolean comparePlateau( Plateau *p1, Plateau *p2 ) {
	Case ca;

	if (p1->cle != p2->cle) { printf("CLE : NOK\n"); return 1;}

	if (p1->cote != p2->cote) { printf("Couleur : NOK\n"); return 1;}
	if (p1->ep != p2->ep) {printf("Case en passant : NOK\n"); return 1;}
	
	if (p1->petitRoque[BLANC] != p2->petitRoque[BLANC]) {printf("Petit roque BLANC : NOK\n"); return 1;}
	if (p1->petitRoque[NOIR] != p2->petitRoque[NOIR]) {printf("Petit roque NOIR : NOK\n"); return 1;}
	if (p1->grandRoque[BLANC] != p2->grandRoque[BLANC]) {printf("Grand roque BLANC : NOK\n"); return 1;}
	if (p1->grandRoque[NOIR] != p2->grandRoque[NOIR]) {printf("Grand roque NOIR : NOK\n"); return 1;}

	if (p1->bbPion[BLANC] != p2->bbPion[BLANC]) {printf("bbPion BLANC : NOK\n"); return 1;}
	if (p1->bbPion[NOIR] != p2->bbPion[NOIR]) {printf("bbPion  NOIR : NOK\n"); return 1;}

	if (p1->bbTour[BLANC] != p2->bbTour[BLANC]) {printf("bbTour BLANC : NOK\n"); return 1;}
	if (p1->bbTour[NOIR] != p2->bbTour[NOIR]) {printf("bbTour  NOIR : NOK\n"); return 1;}

	if (p1->bbCavalier[BLANC] != p2->bbCavalier[BLANC]) {printf("bbCavalier BLANC : NOK\n"); return 1;}
	if (p1->bbCavalier[NOIR] != p2->bbCavalier[NOIR]) {printf("bbCavalier  NOIR : NOK\n"); return 1;}
	if (p1->bbFou[BLANC] != p2->bbFou[BLANC]) {printf("bbFou BLANC : NOK\n"); return 1;}
	if (p1->bbFou[NOIR] != p2->bbFou[NOIR]) {printf("bbFou  NOIR : NOK\n"); return 1;}

	if (p1->bbDame[BLANC] != p2->bbDame[BLANC]) {printf("bbDame BLANC : NOK\n"); return 1;}
	if (p1->bbDame[NOIR] != p2->bbDame[NOIR]) {printf("bbDame  NOIR : NOK\n"); return 1;}

	if (p1->bbRoi[BLANC] != p2->bbRoi[BLANC]) {printf("bbRoi BLANC : NOK\n"); return 1;}
	if (p1->bbRoi[NOIR] != p2->bbRoi[NOIR]) {printf("bbRoi  NOIR : NOK\n"); return 1;}

	if (p1->bbAllPiece[BLANC] != p2->bbAllPiece[BLANC]) { printf("bbAllPiece BLANC : NOK\n"); return 1;}
	if (p1->bbAllPiece[NOIR] != p2->bbAllPiece[NOIR])  { printf("bbAllPiece  NOIR : NOK\n"); return 1;}

	if (p1->bbAllPiece90 != p2->bbAllPiece90) { printf("bbAllPiece90 : NOK\n"); return 1;}
	if (p1->bbAllPiece45 != p2->bbAllPiece45) { printf("bbAllPiece45 : NOK\n"); return 1;}
	if (p1->bbAllPiece315 != p2->bbAllPiece315) { printf("bbAllPiece315 : NOK\n"); return 1;}

	if (p1->emplacementRoi[BLANC] != p2->emplacementRoi[BLANC]) { printf("emplacementRoi BLANC : NOK\n"); return 1;}
	if (p1->emplacementRoi[NOIR] != p2->emplacementRoi[NOIR]) { printf("emplacementRoi  NOIR : NOK\n"); return 1;}

	for(ca=A1;ca>=H8;ca++) {
		if ((piece_getNom(p1->listePiece[ca]) != piece_getNom(p2->listePiece[ca])) ||
		    (piece_getCouleur(p1->listePiece[ca]) != piece_getCouleur(p2->listePiece[ca])) 
	   	   ) 
		{printf("ListePiece differentes en %d\n",ca); return 1;}
	}

	return 0;
}

void plateau_getFen( Plateau *p, char *fen) {
  int c,l,nbCaseVide,delta;
  Case ca;
  int index=0;
  char buf[50];

  nbCaseVide=0;


  for(l=7;l>=0;l--) {
	  for(c=0;c<8;c++) {
		ca= case_getCoordonneeToCase(c,l);
		if (piece_getCouleur( plateau_getPiece(p,ca) )==NOIR ) delta=0;else delta='A'-'a';

		if ( (piece_getNom(plateau_getPiece(p,ca)) !=PASDEPIECE ) && nbCaseVide!=0) {
			fen[index]='0'+nbCaseVide;index++;
			nbCaseVide=0;
		}

		switch ( piece_getNom(plateau_getPiece(p,ca)) ) {
			case PION:	fen[index]='p'+delta;  index++; break;
			case CAVALIER: 	fen[index]='n'+delta;  index++; break;
			case FOU :	fen[index]='b'+delta;  index++; break;
			case TOUR: 	fen[index]='r'+delta;  index++; break;
			case DAME:	fen[index]='q'+delta;  index++; break;
			case ROI:	fen[index]='k'+delta;  index++; break;
			default :	nbCaseVide++;
		}
	  
	  }
	  if (nbCaseVide!=0) {
		fen[index]='0'+nbCaseVide;index++;
		nbCaseVide=0;
	  }
	  if (l!=0) { fen[index]='/'; index++; }
  }
  
  fen[index]='\0';

  strcat(fen ,  plateau_getCoteCourant(p)==BLANC ? " w ":" b " );
  

  if ( plateau_isPetitRoque(p,BLANC) ) strcat( fen , "K" );
  if ( plateau_isGrandRoque(p,BLANC) ) strcat( fen , "Q" );
  if ( plateau_isPetitRoque(p,NOIR) ) strcat( fen , "k" );
  if ( plateau_isGrandRoque(p,NOIR) ) strcat( fen , "q" );

  if ( (!plateau_isPetitRoque(p,BLANC)) && (!plateau_isGrandRoque(p,BLANC)) &&
       (!plateau_isPetitRoque(p,NOIR)) && (! plateau_isGrandRoque(p,NOIR)) ) strcat( fen , "-" );
  
	strcat( fen , " ");
  strcat( fen , plateau_getCaseEp(p) != CASEVIDE ? case_getNom( plateau_getCaseEp(p) ):"-" );
  

   // TODO : ENLEVER LE COMMENTAIRE mis pour minimiser la taille pour debug perft
  //strcat( fen , " 0 1 ");

  /* TODO A SUPPRIMER EN DESSOUS */


  /*sprintf(buf,"%d %d", p->materiel[BLANC] , p->materiel[NOIR] );
  strcat( fen , buf );  */

}

/*---------------------------------------------------------------------------------------*/
void plateau_printCoupJoue( Plateau *p) {
	int i;
	char coup[10];
	
	for ( i=0 ; i<p->nbCoup ; i++ ) {
		printCoup( p->lcJoue[i] , coup );
		printf("%s ",coup);
	}
	
	printf("\n"); fflush(stdout);
}
