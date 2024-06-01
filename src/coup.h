#ifndef COUP_H
#define COUP_H

//#include "plateau.h"
#include "case.h"
#include "boolean.h"
#include "type.h"

typedef int32 Coup;
typedef int32 Flag;

#define PAS_DE_COUP 0
#define COUP_NULL   0	//utilise pour le null move

#if 0

typedef struct {
	Flag coup;
	short score;
//	Case  origine;			
//	Case  destination;		
//	Boolean isEchec;		/* VRAI si le coup met le roi adversaire en echec */
//	Boolean isCapture;		/* VRAI si le coup capture une piece */
//	Boolean isPromotion; /* VRAI si le coup engendre une promotion */
//	Boolean isRoque;		/* VRAI si le coup correspond à un roque */
//	Boolean isEnPassant; /* VRAI si c'est une prise ne passant */
//	Boolean score;			/* indique le score affecte au coup par la fonction d'evaluation */
} Coup;

#endif
		/*************/
		/* FONCTION  */
		/*************/		
extern void printCoup( Coup , char *);

//extern Coup chaineToCoup( char *pco );

		/************/
		/* GETEUR  */
		/************/		
		
#define coup_getCaseOrigine( c )	( c & 0x3F )		/* 6 premeir bit */
#define coup_getCaseDestination( c )	(( c >> 6 )   &  0x3F )	/* de 7 a 12 */
#define coup_getPieceDeplace(c)		(( c >> 12 ) &  0x07 )	/* de 13 a 15 */
#define coup_getPieceCapture(c)		(( c >> 15 ) &  0x07 )	/* de 16 a 18 */
#define coup_getPiecePromotion(c)	(( c >> 18 ) &  0x07 )	/* de 19 a 21 */
#define coup_isCapture( c )		( c & 0x200000 )		/* 22eme bit */
#define coup_isRoque( c )		( c & 0x400000 )		/* 23eme bit */
#define coup_isEnPassant( c )		( c & 0x800000 )	/* 24eme bit */
#define coup_isEchec( c )		( c & 0x1000000 )		/* 25eme bit */
#define coup_isPromotion( c )		( c & 0x2000000 )	/* 26eme bit */
#define coup_empechePetitRoque( c )	( c & 0x4000000 )	/* 27eme bit */
#define coup_empecheGrandRoque( c )	( c & 0x8000000 )	/* 28eme bit */

#define coup_getCaseOriDest( c )	( c  &  0xFFF )		/* donne les 12 premiers bits
															 utilise par exemple comme index
															pour l(historyTable*/
/* #define coup_getScore( c )		c.score 
#define coup_getCoup( c )		c.coup*/
 

		/*********/
		/* FLAG  */
		/*********/
		
#define FLAG_TOUR	0x4000
#define FLAG_CAVALIER	0x2000
#define FLAG_FOU	0x3000
#define FLAG_DAME	0x5000
#define FLAG_ROI	0x6000
#define FLAG_ROI_ROQUE	0x406000
#define FLAG_PION	0x1000
#define FLAG_PION_PROMOTION_DAME	0x2141000
#define FLAG_PION_PROMOTION_TOUR 	0x2101000
#define FLAG_PION_PROMOTION_CAVALIER	0x2081000
#define FLAG_PION_PROMOTION_FOU		0x20C1000	
#define FLAG_PION_ENPASSANT		0xA09000
#define FLAG_CAPTURE_TOUR	0x220000	
#define FLAG_CAPTURE_CAVALIER	0x210000
#define FLAG_CAPTURE_FOU	0x218000
#define FLAG_CAPTURE_DAME	0x228000
#define FLAG_CAPTURE_PION	0x208000
#define FLAG_PAS_DE_CAPTURE	0x000000

#define FLAG_EMPECHE_PETIT_ROQUE	0x4000000
#define FLAG_EMPECHE_GRAND_ROQUE	0x8000000
#define FLAG_EMPECHE_PETIT_GRAND_ROQUE	0xC000000


#define coup_newCoup( o , d , f )	( (d<<6) | o | f)

#endif /* COUP_H */
