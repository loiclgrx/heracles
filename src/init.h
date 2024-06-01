#ifndef INIT_H
#define INIT_H
 
#include "bitboard.h"
#include "plateau.h"
#include "coup.h"
#include "type.h"

	/**********/
	/* DEFINE */
	/**********/
#define getCaseToBitBoard( c ) 			caseToBitBoard[ c ] 
#define getCaseToBitBoard45( c ) 		caseToBitBoard45[ c ] 
#define getCaseToBitBoard90( c ) 		caseToBitBoard90[ c ] 
#define getCaseToBitBoard315( c ) 		caseToBitBoard315[ c ] 

#define getComplementCaseToBitBoard( c )	complementCaseToBitBoard[ c ]
#define getComplementCaseToBitBoard45( c )	complementCaseToBitBoard45[ c ]
#define getComplementCaseToBitBoard90( c )	complementCaseToBitBoard90[ c ]
#define getComplementCaseToBitBoard315( c )	complementCaseToBitBoard315[ c ]

#define getDeplacementTour(c) 				deplacementTour[ c ]
#define getDeplacementCavalier(c)			deplacementCavalier[ c ]
#define deplacementFou(c) 				deplacementFou[ c ]
#define getDeplacementRoi(c) 				deplacementRoi[ c ]
#define getDeplacementDame(c) 				deplacementDame[ c ]
#define getDeplacementFouA1H8( ca , occDiag ) 		deplacementFouA1H8[ ca ][ occDiag ]
#define getDeplacementFouA8H1( ca , occDiag ) 	   	deplacementFouA8H1[ ca ][ occDiag ]
#define getDeplacementTourColonne(ca, occColonne ) 	deplacementTourColonne[ ca ][occColonne]
#define getDeplacementTourLigne( ca , occLigne )   	deplacementTourLigne[ ca ][ occLigne ]
#define getDeplacementPionCapture( ca , co )		deplacementPionCapture[ ca ][ co ]
#define getDecaleBitBoard45( c) 			decaleBitBoard45[ c ]
#define getDecaleBitBoard315( c) 			decaleBitBoard315[ c ]
#define getDecaleBitBoard90( c) 			decaleBitBoard90[ c ]
#define getDecaleBitBoardLigne( c) 			decaleBitBoardLigne[ c ]
#define getPieceToBitBoard( pi )			pieceToBitBoard[ pi ]
#define getFlagCapture( pi )				captureFlag[ pi ]
#define getCasesEntreDeuxCasesDiag( c1 , c2 )	casesEntreDeuxCasesDiag[c1][c2]
#define getCasesEntreDeuxCasesColLig( c1 , c2 )	casesEntreDeuxCasesColLig[c1][c2]
#define getCasesEntreDeuxCases( c1 , c2 )	casesEntreDeuxCases[c1][c2]
#define getNbBits( b )		( nbBits[ (b)>>48 ] + \
				  nbBits[ ((b)>>32) & LIGNE1ET2 ] + \
				  nbBits[ ((b)>>16) & LIGNE1ET2 ] + \
				  nbBits[ (b) & LIGNE1ET2 ] )

#define getNbBits_ligne12( b )	( nbBits[ (b) & LIGNE1ET2 ] ) 
#define getNbBits_ligne34( b )	( nbBits[ ((b) >> 16) & LIGNE1ET2 ] ) 
#define getNbBits_ligne56( b )	( nbBits[ ((b) >> 32) & LIGNE1ET2 ] ) 
#define getNbBits_ligne78( b )	( nbBits[  (b) >> 48 ] ) 

#define getDroiteContenantCases(c1,c2)	droiteContenantCases[c1][c2]
#define getZobrist( p , co , ca )		zobrist[p][co][ca]
#define getZobristEnPassant( ca )		zobristEnPassant[ ca ]
#define getZobristPhase( p )			zobristPhase[ p ]
#define getPionAcolyteBlanc( ca ) 		pionAcolyteBlanc[ ca ]
#define getPionAcolyteNoir( ca ) 		pionAcolyteNoir[ ca ]
#define getPionPasseBlanc( ca )		 	pionPasseBlanc[ ca ]
#define getPionPasseNoir( ca )		 	pionPasseNoir[ ca ]

	/*************/
	/* FONCTIONS */
	/*************/

void init( Plateau *p );
 
	/************/
	/* TABLEAUX */
	/************/

/* Ensemble de tableaux qui pour une piece donnee et une case donnee 
 * retourne le bitBoard de deplacement possible de la piece */
extern BitBoard deplacementTour[ 64 ];
extern BitBoard deplacementCavalier[ 64 ];
extern BitBoard deplacementFou[ 64 ];
extern BitBoard deplacementRoi[ 64 ];
extern BitBoard deplacementDame[ 64 ];

/* Deplacement possible d'un fou , d'une tour en fonction 
 * des pieces placees sur la ligne , la colonne ou la diagonale */
extern BitBoard deplacementFouA1H8[ 64 ][256];
extern BitBoard deplacementFouA8H1[ 64 ][256];
extern BitBoard deplacementTourColonne[ 64 ][256];
extern BitBoard deplacementTourLigne[ 64 ][256];
extern BitBoard deplacementPionCapture[ 64 ][ 2 ];

/* Retourne un pointeur sur les BitBoard(piece noires et blanches) correspondant 
 * à la piece passe en indice */
extern BitBoard* pieceToBitBoard[ 7 ];

/* Tableau qui donne le Flag d'une capture pour la piece passee en indice 
 * (c'est la piece capturee) */
extern Flag captureFlag[ 7 ];

extern short		decaleBitBoard45[64];
extern short		decaleBitBoard315[64];
extern short		decaleBitBoard90[64];
extern short		decaleBitBoardLigne[64];	// Nombre de decalage necessaire pour avoir à droite du bitboard la ligne contenant la case
												// exemple pour les cases a2 à h2 il faut faire un décalage de 8
												// exemple pour les cases a4 à h4 il faut faire un décalage de 24

/* Donne pour deux cases données les cases situées sur la même diagonale et entre les 2 cases 
   En excluant les 2 cases en question.
   Donne un BitBoard = 0 si les 2 cases ne sont pas sur une même diagonale.
*/
extern BitBoard		casesEntreDeuxCasesDiag[64][64];

/* Donne pour deux cases données les cases situées sur la même colonne,ligne et entre les 2 cases 
   En excluant les 2 cases en question.
   Donne un BitBoard = 0 si les 2 cases ne sont pas sur une même colone ou une même ligne.
*/
extern BitBoard		casesEntreDeuxCasesColLig[64][64];

/* Donne pour deux cases données les cases situées  entre les 2 cases 
   En excluant les 2 cases en question.
   Donne un BitBoard = 0 si les 2 cases ne sont pas sur une même colone ou une même ligne.
*/
extern BitBoard		casesEntreDeuxCases[64][64];

/* Donne le BitBoard representant la droite passant par les 2 cases passees en indice */
extern BitBoard 	droiteContenantCases[64][64];

/* Donne le BitBoard representant les cases situees  devant, une case 
 * à gauche , à droite d'elle à partir de ligne de dessus  
 * Permet de determiner rapidement les pions passes BLANC
 *
 * exemple :
 *         x x x
 *         x x x
 *         x x x
 *           o 
 * */
extern BitBoard 	pionPasseBlanc[64];

/* Donne le BitBoard representant les cases situees à l'arriere d'une case 
 * à gauche , à droite et en face d'elle à partir de ligne d'en dessous  
 * Permet de determiner rapidement les pions passes NOIR
 *
 * exemple :
 *
 *           o 
 *	       x x x
 *	       x x x
 *         x x x
 * */
extern BitBoard 	pionPasseNoir[64];

/* Donne le BitBoard representant les cases situees à l'arriere d'un pion (d'une case )
 * à gauche et à droite à partir de ligne du pion 
 * Permet de determiner rapidement les pions arrieres BLANC
 *
 * exemple :
 *
 *         x o x
 *	       x   x
 *	       x   x
 * */
extern BitBoard 	pionAcolyteBlanc[64];

/* Donne le BitBoard representant les cases situees devant un pion (d'une case )
 * à gauche et à droite à partir de ligne du pion 
 * Permet de determiner rapidement les pions arrieres NOIR
 *
 * exemple :
 *
 *     x   x
 *	   x   x
 *	   x o x
 * */
extern BitBoard 	pionAcolyteNoir[64];

/* Donne pour un nombre compris entre 0 et 65535 le nombre e bit a 1 qui le compose */
extern short nbBits[65536];

/* Utilise pour genere la cle de hachage d'une position */
extern int64 zobrist[7][2][64];
extern int64 zobristEnPassant[65];
extern int64 zobristBlanc;
extern int64 zobristPetitRoque[2];
extern int64 zobristGrandRoque[2];
extern int64 zobristPhase[3];

#endif /* INIT_H */
