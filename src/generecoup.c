#include <stdio.h>
#include "plateau.h"
#include "couleur.h"
#include "bitboard.h"
#include "init.h"
#include "case.h"
#include "constante.h"
#include "trace.h"
#include "inline.h"
#include "listecoup.h"

/*--------------------------------------------------------------------------------------*/
void genereCoupEchappeEchec(  ListeCoup *lc , Plateau *p, Couleur c, BitBoard pieceAttaqueRoi ) {
	register BitBoard bo;
	register BitBoard bd;
	register BitBoard caseVide,caseCapture,caseVideEtAdverse;
	BitBoard pieceBlancheEtNoire = plateau_getAllPiece( p );
	BitBoard pieceClouee;
	BitBoard pieceAttaqueRoiBis=pieceAttaqueRoi,casePossible;
	Case ori,dest,caseAttaqueRoi;
	BitBoard pionCloue;

	caseVide  =  plateau_getCaseVide( p );
	caseCapture =  p->bbAllPiece[ couleur_oppose(c) ] | plateau_getBBEnPassantCase(p);
	caseVideEtAdverse =  ~p->bbAllPiece[ c ];

	TRACE_GENCOUP("DEP ROI\n");
	bo = p->bbRoi[ c ];
	
	while (bo) {
		ori = getFirstBit( bo );
		
		bd =  (getDeplacementRoi( ori ) & caseVideEtAdverse ) ;

		while ( bd) {
			dest =  getFirstBit( bd );
			plateau_supprimerPieceEchiquier( p , ori , c );
			if ( !plateau_caseAttaque(p,dest,couleur_oppose(c) ) ) {
			  TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			  listecoup_addCoup( lc , ori , dest, 
					     FLAG_ROI  | 
					     getFlagCapture(piece_getNom(plateau_getPiece(p,dest))),
					     c );
			}
			plateau_ajouterPieceEchiquier( p , ROI , ori , c );
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}

	/* On regarde si c'est un echec double si oui pas necesaire de continuer */
	
	caseAttaqueRoi = getFirstBit( pieceAttaqueRoiBis );
	videBit( pieceAttaqueRoiBis , caseAttaqueRoi );

	TRACE_GENCOUP1("Case qui  met en echec %s\n",case_getNom(caseAttaqueRoi) );

	if ( pieceAttaqueRoiBis ) return;

	pieceClouee = plateau_pieceClouee( p , plateau_getEmplacementRoi(p,c) , couleur_oppose(c) );
	casePossible = getCasesEntreDeuxCases( caseAttaqueRoi , plateau_getEmplacementRoi(p,c) ) |
		       getCaseToBitBoard( caseAttaqueRoi );

	TRACE_GENCOUP("Case possible\n");
	//printBitBoard( casePossible );
	bo = p->bbCavalier[ c ];

	while (bo) {
		ori = getFirstBit( bo );
		
		bd =  (getDeplacementCavalier( ori ) &  caseVideEtAdverse & casePossible );

		if ( getCaseToBitBoard(ori) & pieceClouee )
		  bd &= getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori );

		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			TRACE_GENCOUP1("Piece capture %d\n", piece_getNom(plateau_getPiece(p,dest)) );
			listecoup_addCoup( lc ,  ori , dest,
				       	   FLAG_CAVALIER | 
					   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))),
					   c  );
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}




	 bo = p->bbTour[ c ];
	
	while (bo) {
		ori = getFirstBit( bo );
		

		bd =  (getDeplacementTourLigne( ori , plateau_getLigne( p , ori ) ) | 
				getDeplacementTourColonne( ori , plateau_getColonne( p , ori ) ) ) &
				caseVideEtAdverse & casePossible ;
				
		if ( getCaseToBitBoard(ori) & pieceClouee )
		  bd &= getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori );

		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, 
					    FLAG_TOUR  | 
					    getFlagCapture(piece_getNom(plateau_getPiece(p,dest))), c);
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}

	 bo = p->bbDame[ c ];
	
	while (bo) {
		ori = getFirstBit( bo );
				
		bd =  ( getDeplacementTourLigne( ori , plateau_getLigne( p , ori ) )  | 
				 getDeplacementTourColonne( ori , plateau_getColonne( p , ori ) ) |
				 getDeplacementFouA1H8( ori , plateau_getDiagonaleA1H8( p , ori ) )  |
			 	 getDeplacementFouA8H1( ori , plateau_getDiagonaleA8H1( p , ori ) )  ) &
				 caseVideEtAdverse & casePossible;

		
		if ( getCaseToBitBoard(ori) & pieceClouee )
		  bd &= getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori );
		
		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, 
					FLAG_DAME | 
					getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}

	 bo = p->bbFou[ c ];
	
	while (bo) {
		ori = getFirstBit( bo );
		
		
		bd =  ( getDeplacementFouA1H8( ori , plateau_getDiagonaleA1H8( p , ori ) )  |
				 getDeplacementFouA8H1( ori , plateau_getDiagonaleA8H1( p , ori ) )  ) &
				caseVideEtAdverse & casePossible;
		
		
		if ( getCaseToBitBoard(ori) & pieceClouee )
		  bd &= getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori );
		
		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, 
					   FLAG_FOU  | 
					   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))), c );
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}

 	pionCloue =  p->bbPion[ c ] & pieceClouee;	

	if ( c == BLANC ) {
		/*********************/
		/* Avance d'une case */
		/*********************/		
		bd = ( p->bbPion[ BLANC ] << 8 ) & caseVide & casePossible;
		

		while (bd) {
			dest = getFirstBit( bd );
			ori = dest-8;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if (dest>55)  {
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_DAME , c );
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_TOUR , c );
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_CAVALIER , c );
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_FOU , c );
			}
			else {
				listecoup_addCoup( lc , ori , dest, FLAG_PION , c );
			}
			videBit( bd , dest );			
		}
		
		/*********************/		
		/* Avance de 2 cases */
		/*********************/		
		bd = ( ( p->bbPion[ BLANC ] & LIGNE2 ) << 8 ) & caseVide;
		bd = ( bd << 8 ) & caseVide & casePossible;		

		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest-16;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, FLAG_PION , c );
			videBit( bd , dest );			
		}  
		
		/******************************************/		
		/* Captures  a droite avec prise en passant */		
		/******************************************/

		if ( caseAttaqueRoi == (plateau_getCaseEp(p)-8) )  
		  bd = ( ( p->bbPion[ BLANC ] & COLONNE1A7 ) << 9 ) & caseCapture & (casePossible|plateau_getBBEnPassantCase(p));
		else
		  bd = ( ( p->bbPion[ BLANC ] & COLONNE1A7 ) << 9 ) & caseCapture & casePossible;
		
		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest-9;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if ( dest == plateau_getCaseEp(p) ) {
				listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
			}
			else {
			   if (dest>55) {
				listecoup_addCoup( lc , ori , dest,
					           FLAG_PION_PROMOTION_DAME | 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				listecoup_addCoup( lc , ori , dest,
					           FLAG_PION_PROMOTION_TOUR| 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				listecoup_addCoup( lc , ori , dest, 
						   FLAG_PION_PROMOTION_FOU | 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				listecoup_addCoup( lc , ori , dest, 
						   FLAG_PION_PROMOTION_CAVALIER | 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				}
				else {
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				}
			}
			videBit( bd , dest );			
		}  
		
		/********************************************/
		/* Captures  a gauche avec prise en passant*/		
		/********************************************/

		if ( caseAttaqueRoi == plateau_getCaseEp(p)-8 )  
		  bd = ( ( p->bbPion[ BLANC ] & COLONNE2A8 ) << 7 ) & caseCapture & (casePossible|plateau_getBBEnPassantCase(p));
		else
		  bd = ( ( p->bbPion[ BLANC ] & COLONNE2A8 ) << 7 ) & caseCapture & casePossible;
		
		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest-7;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if ( dest == plateau_getCaseEp(p) ) {
				listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c);
			}
			else {
				if (dest>55) {
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_DAME | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_TOUR | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_FOU| 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );								
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_CAVALIER | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				}
				else {
				  listecoup_addCoup( lc , ori , dest, 
						   FLAG_PION| 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );			
				}
			}
			videBit( bd , dest );			
		}  
	}	
	else {		/* deplacement des pions NOIR */

		/*********************/
		/* Avance d'une case */
		/*********************/		
		bd = ( p->bbPion[ NOIR ] >> 8 ) & caseVide & casePossible;
	
		TRACE_GENCOUP1("nbCoup %d\n", listecoup_getNbCoup(lc) );	
		while (bd) {
			dest = getFirstBit( bd );
			ori = dest+8;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if (dest<8) {
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_DAME , c);
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_TOUR , c);				
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_CAVALIER ,c );				
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_FOU , c);				
			}
			else {
				listecoup_addCoup( lc , ori , dest, FLAG_PION ,c);					      }

			videBit( bd , dest );			
		}
		
		/*********************/		
		/* Avance de 2 cases */
		/*********************/		
		bd = ( ( p->bbPion[ NOIR ] & LIGNE7 ) >> 8 ) & caseVide;
		bd = ( bd >> 8 ) & caseVide & casePossible;		

		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest+16;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, FLAG_PION ,c );							
			videBit( bd , dest );			
		}  
		
		/******************************************/		
		/* Captures  a droite avec prise en passant */		
		/******************************************/

		if ( caseAttaqueRoi == plateau_getCaseEp(p)+8 )  
		  bd = ( ( p->bbPion[ NOIR ] & COLONNE1A7 ) >> 7 ) & caseCapture &(casePossible|plateau_getBBEnPassantCase(p));
		else
		  bd = ( ( p->bbPion[ NOIR ] & COLONNE1A7 ) >> 7 ) & caseCapture & casePossible;
		
		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest+7;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if ( dest == plateau_getCaseEp(p) ) {
				listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
			}
			else {
				if (dest<8) {
		TRACE_GENCOUP1("nbCoup %d\n", listecoup_getNbCoup(lc) );	
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_DAME| getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );			
		TRACE_GENCOUP1("nbCoup %d\n", listecoup_getNbCoup(lc) );	
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_TOUR | getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
		TRACE_GENCOUP1("nbCoup %d\n", listecoup_getNbCoup(lc) );	
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_FOU| getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );								
		TRACE_GENCOUP1("nbCoup %d\n", listecoup_getNbCoup(lc) );	
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_CAVALIER | getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				}
				else {
				  listecoup_addCoup( lc , ori , dest, 
					FLAG_PION | 
					getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );
				}
			}

			videBit( bd , dest );			
		}  
		
		/********************************************/
		/* Captures  a gauche avec prise en passant*/		
		/********************************************/

		if ( caseAttaqueRoi == plateau_getCaseEp(p)+8 )  
		  bd = ( ( p->bbPion[ NOIR ] & COLONNE2A8 ) >> 9 ) & caseCapture & (casePossible|plateau_getBBEnPassantCase(p));
		else
		  bd = ( ( p->bbPion[ NOIR ] & COLONNE2A8 ) >> 9 ) & caseCapture & casePossible;
		
		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest+9;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if ( dest == plateau_getCaseEp(p) ) {
				listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
			}
			else {
				if (dest<8) {
				  TRACE_GENCOUP1("nbCoup %d\n", listecoup_getNbCoup(lc) );	
				  listecoup_addCoup( lc , ori , dest, 
					FLAG_PION_PROMOTION_DAME | 
					getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				  TRACE_GENCOUP1("nbCoup %d\n", listecoup_getNbCoup(lc) );	
				  listecoup_addCoup( lc , ori , dest, 
					FLAG_PION_PROMOTION_TOUR | 
					getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				  TRACE_GENCOUP1("nbCoup %d\n", listecoup_getNbCoup(lc) );	
				  listecoup_addCoup( lc , ori , dest, 
					FLAG_PION_PROMOTION_FOU | 
					getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				  TRACE_GENCOUP1("nbCoup %d\n", listecoup_getNbCoup(lc) );	
				  listecoup_addCoup( lc , ori , dest, 
					FLAG_PION_PROMOTION_CAVALIER | 
					getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				  TRACE_GENCOUP1("**nbCoup %d\n", listecoup_getNbCoup(lc) );	
				}
				else {				
					listecoup_addCoup( lc , ori , dest, FLAG_PION | getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				}
			}

			videBit( bd , dest );			
		}  
	}
			/*****************/
			/* Roque BLANC */
			/*****************/
		TRACE_GENCOUP1("---nbCoup %d\n", listecoup_getNbCoup(lc) );	
		
	if ( c == BLANC ) {
		if ( plateau_petitRoqueBlancPossible(p) && (!( CASE_F1G1 & pieceBlancheEtNoire )) ) {
			if ( !plateau_caseAttaque( p, E1 , NOIR ) && 
			     !plateau_caseAttaque( p, F1 , NOIR ) && 
				!plateau_caseAttaque( p , G1 , NOIR ) ) {
					TRACE_GENCOUP("Coup possible O-O\n");			
					listecoup_addCoup( lc , E1 , G1, FLAG_ROI_ROQUE , c);			}
		}
		if ( plateau_grandRoqueBlancPossible(p) && (!( CASE_B1C1D1 & pieceBlancheEtNoire )) ) {
			if ( !plateau_caseAttaque( p, C1 , NOIR ) && 
			     !plateau_caseAttaque( p, D1 , NOIR ) && 
				!plateau_caseAttaque( p, E1 , NOIR ) )  {
					TRACE_GENCOUP("Coup possible O-O-O\n");			
					listecoup_addCoup( lc , E1 , C1 , FLAG_ROI_ROQUE ,c );			
			}
		}		
	}
	else {
			/*****************/
			/* Roque NOIR    */
			/*****************/
		if ( plateau_petitRoqueNoirPossible(p) && (!( CASE_F8G8 & pieceBlancheEtNoire )) ) {
			if ( !plateau_caseAttaque( p, E8 , BLANC ) && 
			     !plateau_caseAttaque( p, F8 , BLANC ) && 
				!plateau_caseAttaque( p, G8 , BLANC ) )  {
					TRACE_GENCOUP("Coup possible O-O\n");			
					listecoup_addCoup( lc , E8 , G8, FLAG_ROI_ROQUE ,c);			
			}
		}
		if ( plateau_grandRoqueNoirPossible(p) && (!( CASE_B8C8D8 & pieceBlancheEtNoire )) ) {
			if ( !plateau_caseAttaque( p, C8 , BLANC ) && 
			     !plateau_caseAttaque( p, D8 , BLANC ) && 
				!plateau_caseAttaque( p, E8 , BLANC ) )  {
					TRACE_GENCOUP("Coup possible O-O-O\n");			
					listecoup_addCoup( lc , E8 , C8 , FLAG_ROI_ROQUE ,c );			
				}

		}			
	}
}

/*--------------------------------------------------------------------------------------*/
void genereCoupCapture( ListeCoup *lc , Plateau *p, Couleur c ) {
	register BitBoard bo;
	register BitBoard bd;
	register BitBoard caseCapture,caseEp;
	BitBoard pieceClouee;
	BitBoard pionCloue;
	Case ori,dest;

	/* TODO : Pour la fonction d'evaluation de la mobilite j'interdis la capture de la prise en passant
	 * si je genere les coups pour le joueur pour qui ce n'est pas le tour de jouer
	 * Lors de l'optimisation de l'evaluation de la mobilite supprimer ce test 
	 * Ce test sera tout compte fait peut etre necessaire pour le Null Move*/

	caseCapture =  p->bbAllPiece[ couleur_oppose(c) ];
	caseEp      = ((plateau_getCoteCourant(p)==c) ? plateau_getBBEnPassantCase(p) : 0);

	pieceClouee = plateau_pieceClouee( p , plateau_getEmplacementRoi(p,c) , 
					   couleur_oppose(c) );

			/* Capture avec les pions */

	TRACE_GENCOUP("BitBoard pion + BitBoard pieces Cloues\n");
 	pionCloue =  (p->bbPion[ c ]) & pieceClouee;	

	if ( c == BLANC ) {
		
		/******************************************/		
		/* Captures  a droite avec prise en passant */		
		/******************************************/
		bd = ( ( p->bbPion[ BLANC ] & COLONNE1A7 ) << 9 ) & (caseCapture | caseEp );
		
		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest-9;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if ( dest == plateau_getCaseEp(p) ) {
				if ( !(plateau_testCoup(p,ori,dest,FLAG_PION_ENPASSANT,c) ) ) {
			  	   listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
				}
			}
			else {
			   if (dest>55) {
				listecoup_addCoup( lc , ori , dest,
					           FLAG_PION_PROMOTION_DAME | 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				listecoup_addCoup( lc , ori , dest,
					           FLAG_PION_PROMOTION_TOUR| 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				listecoup_addCoup( lc , ori , dest, 
						   FLAG_PION_PROMOTION_FOU | 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				listecoup_addCoup( lc , ori , dest, 
						   FLAG_PION_PROMOTION_CAVALIER | 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				}
				else {	
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				}
			}
			videBit( bd , dest );			
		}  
		
		/********************************************/
		/* Captures  a gauche avec prise en passant*/		
		/********************************************/
		bd = ( ( p->bbPion[ BLANC ] & COLONNE2A8 ) << 7 ) & ( caseCapture | caseEp );
		
		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest-7;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );

			if ( dest == plateau_getCaseEp(p) ) {
					if ( !(plateau_testCoup(p,ori,dest,FLAG_PION_ENPASSANT,c) ) ) {
				  	  listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
					}
			}
			else {
				if (dest>55) {
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_DAME | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_TOUR | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_FOU| 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );								
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_CAVALIER | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				}
				else	{			
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION| 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );			
				}
			}
			videBit( bd , dest );			
		}  
	}	
	else {		/* deplacement des pions NOIR */

		/******************************************/		
		/* Captures  a droite avec prise en passant */		
		/******************************************/
		bd = ( ( p->bbPion[ NOIR ] & COLONNE1A7 ) >> 7 ) & ( caseCapture | caseEp );

		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest+7;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );

			if ( dest == plateau_getCaseEp(p) ) {
				if ( !(plateau_testCoup(p,ori,dest,FLAG_PION_ENPASSANT,c) ) ) {
				  	  listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
				}
			}
			else {
				if (dest<8) {
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_DAME | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );			
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_TOUR | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_FOU | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );								
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_CAVALIER |
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				}
				else {
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				}
			}

			videBit( bd , dest );			
		}  
		
		/********************************************/
		/* Captures  a gauche avec prise en passant*/		
		/********************************************/
		bd = ( ( p->bbPion[ NOIR ] & COLONNE2A8 ) >> 9 ) & ( caseCapture | caseEp );
		
		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest+9;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );

			if ( dest == plateau_getCaseEp(p) ) {
				if ( !(plateau_testCoup(p,ori,dest,FLAG_PION_ENPASSANT,c) ) ) {
				  	  listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
				}
			}
			else {
				if (dest<8) {
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_DAME | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_TOUR | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_FOU | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_CAVALIER | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				}
				else {
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				}
			}

			videBit( bd , dest );			
		}  
	}
			/* Capture du cavalier */

	/* Un cavalier cloue ne peut pas bouger 
	 * TODO : faire la modif dans genereCoupSiPasEchec car plus performant */

	bo = p->bbCavalier[ c ] & (~pieceClouee);

	while (bo) {
		ori = getFirstBit( bo );
		
		bd =  (getDeplacementCavalier( ori ) &  caseCapture );

		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			TRACE_GENCOUP1("Piece capture %d\n", piece_getNom(plateau_getPiece(p,dest)) );
			listecoup_addCoup( lc ,  ori , dest,
				       	   FLAG_CAVALIER | 
					   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))),
					   c  );
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}

			/* Capture du Fou */
	
	bo = p->bbFou[ c ];
	
	while (bo) {
		ori = getFirstBit( bo );
		
		bd =  ( getDeplacementFouA1H8( ori , plateau_getDiagonaleA1H8( p , ori ) )  |
			getDeplacementFouA8H1( ori , plateau_getDiagonaleA8H1( p , ori ) )  ) &
			caseCapture;
					
		if ( getCaseToBitBoard(ori) & pieceClouee )
		  bd &= getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori );
		
		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, 
					   FLAG_FOU  | 
					   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))), c );
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}

			/* Capture de la Tour */

	bo = p->bbTour[ c ];
	
	while (bo) {
		ori = getFirstBit( bo );
		
		bd =  (getDeplacementTourLigne( ori , plateau_getLigne( p , ori ) ) | 
		      getDeplacementTourColonne( ori , 
						 plateau_getColonne( p , ori ) ) ) &
		      caseCapture;
		
		if ( getCaseToBitBoard(ori) & pieceClouee )
		  bd &= getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori );

		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, 
					    FLAG_TOUR  | 
					    getFlagCapture(piece_getNom(plateau_getPiece(p,dest))), c);
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}

			/* Capture de la Dame */

	 bo = p->bbDame[ c ];
	
	while (bo) {
		ori = getFirstBit( bo );
		
		bd =  ( getDeplacementTourLigne( ori , plateau_getLigne( p , ori ) )  | 
		        getDeplacementTourColonne( ori , plateau_getColonne( p , ori ) ) |
			getDeplacementFouA1H8( ori , plateau_getDiagonaleA1H8( p , ori ) )  |
			getDeplacementFouA8H1( ori , plateau_getDiagonaleA8H1( p , ori ) )  ) &
			caseCapture;

		if ( getCaseToBitBoard(ori) & pieceClouee )
		  bd &= getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori );
	
		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, 
					FLAG_DAME | 
					getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}

			/* Capture du Roi */

	/* TODO : il ne peut avoir qu'un roi sur l'echequier donc pas necessaire de boucler
	 * faire la modif dans genereCoupSiPasEchec */

	ori = getFirstBit( p->bbRoi[ c ] );
	bd =  (getDeplacementRoi( ori ) & caseCapture ) ;

	while ( bd) {
		dest =  getFirstBit( bd );
		if ( !plateau_caseAttaque(p,dest,couleur_oppose(c) ) ) {
		  TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
		  listecoup_addCoup( lc , ori , dest, 
				     FLAG_ROI  | 
				     getFlagCapture(piece_getNom(plateau_getPiece(p,dest))),
				     c );
		}
		videBit( bd , dest );
	}
} 
/*--------------------------------------------------------------------------------------*/
void genereCoupSiPasEchec( ListeCoup *lc , Plateau *p, Couleur c ) {
	register BitBoard bo = p->bbCavalier[ c ];
	register BitBoard bd;
	register BitBoard caseVide,caseCapture,caseVideEtAdverse;
	BitBoard pieceBlancheEtNoire = plateau_getAllPiece( p );
	BitBoard pieceClouee;
	Case ori,dest;
	BitBoard pionCloue;

	caseVide  =  plateau_getCaseVide( p );
	/* TODO : Pour la fonction d'evaluation de la mobilite j'interdis la capture de la prise en passant
	 * si je genere les coups pour le joueur pour qui ce n'est pas le tour de jouer
	 * Lors de l'optimisation de l'evaluation de la mobilite supprimer ce test 
	 * Ce test sera tout compte fait peut etre necessaire pour le Null Move*/

	caseCapture =  p->bbAllPiece[ couleur_oppose(c) ] | 
		      ((plateau_getCoteCourant(p)==c) ? plateau_getBBEnPassantCase(p) : 0);
//	caseCapture =  p->bbAllPiece[ couleur_oppose(c) ] | plateau_getBBEnPassantCase(p);
	caseVideEtAdverse =  ~p->bbAllPiece[ c ];
	pieceClouee = plateau_pieceClouee( p , plateau_getEmplacementRoi(p,c) , couleur_oppose(c) );

	while (bo) {
		ori = getFirstBit( bo );
		
		bd =  (getDeplacementCavalier( ori ) &  caseVideEtAdverse );

		if ( getCaseToBitBoard(ori) & pieceClouee )
		  bd &= getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori );

		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			TRACE_GENCOUP1("Piece capture %d\n", piece_getNom(plateau_getPiece(p,dest)) );
			listecoup_addCoup( lc ,  ori , dest,
				       	   FLAG_CAVALIER | 
					   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))),
					   c  );
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}


	 bo = p->bbRoi[ c ];
	
	while (bo) {
		ori = getFirstBit( bo );
		
		bd =  (getDeplacementRoi( ori ) & caseVideEtAdverse ) ;

		while ( bd) {
			dest =  getFirstBit( bd );
			if ( !plateau_caseAttaque(p,dest,couleur_oppose(c) ) ) {
			  TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			  listecoup_addCoup( lc , ori , dest, 
					     FLAG_ROI  | 
					     getFlagCapture(piece_getNom(plateau_getPiece(p,dest))),
					     c );
			}
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}


	 bo = p->bbTour[ c ];
	
	while (bo) {
		ori = getFirstBit( bo );
		

		bd =  (getDeplacementTourLigne( ori , plateau_getLigne( p , ori ) ) | 
				getDeplacementTourColonne( ori , plateau_getColonne( p , ori ) ) ) &
				caseVideEtAdverse;
				
		
		if ( getCaseToBitBoard(ori) & pieceClouee )
		  bd &= getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori );
#if 0
		TRACE_GENCOUP("Piece Cloue\n");
		printBitBoard( pieceClouee );
		
		TRACE_GENCOUP1("Emplacement Roi %d\n",plateau_getEmplacementRoi(p,c));
		TRACE_GENCOUP("Droite BitBoard\n");
		printBitBoard( getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) );
		
		TRACE_GENCOUP("Deplacement reelle de la tour\n");
		printBitBoard( bd );
#endif

		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, 
					    FLAG_TOUR  | 
					    getFlagCapture(piece_getNom(plateau_getPiece(p,dest))), c);
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}

	 bo = p->bbDame[ c ];
	
	while (bo) {
		ori = getFirstBit( bo );
		
		bd =  ( getDeplacementTourLigne( ori , plateau_getLigne( p , ori ) )  | 
				 getDeplacementTourColonne( ori , plateau_getColonne( p , ori ) ) |
				 getDeplacementFouA1H8( ori , plateau_getDiagonaleA1H8( p , ori ) )  |
			 	 getDeplacementFouA8H1( ori , plateau_getDiagonaleA8H1( p , ori ) )  ) &
				 caseVideEtAdverse;


		if ( getCaseToBitBoard(ori) & pieceClouee )
		  bd &= getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori );
	
		
		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, 
					FLAG_DAME | 
					getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}

	 bo = p->bbFou[ c ];
	
	while (bo) {
		ori = getFirstBit( bo );
		
		bd =  ( getDeplacementFouA1H8( ori , plateau_getDiagonaleA1H8( p , ori ) )  |
				 getDeplacementFouA8H1( ori , plateau_getDiagonaleA8H1( p , ori ) )  ) &
				caseVideEtAdverse;
					
				
		if ( getCaseToBitBoard(ori) & pieceClouee )
		  bd &= getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori );
		

		
		while ( bd) {
			dest =  getFirstBit( bd );
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, 
					   FLAG_FOU  | 
					   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))), c );
			videBit( bd , dest );
		}
		videBit( bo , ori );
	}

	TRACE_GENCOUP("BitBoard pion + BitBoard pieces Cloues\n");
	//printBitBoard( p->bbPion[ c ] ); 
	//printBitBoard( pieceClouee ); 
 	pionCloue =  (p->bbPion[ c ]) & pieceClouee;	
	//printBitBoard( pionCloue ); 
	//if ( pionCloue ) TRACE_GENCOUP("JE PEUT PAX BOUGER\n");

	if ( c == BLANC ) {
		/*********************/
		/* Avance d'une case */
		/*********************/		
		bd = ( p->bbPion[ BLANC ] << 8 ) & caseVide;
		
		while (bd) {
			dest = getFirstBit( bd );
			ori = dest-8;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if (dest>55)  {
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_DAME , c );
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_TOUR , c );
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_CAVALIER , c );
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_FOU , c );
			}
			else {
				listecoup_addCoup( lc , ori , dest, FLAG_PION , c );
			}
			videBit( bd , dest );			
		}
		
		/*********************/		
		/* Avance de 2 cases */
		/*********************/		
		bd = ( ( p->bbPion[ BLANC ] & LIGNE2 ) << 8 ) & caseVide;
		bd = ( bd << 8 ) & caseVide;		

		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest-16;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, FLAG_PION , c );
			videBit( bd , dest );			
		}  
		
		/******************************************/		
		/* Captures  a droite avec prise en passant */		
		/******************************************/
		bd = ( ( p->bbPion[ BLANC ] & COLONNE1A7 ) << 9 ) & caseCapture;
		
		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest-9;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if ( dest == plateau_getCaseEp(p) ) {
				/* Si roi + pion capture + tour dame ou fou alignes alors verifier si pas echec */
		/*		if ( getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , plateau_getCaseEp(p)-8 ) &&
				     (p->bbTour[NOIR] || p->bbFou[NOIR] || p->bbDame[NOIR]) ) {*/
					if ( !(plateau_testCoup(p,ori,dest,FLAG_PION_ENPASSANT,c) ) ) {
				  	  listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
					}
			/*	}
				else {
				  listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
				} */
			}
			else {
			   if (dest>55) {
				listecoup_addCoup( lc , ori , dest,
					           FLAG_PION_PROMOTION_DAME | 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				listecoup_addCoup( lc , ori , dest,
					           FLAG_PION_PROMOTION_TOUR| 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				listecoup_addCoup( lc , ori , dest, 
						   FLAG_PION_PROMOTION_FOU | 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				listecoup_addCoup( lc , ori , dest, 
						   FLAG_PION_PROMOTION_CAVALIER | 
						   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				}
				else {	
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				}
			}
			videBit( bd , dest );			
		}  
		
		/********************************************/
		/* Captures  a gauche avec prise en passant*/		
		/********************************************/
		bd = ( ( p->bbPion[ BLANC ] & COLONNE2A8 ) << 7 ) & caseCapture;
		
		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest-7;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if ( dest == plateau_getCaseEp(p) ) {
				/* Si roi + pion capture + tour dame ou fou alignes alors verifier si pas echec */
		/*		if ( getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , plateau_getCaseEp(p)-8 ) &&
				     (p->bbTour[NOIR] || p->bbFou[NOIR] || p->bbDame[NOIR]) ) { */
					if ( !(plateau_testCoup(p,ori,dest,FLAG_PION_ENPASSANT,c) ) ) {
				  	  listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
					}
/*				}
				else {
				  listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
				} */
			}
			else {
				if (dest>55) {
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_DAME | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_TOUR | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_FOU| 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );								
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION_PROMOTION_CAVALIER | 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				}
				else	{			
					listecoup_addCoup( lc , ori , dest, 
							   FLAG_PION| 
							   getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );			
				}
			}
			videBit( bd , dest );			
		}  
	}	
	else {		/* deplacement des pions NOIR */

		/*********************/
		/* Avance d'une case */
		/*********************/		
		bd = ( p->bbPion[ NOIR ] >> 8 ) & caseVide;
		
		while (bd) {
			dest = getFirstBit( bd );
			ori = dest+8;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if (dest<8) {
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_DAME , c);
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_TOUR , c);				
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_CAVALIER ,c );				
				listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_FOU , c);				
			}
			else {
				listecoup_addCoup( lc , ori , dest, FLAG_PION ,c);
			}
			videBit( bd , dest );			
		}
		/*********************/		
		/* Avance de 2 cases */
		/*********************/		
		bd = ( ( p->bbPion[ NOIR ] & LIGNE7 ) >> 8 ) & caseVide;
		bd = ( bd >> 8 ) & caseVide;		

		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest+16;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			listecoup_addCoup( lc , ori , dest, FLAG_PION ,c );							
			videBit( bd , dest );			
		}  

		/******************************************/		
		/* Captures  a droite avec prise en passant */		
		/******************************************/
		bd = ( ( p->bbPion[ NOIR ] & COLONNE1A7 ) >> 7 ) & caseCapture;
		
		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest+7;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if ( dest == plateau_getCaseEp(p) ) {
				/* Si roi + pion capture + tour dame ou fou alignes alors verifier si pas echec */
	/*			if ( getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , plateau_getCaseEp(p)+8 ) &&
				     (p->bbTour[BLANC] || p->bbFou[BLANC] || p->bbDame[BLANC]) ) { */
					if ( !(plateau_testCoup(p,ori,dest,FLAG_PION_ENPASSANT,c) ) ) {
				  	  listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
					}
			/*	}
				else {
				  listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
				} */
			}
			else {
				if (dest<8) {
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_DAME| getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );			
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_TOUR | getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_FOU| getFlagCapture(piece_getNom(plateau_getPiece(p,dest)))  , c );								
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_CAVALIER | getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				}
				else {
					listecoup_addCoup( lc , ori , dest, FLAG_PION | getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				}
			}

			videBit( bd , dest );			
		}  
		
		/********************************************/
		/* Captures  a gauche avec prise en passant*/		
		/********************************************/
		bd = ( ( p->bbPion[ NOIR ] & COLONNE2A8 ) >> 9 ) & caseCapture;
		
		while (bd) {
			dest = getFirstBit( bd );			
			ori = dest+9;
			if (pionCloue) {
			   if ( pionCloue & getCaseToBitBoard(ori) ) {
			     if (!( getCaseToBitBoard(dest) & 
				  getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , ori ) )){
				    videBit( bd , dest ); 
				    continue; 
			     }
			   }
			}
			TRACE_GENCOUP2("Coup possible %s%s\n",case_getNom(ori),case_getNom(dest) );
			if ( dest == plateau_getCaseEp(p) ) {
				/* Si roi + pion capture + tour dame ou fou alignes alors verifier si pas echec */
/*				if ( getDroiteContenantCases( plateau_getEmplacementRoi(p,c) , plateau_getCaseEp(p)+8 ) &&
				     (p->bbTour[BLANC] || p->bbFou[BLANC] || p->bbDame[BLANC]) ) { */
					if ( !(plateau_testCoup(p,ori,dest,FLAG_PION_ENPASSANT,c) ) ) {
				  	  listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
					}
		/*		}
				else {
				  listecoup_addCoup( lc , ori , dest, FLAG_PION_ENPASSANT , c );
				} */
			}
			else {
				if (dest<8) {
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_DAME | getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_TOUR | getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_FOU | getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
					listecoup_addCoup( lc , ori , dest, FLAG_PION_PROMOTION_CAVALIER | getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );								
				}
				else {
					listecoup_addCoup( lc , ori , dest, FLAG_PION | getFlagCapture(piece_getNom(plateau_getPiece(p,dest))) , c );			
				}
			}

			videBit( bd , dest );			
		}  
	}

			/*****************/
			/* Roque BLANC */
			/*****************/
		
	if ( c == BLANC ) {
		if ( plateau_petitRoqueBlancPossible(p) && (!( CASE_F1G1 & pieceBlancheEtNoire )) ) {
			if ( !plateau_caseAttaque( p, E1 , NOIR ) && 
			     !plateau_caseAttaque( p, F1 , NOIR ) && 
				!plateau_caseAttaque( p, G1 , NOIR ) ) {
					TRACE_GENCOUP("Coup possible O-O\n");			
					listecoup_addCoup( lc , E1 , G1, FLAG_ROI_ROQUE , c);			
				}
		}
		if ( plateau_grandRoqueBlancPossible(p) && (!( CASE_B1C1D1 & pieceBlancheEtNoire )) ) {
			if ( !plateau_caseAttaque( p, C1 , NOIR ) && 
			     !plateau_caseAttaque( p, D1 , NOIR ) && 
				!plateau_caseAttaque( p, E1 , NOIR ) )  {
					TRACE_GENCOUP("Coup possible O-O-O\n");			
					listecoup_addCoup( lc , E1 , C1 , FLAG_ROI_ROQUE ,c );			
			}
		}		
	}
	else {
			/*****************/
			/* Roque NOIR    */
			/*****************/
		if ( plateau_petitRoqueNoirPossible(p) && (!( CASE_F8G8 & pieceBlancheEtNoire )) ) {
			if ( !plateau_caseAttaque( p, E8 , BLANC ) && 
			     !plateau_caseAttaque( p, F8 , BLANC ) && 
				!plateau_caseAttaque( p, G8 , BLANC ) )  {
					TRACE_GENCOUP("Coup possible O-O\n");			
					listecoup_addCoup( lc , E8 , G8, FLAG_ROI_ROQUE ,c);			
			}
		}
		if ( plateau_grandRoqueNoirPossible(p) && (!( CASE_B8C8D8 & pieceBlancheEtNoire )) ) {
			if ( !plateau_caseAttaque( p, C8 , BLANC ) && 
			     !plateau_caseAttaque( p, D8 , BLANC ) && 
				!plateau_caseAttaque( p, E8 , BLANC ) )  {
					TRACE_GENCOUP("Coup possible O-O-O\n");			
					listecoup_addCoup( lc , E8 , C8 , FLAG_ROI_ROQUE ,c );			
				}

		}			
	}
} 
/*------------------------------------------------------------------------------------------*/
void genereCoup( ListeCoup *lc , Plateau *p, Couleur c ) {
	BitBoard pieceAttaqueRoi;

	pieceAttaqueRoi = plateau_pieceAttaqueCase( p, plateau_getEmplacementRoi( p , c ) ,
		       		      		    couleur_oppose( c) );

	if ( pieceAttaqueRoi ) {
		TRACE_GENCOUP("ECHEC !!!\n");
		genereCoupEchappeEchec( lc , p , c , pieceAttaqueRoi );
	}
	else 
		genereCoupSiPasEchec( lc , p , c );
}
