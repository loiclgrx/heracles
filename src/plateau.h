#ifndef PLATEAU_H
#define PLATEAU_H


#include "bitboard.h"
#include "piece.h"
#include "piece.h"
#include "case.h"
#include "couleur.h"
#include "boolean.h"
#include "epd.h"
//#include "init.h"
#include "annulecoup.h"
#include "type.h"

/* Nombre de coups maximum que l'on peut jouer sur le plateau */
#define NB_COUP_JOUE_MAX	500


typedef struct {
	Couleur cote;			 /* Indique si c'est aux Blancs ou aux Noirs de jouer */
	Case ep;				 /* indique l'eventuelle case en passant */
	Boolean petitRoque[2]; 	 /* indique si le petit roque est possible */
	Boolean grandRoque[2]; 	 /* indique si le grand roque est possible */
	Boolean aRoque[2];		 /* indique si tel joueur a roque ou non */
	
	/* Emplacement des pieces blanches [1] et noires [0] */
	BitBoard bbPion[2];
	BitBoard bbTour[2];
	BitBoard bbCavalier[2];
	BitBoard bbFou[2];
	BitBoard bbDame[2];
	BitBoard bbRoi[2];

	/* Emplacement des pieces Blanches [1] et des pieces Noires [0] */
	BitBoard bbAllPiece[2];	

	/* Rotation des BitBoard contenant l'ensemble des pieces de l'echiquier */
	BitBoard bbAllPiece90;
	BitBoard bbAllPiece45;	
	BitBoard bbAllPiece315;	
	
	/* Indique pour chaque case la piece situee dessus */
	Piece listePiece[ 64 ];
	
	/* Emplacement des rois */
	Case  emplacementRoi[ 2 ];

	/* Materiel des Blancs [1] et des Noirs [0] */
	short materiel[2];

	/* Nombre de coups joues */
	short nbCoup;
	
	/* Liste des coups joués */
	Coup lcJoue[NB_COUP_JOUE_MAX];

	/* Tableau permettant l'annulation des coups joués */
	AnnuleCoup lcUndo[NB_COUP_JOUE_MAX];

	/* cle utlise pour identifier une position dans une table de hachage */
	int64 cle;

	/* cle utlisee pour identifier une position de pions BLANC et NOIR*/
	int64 clePion[2];
} Plateau;

//extern Plateau *p;

		/************/
		/* GETEUR  */
		/************/		

#define plateau_getCoteCourant( p )					((p)->cote)
#define plateau_getCaseEp( p )						(p->ep)
#define plateau_getPiece( p ,c )					(p->listePiece[ c ])
#define plateau_isPetitRoque( p ,co )				(p->petitRoque[ co ])
#define plateau_isGrandRoque( p ,co )				(p->grandRoque[ co ])
#define plateau_getEmplacementRoi( p ,co )			(p->emplacementRoi[ co ])
#define plateau_getMateriel( p ,co )				(p->materiel[ co ])
#define plateau_getNbCoup( p )						(p->nbCoup)
#define plateau_getCle(p)							((p)->cle)
#define plateau_getClePion(p , c)					((p)->clePion[ c ] )
#define plateau_getClePionMixte(p)					(plateau_getClePion(p,BLANC)^\
																						 plateau_getClePion(p,NOIR) )
#define plateau_isOpening(p)			((p)->nbCoup < 13 )

#define plateau_isEndGame(p)		   	( (p->materiel[ BLANC ]) + \
										  (p->materiel[ NOIR ])	< 3000 )
										  
#define plateau_nullMovePossible(p)	    ( (p->materiel[ BLANC ]) + \
									  	  (p->materiel[ NOIR ])	> 700 ) 
									  	  
#define plateau_isMiddleGame(p)		   	( !plateau_isOpening(p) && !plateau_isEndGame(p) )
#define plateau_getCoup( p , n )	    ( p->lcJoue[ n ] )
#define plateau_getUndo( p , n )	    ( p->lcUndo[ n ] )


		/************/
		/* SETEUR  */
		/************/
		
#define plateau_setCoteCourant( p , co )	p->cote=co
#define plateau_setCaseEp( p , ca )			p->ep=ca
#define plateau_setPiece( p , c )			p->listePiece[ c ]=p
#define plateau_setPetitRoque( p , co ,b )	p->petitRoque[ co ]=b
#define plateau_setGrandRoque( p , co ,b )	p->grandRoque[ co ]=b
#define plateau_addCoupJoue( p , c , u )	p->lcJoue[ p->nbCoup ] = c; \
											p->lcUndo[ p->nbCoup ] = u; \
											(p->nbCoup)++

#define plateau_supprimeCoupJoue( p )		(p->nbCoup)--
											 
											

extern void plateau_setPosition( Plateau * , Epd );
extern void plateau_setCle( Plateau * );
extern void plateau_getFen( Plateau *,char * );
extern void printPlateau( Plateau *p ); 
extern void plateau_printCoupJoue( Plateau *p);		// Affiche les coups qui ont ete joues
extern Boolean comparePlateau( Plateau *p1, Plateau *p2 ); 
extern Boolean plateau_caseAttaque( Plateau * ,Case , Couleur );
extern short plateau_nbPieceAttaque( Plateau *p , Case ca , Couleur co);

/* Joue le coup sur l'echiquier puis regarde si le joueur qui joue n'est pas en echec */
extern Boolean plateau_testCoup( Plateau *p ,Case ori, Case dest , Flag flag ,Couleur co);
extern void plateau_joueCoup( Plateau *, Coup c , AnnuleCoup *undo);
extern void plateau_annuleCoup( Plateau * , Coup c , AnnuleCoup *undo);


/* Retourne un BitBoard contenant l'emplacement des pieces clouee par des piece de couleur co.Le clouage  
   est du  a lapiece placée en case CA. Si la piece en ca est le roi c'est un clouage absolue.
*/

extern BitBoard plateau_pieceClouee( Plateau *p, Case ca , Couleur co );


	/***********************************************************************************/
	/* retourne les cases occupees pour une ligne , une colonne , une diagonale donnee */
	/***********************************************************************************/

#define plateau_getLigne( p , c )           (( (p->bbAllPiece[BLANC] | p->bbAllPiece[NOIR]) >> getDecaleBitBoardLigne(c) ) & 0xFF )
#define plateau_getColonne( p , c )         (( p->bbAllPiece90 >> getDecaleBitBoard90(c) ) & 0xFF )
#define plateau_getDiagonaleA1H8( p, c )    (( p->bbAllPiece45 >> getDecaleBitBoard45(c) ) & 0xFF)
#define plateau_getDiagonaleA8H1( p, c )    (( p->bbAllPiece315 >> getDecaleBitBoard315(c) ) & 0xFF)

/* Retourne un Bitboard representant les cases vides de l'echiquier */
#define plateau_getCaseVide( p ) 	    (~( p->bbAllPiece[BLANC] | p->bbAllPiece[NOIR] ))

/* Retourne un Bitboard representant les cases vides et les cases ou se trouve une piece de couleur opposée à "co" */
#define plateau_getCaseVideEtPiecesAdverses( p  , co ) 	    (~p->bbAllPiece[ co ] )

/* Retourne le BitBoard de la case en passant */
#define plateau_getBBEnPassantCase(p) 	    (getCaseToBitBoard( p-> ep) )

/* Retourne le BitBoard de toutes les pieces (noires et blanches) */
#define plateau_getAllPiece( p ) 	    ( p->bbAllPiece[ BLANC ] | p->bbAllPiece[ NOIR ] )

/* Retourne si roque possible faux sinon */
#define plateau_petitRoqueBlancPossible( p )  	(p->petitRoque[ BLANC ] )
#define plateau_grandRoqueBlancPossible( p )	(p->grandRoque[ BLANC ] )

#define plateau_petitRoqueNoirPossible( p )	(p->petitRoque[ NOIR ] )
#define plateau_grandRoqueNoirPossible( p )	(p->grandRoque[ NOIR ] )



/* Ensemble de define permettant la suppression ou l'ajout d'une piece sur le plateau  (getPieceToBitBoard( p->listePiece[ca] )[co])  */

#define plateau_supprimerPiece( p , ca , co )	videBit( getPieceToBitBoard( piece_getNom( p->listePiece[ca] ) )[co]  ,  ca  );\
												videBit( p->bbAllPiece[co] , ca )
#define plateau_supprimerPiece90( p , ca )	videBit90( p->bbAllPiece90 ,  ca )
#define plateau_supprimerPiece45( p , ca )	videBit45( p->bbAllPiece45 , ca )
#define plateau_supprimerPiece315( p , ca )	videBit315(p->bbAllPiece315 , ca )

							
#define plateau_supprimerPieceEchiquier( p , ca , co )  plateau_supprimerPiece( p , ca , co); \
							plateau_supprimerPiece90( p , ca );\
							plateau_supprimerPiece45( p , ca );\
							plateau_supprimerPiece315( p , ca );\
							p->cle ^= getZobrist(piece_getNom(p->listePiece[ca]) ,\
									     co , ca );\
							piece_setNom( p->listePiece[ca] , PASDEPIECE );

//#define plateau_supprimerPieceEchiquier( p , ca , co ) 

#define plateau_ajouterPiece( p , pi , ca ,co)	getPieceToBitBoard( pi )[co] |= getCaseToBitBoard( ca );\
						p->bbAllPiece[ co ] |= getCaseToBitBoard( ca )
#define plateau_ajouterPiece90( p , ca )	p->bbAllPiece90 |= getCaseToBitBoard90( ca )
#define plateau_ajouterPiece45( p , ca )	p->bbAllPiece45 |= getCaseToBitBoard45( ca )
#define plateau_ajouterPiece315( p , ca )	p->bbAllPiece315 |= getCaseToBitBoard315( ca )


#define plateau_ajouterPieceEchiquier( p , pi , ca ,co)	 plateau_ajouterPiece( p , pi , ca ,co);\
							 plateau_ajouterPiece90( p , ca );\
							 plateau_ajouterPiece45( p , ca );\
							 plateau_ajouterPiece315( p , ca );\
							 piece_setNom( p->listePiece[ca] , pi );\
							 piece_setCouleur( p->listePiece[ca] , co );\
							 if ( pi == ROI )\
								p->emplacementRoi[co]=ca;\
							 p->cle ^= getZobrist( pi , co , ca ); 





/* Retourne un BitBoard avec l'emplacement des pieces de couleur co qui attaquent la case ca */
#define plateau_pieceAttaqueCase( p , ca , co ) ( getDeplacementCavalier( ca ) & p->bbCavalier[ co ] ) |\
				   								(( getDeplacementFouA1H8( ca ,\
													  plateau_getDiagonaleA1H8( p , ca ) )  |\
											   getDeplacementFouA8H1( ca ,\
							   						  plateau_getDiagonaleA8H1( p , ca ) )  )\
				  	 						   &\
	      										(p->bbFou[ co ] | p->bbDame[ co ] )   ) |\
											   (( getDeplacementTourLigne( ca , plateau_getLigne( p , ca ) ) |\
				 							  getDeplacementTourColonne( ca ,\
							  								 plateau_getColonne( p , ca ) ) ) \
											  &\
											 (p->bbTour[ co ] | p->bbDame[ co ] )   ) |\
				 							   ( getDeplacementRoi( ca ) & p->bbRoi[ co ] ) |\
											   ( getDeplacementPionCapture( ca , couleur_oppose(co) ) &\
											 p->bbPion[ co ] )

#define estEchec( p ) 	plateau_caseAttaque( p ,  \
						plateau_getEmplacementRoi( p , plateau_getCoteCourant(p) ) , \
						couleur_oppose( plateau_getCoteCourant(p) ) )
													


/* Macro pour jouer un coup nul sur l'echiquier */
#define plateau_joueNullMove( p , u )   annulecoup_setEp( u , plateau_getCaseEp(p) ); \
										if ( plateau_getCaseEp( p ) != CASEVIDE ) { \
											 		p->cle ^= getZobristEnPassant( plateau_getCaseEp( p ) ); \
													plateau_setCaseEp( p , CASEVIDE ); \
										} \
										p->cle ^= zobristBlanc; \
										plateau_setCoteCourant( p , couleur_oppose( plateau_getCoteCourant(p) ) ); \
										plateau_addCoupJoue( p , COUP_NULL , *u )
												
#define plateau_annuleNullMove( p , u )	if ( annulecoup_getEp( u ) != CASEVIDE ) { \
												plateau_setCaseEp( p , annulecoup_getEp( u ) ); \
												p->cle ^= getZobristEnPassant( annulecoup_getEp( u ) ); \
										} \
										p->cle ^= zobristBlanc; \
										plateau_setCoteCourant( p , couleur_oppose( plateau_getCoteCourant(p) ) ); \
										plateau_supprimeCoupJoue( p )

#define plateau_annuleDernierCoup(p)		if ( (p->lcJoue[ (p->nbCoup)-1 ]) == COUP_NULL) { \
												plateau_annuleNullMove( p , (&(p->lcUndo[ (p->nbCoup)-1 ])) );	 \
											}																	 \
											else {																 \
												plateau_annuleCoup(p,p->lcJoue[ (p->nbCoup)-1 ],&(p->lcUndo[ (p->nbCoup)-1 ]) ); \
											}
																																	
//#define plateau_ajouterPieceEchiquier( p , pi , ca ,co)	
						 
#endif /* PLATEAU_H */



