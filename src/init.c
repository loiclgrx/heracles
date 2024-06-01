#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include "case.h"  
#include "bitboard.h"
#include "init.h"
//#include "plateau.h"
#include "listecoup.h"
#include "coup.h"
#include "piece.h"
#include "util.h"
#include "multipv.h"

BitBoard deplacementTour[ 64 ];
BitBoard deplacementCavalier[ 64 ];
BitBoard deplacementFou[ 64 ];
BitBoard deplacementRoi[ 64 ];
BitBoard deplacementDame[ 64 ];

BitBoard deplacementTourLigne[ 64 ][256];
BitBoard deplacementTourColonne[ 64 ][256];
BitBoard deplacementFouA1H8[ 64 ][256];
BitBoard deplacementFouA8H1[ 64 ][256]; 
BitBoard deplacementPionCapture[ 64 ][ 2 ];	// Donne pour chaque case de l'echiquier et une couleur donnée les cases de capture d'un pion situé sur cette case

/* Donne le bitboard du plateau correspondant à la piece */
BitBoard* pieceToBitBoard[ 7 ];

/* pour un case donnee indique de combien il faut deplacer le BitBoard 45 pour obtenir la diagonae contenant la case */
short	decaleBitBoard45[64];
short	decaleBitBoard315[64];
short	decaleBitBoard90[64];
short	decaleBitBoardLigne[64];

Flag 	captureFlag[ 7 ];

BitBoard		casesEntreDeuxCasesDiag[64][64];
BitBoard		casesEntreDeuxCasesColLig[64][64];
BitBoard		casesEntreDeuxCases[64][64];
short 			nbBits[65536];

BitBoard 	pionAcolyteBlanc[64];
BitBoard 	pionAcolyteNoir[64];
BitBoard 	pionPasseBlanc[64];
BitBoard 	pionPasseNoir[64];

/* Donne le BitBoard representant la droite passant par les 2 cases passees en indice */
BitBoard 	droiteContenantCases[64][64];

/* Utilise pour genere la cle de hachage d'une position */
int64 zobrist[7][2][64];
int64 zobristEnPassant[65];
int64 zobristBlanc;
int64 zobristPetitRoque[2];
int64 zobristGrandRoque[2];
int64 zobristPhase[3];

void genereZobrist() {
	int p,co,ca;
	
    /* TODO : l'appel a rand64 ne fonctionne pas elle retourne un int sur 4 octets ??? */	
	
	for( p=0 ; p<7 ; p++ ) {
		for( co=NOIR ; co <= BLANC ; co++ ) {
			for( ca=A1; ca<=H8 ; ca++ ) {
				
				zobrist[p][co][ca] = rand() ^ ((int64)rand() << 15) ^
											  ((int64)rand() << 30) ^
											  ((int64)rand() << 45) ^
											  ((int64)rand() << 60);
				printf("zobrist[%d][%d][%d]=0x%X",p,co,ca,zobrist[p][co][ca]>>32);
				printf("%XL;\n",zobrist[p][co][ca] );
			}
		}
	}
	for( ca=A1; ca<=H8 ; ca++ ) {
		zobristEnPassant[ca] = rand() ^ ((int64)rand() << 15) ^
										((int64)rand() << 30) ^
										((int64)rand() << 45) ^
										((int64)rand() << 60);
				printf("zobristEnPassant[%d]=0x%X",ca,zobristEnPassant[ca]>>32);
				printf("%XL;\n",zobristEnPassant[ca] );
	}
																				
	zobristEnPassant[CASEVIDE]= rand() ^ ((int64)rand() << 15) ^
										((int64)rand() << 30) ^
										((int64)rand() << 45) ^
										((int64)rand() << 60);

  printf("zobristEnPassant[CASEVIDE]=0x%X",zobristEnPassant[CASEVIDE]>>32);
	printf("%XL;\n",zobristEnPassant[CASEVIDE] );

	zobristBlanc = rand() ^ ((int64)rand() << 15) ^
							((int64)rand() << 30) ^
							((int64)rand() << 45) ^
							((int64)rand() << 60);
							
  printf("zobristBlanc=0x%X",zobristBlanc>>32);
	printf("%XL;\n",zobristBlanc );
							
	zobristPetitRoque[BLANC] = rand() ^ ((int64)rand() << 15) ^
							((int64)rand() << 30) ^
							((int64)rand() << 45) ^
							((int64)rand() << 60);
							
  printf("zobristPetitRoque[BLANC]=0x%X",zobristPetitRoque[BLANC]>>32);
	printf("%XL;\n",zobristPetitRoque[BLANC] );

	zobristGrandRoque[BLANC] = rand() ^ ((int64)rand() << 15) ^
							((int64)rand() << 30) ^
							((int64)rand() << 45) ^
							((int64)rand() << 60);
							
  printf("zobristGrandRoque[BLANC]=0x%X",zobristGrandRoque[BLANC]>>32);
	printf("%XL;\n",zobristGrandRoque[BLANC] );

	zobristPetitRoque[NOIR] = rand() ^ ((int64)rand() << 15) ^
							((int64)rand() << 30) ^
							((int64)rand() << 45) ^
							((int64)rand() << 60);
							
  printf("zobristPetitRoque[NOIR]=0x%X",zobristPetitRoque[NOIR]>>32);
	printf("%XL;\n",zobristPetitRoque[NOIR] );

	zobristGrandRoque[NOIR] = rand() ^ ((int64)rand() << 15) ^
							((int64)rand() << 30) ^
							((int64)rand() << 45) ^
							((int64)rand() << 60);

  printf("zobristGrandRoque[NOIR]=0x%X",zobristGrandRoque[NOIR]>>32);
	printf("%XL;\n",zobristGrandRoque[NOIR] );
}

/*------------------------------------------------------------------------------------------*/

void initNbBits()  {
  int i,j,t;

  for( i=0 ; i<65536 ; i++ ) {
	  nbBits[i] = 0;
	  t=1;
	  for( j=0 ; j<16 ; j++ ) {  
		  if ( i &t )  nbBits[i]++; 
		  t = t << 1;
	  }
  }

}

/*------------------------------------------------------------------------------------------*/

void initDroiteContenantCases() {
	Case c1,c2,c;
	float vx,vy,vxc,vyc,kx;

	for( c1=A1 ; c1<=H8 ; c1++ ) {
		for( c2=A1 ; c2<=H8 ; c2++ ) {
		    droiteContenantCases[c1][c2] = 0;
		    vx = (c2%8) - (c1%8);
		    vy = (c2/8) - (c1/8);
			
			if ( (vx!=0) && (vy!=0) ) {
				if ( abs(vx) != abs(vy)  ) continue;
			} /* Les cases ne sont pas alignees */
			
		    for( c=A1 ; c<=H8 ; c++ ) {
		  	  vxc = (c2%8) - (c%8);
			  vyc = (c2/8) - (c/8);
			  //kx  = vxc / vx;
			  if ( (vyc*vx) == ( vxc*vy ) ) /* Vecteur colineaire */ 
				  droiteContenantCases[c1][c2] |= getCaseToBitBoard(c);
		    }
		
		}
	}
}
/*------------------------------------------------------------------------------------------*/

void initCasesEntreDeuxCasesDiag() {
	Case c1,c2,c,cMin,cMax;
	short deltaCol,deltaLig,deltaCase,pente;
	
	for( c1=A1 ; c1<=H8 ; c1++ ) {
		for( c2=A1 ; c2<=H8 ; c2++ ) {
			casesEntreDeuxCasesDiag[c1][c2] = 0;
			if (c1==c2) continue;
			deltaCol = abs( (c2%8) - (c1%8) );
			deltaLig = abs( (c2/8) - (c1/8) );
			deltaCase = abs( c2 - c1 );
			if ( deltaCol == deltaLig ) {	/* 2 cases sur la même diagonale */
				cMin = ( c1>c2 ) ? c2 : c1;
				cMax = ( c1>c2 ) ? c1 : c2;
				pente = ((deltaCase%9)==0) ? 9:7;
				c = cMin+pente;
				while ( c<cMax) {
					casesEntreDeuxCasesDiag[c1][c2] |= getCaseToBitBoard(c);
					c += pente;
				}
			}
		}
	}
}

/*------------------------------------------------------------------------------------------*/
void initcasesEntreDeuxCasesColLig() {
	Case c1,c2,c,cMin,cMax;
	short c1Col,c2Col,c1Lig,c2Lig,pente;

	for( c1=A1 ; c1<=H8 ; c1++ ) {
		for( c2=A1 ; c2<=H8 ; c2++ ) {	
			casesEntreDeuxCasesColLig[c1][c2] = 0;
			if (c1==c2) continue;
			c1Col = c1%8;
			c1Lig = c1/8;
			c2Col = c2%8;
			c2Lig = c2/8;
			if ( c1Col==c2Col || c1Lig==c2Lig ) { /* Sur la même colonne ou sur la même ligne */
				cMin = ( c1>c2 ) ? c2 : c1;
				cMax = ( c1>c2 ) ? c1 : c2;
				pente = ( c1Col==c2Col ) ? 8:1;
				c = cMin+pente;
				while ( c<cMax) {
					casesEntreDeuxCasesColLig[c1][c2] |= getCaseToBitBoard(c);
					c += pente;
				}
			}
			
		}
	}
}
/*------------------------------------------------------------------------------------------*/
void initCasesEntreDeuxCases() {
	Case c1,c2;

	for( c1=A1 ; c1<=H8 ; c1++ ) {
		for( c2=A1 ; c2<=H8 ; c2++ ) {	
		  casesEntreDeuxCases[c1][c2] = casesEntreDeuxCasesColLig[c1][c2] | 
			  			casesEntreDeuxCasesDiag[c1][c2]	;
		}
	}
}

/*------------------------------------------------------------------------------------------*/
void initFlagCapture() {
	captureFlag[PASDEPIECE] = FLAG_PAS_DE_CAPTURE;
	captureFlag[PION] = FLAG_CAPTURE_PION;
	captureFlag[TOUR] = FLAG_CAPTURE_TOUR;
	captureFlag[CAVALIER] = FLAG_CAPTURE_CAVALIER;
	captureFlag[FOU] = FLAG_CAPTURE_FOU;
	captureFlag[DAME] = FLAG_CAPTURE_DAME;
}
/*------------------------------------------------------------------------------------------*/


void initDeplacementTour() {
  Case i,j;
  int c,l;

  for ( i=A1 ; i<=H8 ; i++ ) {
	deplacementTour[ i ]  =0;
	c = i %8;
	l  = i / 8;
	for( j=A1 ; j<=H8 ; j++ ) {
		if ( ( j % 8 ) == c ) deplacementTour[ i ] |= caseToBitBoard[ j ];
		if ( ( j / 8 )   == l )  deplacementTour[ i ] |= caseToBitBoard[ j ];		
	}
	deplacementTour[ i ] &= complementCaseToBitBoard[ i ];
  }	
}

/*------------------------------------------------------------------------------------------*/

void initDeplacementCavalier() {
	Case i,d;
	
	for( i=A1 ; i<=H8 ; i++ ) {
		deplacementCavalier[ i ] = 0;
		d = i+10;
		if ( (d<64) && (d%8 > i%8) ) deplacementCavalier[ i ] |= caseToBitBoard[ d];
		d = i+17;		
		if ( (d<64) && (d%8 > i%8) ) deplacementCavalier[ i ] |= caseToBitBoard[ d];
		d = i-10;
		if ( (d>=0) && (d%8 < i%8) ) deplacementCavalier[ i ] |= caseToBitBoard[ d];
		d = i-17;		
		if ( (d>=0) && (d%8 < i%8) ) deplacementCavalier[ i ] |= caseToBitBoard[ d];
		d = i+6;
		if ( (d<64) && (d%8 < i%8) ) deplacementCavalier[ i ] |= caseToBitBoard[ d];
		d = i+15;
		if ( (d<64) && (d%8 < i%8) ) deplacementCavalier[ i ] |= caseToBitBoard[ d];
		d = i-6;
		if ( (d>=0) && (d%8 > i%8) ) deplacementCavalier[ i ] |= caseToBitBoard[ d];
		d = i-15;
		if ( (d>=0) && (d%8 > i%8) ) deplacementCavalier[ i ] |= caseToBitBoard[ d];
	}
}

/*------------------------------------------------------------------------------------------*/

void initDeplacementFou() {
	Case i,d;
	
	for ( i=A1 ; i<=H8 ; i++ ) {
		deplacementFou[ i ] = 0;
		d=i+9;
		while (d%8>i%8 && d<64) {
			deplacementFou[ i ] |= caseToBitBoard[ d];
			d=d+9;
		}
		d=i-7;
		while ( d >=0 && d%8>i%8 ) {
			deplacementFou[ i ] |= caseToBitBoard[ d];
			d=d-7;
		}
		d=i+7;
		while ( d <64 && d%8<i%8 ) {
			deplacementFou[ i ] |= caseToBitBoard[ d];
			d=d+7;
		}
		d=i-9;
		while ( d >=0 && d%8<i%8 ) {
			deplacementFou[ i ] |= caseToBitBoard[ d];
			d=d-9;
		}
	}
}

/*------------------------------------------------------------------------------------------*/

void initDeplacementRoi() {
	Case i,j;
	short deltax,deltay;
	

	for( i=A1 ; i<=H8 ; i++ ) {
		deplacementRoi[ i ]  = 0;
		for( j=A1 ; j<=H8 ; j++ ) {
			deltax = (i%8)-(j%8);
			deltay = (i/8)-(j/8);			
			if ( deltax>=-1 && deltax<=1 && deltay>=-1 && deltay<=1) 
				deplacementRoi[ i ] |= caseToBitBoard[ j ];
		}
		deplacementRoi[ i ] &= complementCaseToBitBoard[ i ];
	}

}

/*------------------------------------------------------------------------------------------*/

void initDeplacementDame() {
	Case i;
	
	for (i=A1 ; i<= H8 ; i++ ) {
		deplacementDame[i]=0;
		deplacementDame[i] |= deplacementTour[i];
		deplacementDame[i] |= deplacementFou[i];		
	}
}


/*------------------------------------------------------------------------------------------*/

void initDeplacementFouA1H8( ) {
	Case ca,ca45,ca45d;
	short co,d,i;
	unsigned char p=1;
	
	for( ca=0 ; ca< 64 ; ca++ ) {

		ca45 = case_getRotationCase45( ca );

		co	    = ca % 8;
		
		for ( d=0 ; d<256; d++ ) {

			deplacementFouA1H8[ca45] [d ] = 0;
			ca45d = ca;
			
			for(i=co+1;i<8;i++) {
				p = 1 << i;
				ca45d++;
						
		         if ( case_getRotationCase45(ca45d) - case_getRotationCase45(ca45d-1) == 9 ) 
			    		deplacementFouA1H8[ca45] [d ] |= getCaseToBitBoard( case_getRotationCase45(ca45d) );							
			    else
				break;
			
				if ( p & d ) break;
			}			

			ca45d = ca;

			for(i=co-1;i>=0;i--) {
				p = 1 << i;
				ca45d--;
			
		         if ( case_getRotationCase45(ca45d) - case_getRotationCase45(ca45d+1) == -9 ) 
			    		deplacementFouA1H8[ca45] [d ] |= getCaseToBitBoard( case_getRotationCase45(ca45d) );							
			    else
				break;
			
				if ( p & d ) break;
			}			
		}
	}
}


/*------------------------------------------------------------------------------------------*/

void initDeplacementFouA8H1( ) {
	Case ca,ca45,ca45d;
	short co,d,i;
	unsigned char p=1;
	
	for( ca=0 ; ca< 64 ; ca++ ) {

		ca45 = case_getRotationCase315( ca );

		co	    = ca % 8;
		
		for ( d=0 ; d<256; d++ ) {

			deplacementFouA8H1[ca45] [d ] = 0;
			ca45d = ca;
			
			for(i=co+1;i<8;i++) {
				p = 1 << i;
				ca45d++;
						
		         if ( case_getRotationCase315(ca45d) - case_getRotationCase315(ca45d-1) == -7 ) 
			    		deplacementFouA8H1[ca45] [d ] |= getCaseToBitBoard( case_getRotationCase315(ca45d) );							
			    else
				break;
			
				if ( p & d ) break;
			}			

			ca45d = ca;

			for(i=co-1;i>=0;i--) {
				p = 1 << i;
				ca45d--;
			
		         if ( case_getRotationCase315(ca45d) - case_getRotationCase315(ca45d+1) == 7 ) 
			    		deplacementFouA8H1[ca45] [d ] |= getCaseToBitBoard( case_getRotationCase315(ca45d) );							
			    else
				break;
			
				if ( p & d ) break;
			}			
		}
	}
}


/*------------------------------------------------------------------------------------------*/

void initDeplacementTourColonne( ) {
	Case ca,ca45,ca45d;
	short co,d,i;
	unsigned char p=1;
	
	for( ca=0 ; ca< 64 ; ca++ ) {

		ca45 = case_getRotationCase90( ca );

		co	    = 7-(ca % 8);
		
		for ( d=0 ; d<256; d++ ) {

			deplacementTourColonne[ca45] [d ] = 0;
	//		ca45d = ((ca/8)*8)+co;
			ca45d = ca;
			
			for(i=co+1;i<8;i++) {
				p = 1 << (7-i);
				ca45d--;
				
		//		printf("%s  %s  %d\n",	 getNomCase(getRotationCase90(ca45d))	,getNomCase(getRotationCase90(ca45d-1)),getRotationCase90(ca45d) - getRotationCase90(ca45d-1));
		         if ( case_getRotationCase90(ca45d+1) - case_getRotationCase90(ca45d) == 8 ) 
			    		deplacementTourColonne[ca45] [d ] |= getCaseToBitBoard( case_getRotationCase90(ca45d) );							
			    else
				break;
			
				if ( p & d ) break;
			}			

//			ca45d = ((ca/8)*8)+co;;
			ca45d = ca;

			for(i=co-1;i>=0;i--) {
				p = 1 << (7-i);
				ca45d++;
		//		printf("-%s  %s  %d\n",	 getNomCase(getRotationCase90(ca45d))	,getNomCase(getRotationCase90(ca45d+1)),getRotationCase90(ca45d) - getRotationCase90(ca45d+1));			
		         if ( case_getRotationCase90(ca45d-1) - case_getRotationCase90(ca45d) == -8 ) 
			    		deplacementTourColonne[ca45] [d ] |= getCaseToBitBoard( case_getRotationCase90(ca45d) );							
			    else
				break;
			
				if ( p & d ) break;
			}			
		}
	}
}



/*------------------------------------------------------------------------------------------*/

void initDeplacementTourLigne( ) {
	Case ca,ca45,ca45d;
	short co,d,i;
	unsigned char p=1;
	
	for( ca=0 ; ca< 64 ; ca++ ) {

		ca45 = ca;

		co	    = 7-(ca % 8);
		
		for ( d=0 ; d<256; d++ ) {

			deplacementTourLigne[ca45] [d ] = 0;
			ca45d = ca;
			
			for(i=co+1;i<8;i++) {
				p = 1 << (7-i);
				ca45d--;
				
		//		printf("%s  %s  %d\n",	 getNomCase(getRotationCase90(ca45d))	,getNomCase(getRotationCase90(ca45d-1)),getRotationCase90(ca45d) - getRotationCase90(ca45d-1));
		    		deplacementTourLigne[ca45] [d ] |= getCaseToBitBoard( ca45d );							
			
				if ( p & d ) break;
			}			

			ca45d = ca;

			for(i=co-1;i>=0;i--) {
				p = 1 << (7-i);
				ca45d++;
		//		printf("-%s  %s  %d\n",	 getNomCase(getRotationCase90(ca45d))	,getNomCase(getRotationCase90(ca45d+1)),getRotationCase90(ca45d) - getRotationCase90(ca45d+1));			
		    		deplacementTourLigne[ca45] [d ] |= getCaseToBitBoard( ca45d );							
			
				if ( p & d ) break;
			}			
		}
	}
}


/*------------------------------------------------------------------------------------------*/

void initDeplacementPionCapture() {
	Case ca;

	for ( ca=A8 ; ca<=H8 ; ca++ ) 
		deplacementPionCapture[ ca ][ BLANC ] = 0;
	
	for ( ca=A1 ; ca<=H1 ; ca++ ) 
		deplacementPionCapture[ ca ][ NOIR ] = 0;

	for ( ca=A1 ; ca<=H7 ; ca++ ) {
		deplacementPionCapture[ ca ][ BLANC ] = 0;
		if (( ca % 8 ) <7)
		  deplacementPionCapture[ ca ][ BLANC ] = getCaseToBitBoard( ca+9 );
		
		if (( ca % 8 ) > 0)
		  deplacementPionCapture[ ca ][ BLANC ] |= getCaseToBitBoard( ca+7 );		
	}	
	
	for ( ca=A2 ; ca<=H8 ; ca++ ) {
		deplacementPionCapture[ ca ][ NOIR ] = 0;
		if (( ca % 8 ) >0 )
		  deplacementPionCapture[ ca ][ NOIR ] = getCaseToBitBoard( ca-9 );
		
		if (( ca % 8 ) <7 )
		  deplacementPionCapture[ ca ][ NOIR ] |= getCaseToBitBoard( ca-7 );		
	}	

}
void initDecaleBitBoard45() {
	int i;
	
	for(i=0;i<64;i++) {
	//	printf("%s    i %d   decalage %d\n",case_getNom(case_getRotationCase45(i)),i,8*(i/8));
		decaleBitBoard45[ case_getRotationCase45(i) ] = 8*(i/8);
	}
}
/*------------------------------------------------------------------------------------------*/
void initDecaleBitBoard315() {
	int i;
	
	for(i=0;i<64;i++) {
		decaleBitBoard315[ case_getRotationCase315(i) ] = 8*(i/8);
	}	
}


/*------------------------------------------------------------------------------------------*/
void initDecaleBitBoard90() {
	int i;
	
	for(i=0;i<64;i++) {
		decaleBitBoard90[ case_getRotationCase90(i) ] = (i/8)*8 ;
	}	
}

/*------------------------------------------------------------------------------------------*/
void initDecaleBitBoardLigne() {
	int i;
	
	for(i=0;i<64;i++) {
		decaleBitBoardLigne[ i ] =((i>>3)<<3) ;	
	}	
}

/*------------------------------------------------------------------------------------------*/

void initPieceToBitBoard( Plateau *p ) {
	pieceToBitBoard[ PION ] = p->bbPion;
	pieceToBitBoard[ CAVALIER ] = p->bbCavalier;	
	pieceToBitBoard[ FOU ] = p->bbFou;	
	pieceToBitBoard[ TOUR ] = p->bbTour;	
	pieceToBitBoard[ DAME ] = p->bbDame;	
	pieceToBitBoard[ ROI ] = p->bbRoi;	
}

/*------------------------------------------------------------------------------------------*/
void initPionAcolyte( ) {
	Case ca;

	for ( ca = A1 ; ca <= H8 ; ca++ ) {
	  pionAcolyteBlanc[ ca ] = 0;
	  pionAcolyteNoir[ ca ] = 0;

	  if ( ( ca % 8 ) > 0 ) {
	    pionAcolyteBlanc[ ca ] |= getCasesEntreDeuxCases(ca-1 , (ca-1)%8 );
	    pionAcolyteNoir[ ca ]  |= getCasesEntreDeuxCases(ca-1 , ( (ca-1) %8 ) + 56 );

	    pionAcolyteBlanc[ ca ] |= caseToBitBoard[ ca -1 ] | caseToBitBoard[(ca-1)%8];
	    pionAcolyteNoir [ ca ] |= caseToBitBoard[ ca -1 ] | caseToBitBoard[( (ca-1) %8 ) + 56 ];
	  }
	  

	  if ( (ca % 8) < 7 ) {
	    pionAcolyteBlanc[ ca ] |= getCasesEntreDeuxCases(ca+1 , (ca+1)%8 );
	    pionAcolyteNoir[ ca ]  |= getCasesEntreDeuxCases(ca+1 , ( (ca+1) %8 ) + 56 );

	    pionAcolyteBlanc[ ca ] |= caseToBitBoard[ ca +1 ] | caseToBitBoard[(ca+1)%8];
	    pionAcolyteNoir [ ca ] |= caseToBitBoard[ ca +1 ] | caseToBitBoard[( (ca+1) %8 ) + 56 ];
	  }
		
	}

}
/*------------------------------------------------------------------------------------------*/
void initPionPasse() {
  Case ca;

  for ( ca = A2 ; ca <= H7 ; ca++ ) {
	  pionPasseBlanc[ ca ] = 0;
	  pionPasseNoir[ ca ] = 0;

	  if ( ca <= H1 || ca >H7 ) continue;

	  if ( ( ca % 8 ) > 0 ) {
	    pionPasseBlanc[ ca ] |= getCasesEntreDeuxCases(ca , ( ca%8 ) +56 );
	    pionPasseNoir[ ca ]  |= getCasesEntreDeuxCases(ca , ca % 8  );

	    pionPasseBlanc[ ca ] |= getCasesEntreDeuxCases(ca - 1 , ((ca-1)%8)+56 );
	    pionPasseNoir[ ca ]  |= getCasesEntreDeuxCases(ca - 1 , (ca-1) %8  );

	    pionPasseBlanc[ ca ] |= caseToBitBoard[( ca%8 ) +56   ] | caseToBitBoard[((ca-1)%8)+56];
	    pionPasseNoir [ ca ] |= caseToBitBoard[ ca % 8 ] | caseToBitBoard[(ca-1) %8 ];
	  }
	  

	  if ( (ca % 8) < 7 ) {
	    pionPasseBlanc[ ca ] |= getCasesEntreDeuxCases(ca , ( ca%8 ) + 56 );
	    pionPasseNoir[ ca ]  |= getCasesEntreDeuxCases(ca , ca%8 );

	    pionPasseBlanc[ ca ] |= getCasesEntreDeuxCases(ca+1 , ( (ca+1) %8 ) + 56 );
	    pionPasseNoir[ ca ]  |= getCasesEntreDeuxCases(ca+1 , (ca+1)%8 );

	    pionPasseBlanc[ ca ] |= caseToBitBoard[ ( ca%8 ) + 56 ] | 
		    		    caseToBitBoard[( (ca+1) %8 ) + 56];
	    pionPasseNoir [ ca ] |= caseToBitBoard[ ca%8  ] | caseToBitBoard[ (ca+1)%8 ];
	  }
  }
}


void initZobrist() {
zobrist[0][0][0]=0x66BA34B963AEAF9DLL;
zobrist[0][0][1]=0x15F6882E2B5BA718LL;
zobrist[0][0][2]=0x40193B50D8A4A371LL;
zobrist[0][0][3]=0x1485E902B7732CB0LL;
zobrist[0][0][4]=0xE0D3D3FE4EA712ACLL;
zobrist[0][0][5]=0x529C9294B7FEE3EFLL;
zobrist[0][0][6]=0x13A7066DB519EB41LL;
zobrist[0][0][7]=0xCBFBBD06A68CBBE2LL;
zobrist[0][0][8]=0x928FDD91E8A0B8D7LL;
zobrist[0][0][9]=0xF9FFE7471327C2A4LL;
zobrist[0][0][10]=0x1A6C96423C55A254LL;
zobrist[0][0][11]=0x844D8A8BF90694E9LL;
zobrist[0][0][12]=0x75C75D2A6317B95ELL;
zobrist[0][0][13]=0xE7764D86E94EB317LL;
zobrist[0][0][14]=0xF81A4467DC9FAB0ALL;
zobrist[0][0][15]=0x5D727E2B8B5B4579LL;
zobrist[0][0][16]=0xBD39C786FABADCDLL;
zobrist[0][0][17]=0xC8CF8310990DA3FELL;
zobrist[0][0][18]=0xE6F86598FD7D93CALL;
zobrist[0][0][19]=0x437B00E763C53A5BLL;
zobrist[0][0][20]=0x868F164AAA2462C7LL;
zobrist[0][0][21]=0x78686062EC7A4630LL;
zobrist[0][0][22]=0xDCC556E796A5E88FLL;
zobrist[0][0][23]=0x664E882B19736CA5LL;
zobrist[0][0][24]=0x9FB0999E2CB732ACLL;
zobrist[0][0][25]=0xD140DFA4CDFAE869LL;
zobrist[0][0][26]=0x2ECEC4ACD1E5F8D0LL;
zobrist[0][0][27]=0xD0EA6FC99E7133CELL;
zobrist[0][0][28]=0x82EC37196E57D5AALL;
zobrist[0][0][29]=0x2E9A55086737A49ALL;
zobrist[0][0][30]=0xD3C7061A467979C5LL;
zobrist[0][0][31]=0x67CE259EB780772LL;
zobrist[0][0][32]=0x440C93A8D816CD8LL;
zobrist[0][0][33]=0x5BCB69D45038952BLL;
zobrist[0][0][34]=0xE6D9F8A06DD06F22LL;
zobrist[0][0][35]=0x3E8F06FB30638D55LL;
zobrist[0][0][36]=0xD04AEAB1A01344A5LL;
zobrist[0][0][37]=0x1A7CF91B79F98EC7LL;
zobrist[0][0][38]=0xDE71B1A75138B423LL;
zobrist[0][0][39]=0x2723582185977EC6LL;
zobrist[0][0][40]=0x6C2D3BA1A822F4ACLL;
zobrist[0][0][41]=0x92E4C1B751E8ECA2LL;
zobrist[0][0][42]=0xBD3F2550C3C47661LL;
zobrist[0][0][43]=0x548D3F07A20D5191LL;
zobrist[0][0][44]=0xAA79DA458CC1C2FLL;
zobrist[0][0][45]=0xD35BE91EFD148B88LL;
zobrist[0][0][46]=0x182D1DC6A5878593LL;
zobrist[0][0][47]=0x790AE137AE1EEF8ALL;
zobrist[0][0][48]=0x84DDC314EC28398ELL;
zobrist[0][0][49]=0x8944F2FBAE738FC5LL;
zobrist[0][0][50]=0x4259C44DE73CC3ADLL;
zobrist[0][0][51]=0x1E3C2181CC4BB8DCLL;
zobrist[0][0][52]=0xCFEFA1D83CDDCC3LL;
zobrist[0][0][53]=0x43A9098B8B3EAD42LL;
zobrist[0][0][54]=0x82E0A5B1612FE08FLL;
zobrist[0][0][55]=0x3A2E761640ED8D07LL;
zobrist[0][0][56]=0xE8DA069FA4FCD49LL;
zobrist[0][0][57]=0x310FC2B9AA06B528LL;
zobrist[0][0][58]=0xE0ECC0B5F0D4CCE0LL;
zobrist[0][0][59]=0x8340B6D3D7485B21LL;
zobrist[0][0][60]=0x8D8E59A6407FD746LL;
zobrist[0][0][61]=0x41DD02DCB87848B8LL;
zobrist[0][0][62]=0x8D587F257F141439LL;
zobrist[0][0][63]=0xA66DCB18A44D57A2LL;
zobrist[0][1][0]=0x97F8C79C2BB0FC9FLL;
zobrist[0][1][1]=0x5722D0629437DCF5LL;
zobrist[0][1][2]=0xAE7DE0C83066C7A7LL;
zobrist[0][1][3]=0x7E37BD4E8FAF6BB5LL;
zobrist[0][1][4]=0x404AF31650D8E13DLL;
zobrist[0][1][5]=0xC42000C1948FE9D4LL;
zobrist[0][1][6]=0xEC487A3F7E8BD963LL;
zobrist[0][1][7]=0xAF37DB3760D00C78LL;
zobrist[0][1][8]=0x1D2D3E6E3B650D39LL;
zobrist[0][1][9]=0x5CE586E6101054EDLL;
zobrist[0][1][10]=0x1E02FCEBFB342FE7LL;
zobrist[0][1][11]=0x7FCC364D45CE2911LL;
zobrist[0][1][12]=0x8B758627E52A7CD6LL;
zobrist[0][1][13]=0x3092AFE391CB5BCLL;
zobrist[0][1][14]=0x6B37126B49782855LL;
zobrist[0][1][15]=0x7B967F7A4D04DB01LL;
zobrist[0][1][16]=0x65526646901C2228LL;
zobrist[0][1][17]=0xC04437EC7ECB7AD9LL;
zobrist[0][1][18]=0x32E833E27EBC9487LL;
zobrist[0][1][19]=0x392324BD67DE8B2FLL;
zobrist[0][1][20]=0x199EE38430CD6A4LL;
zobrist[0][1][21]=0x4C2EBEB99BEEC3E1LL;
zobrist[0][1][22]=0x21CB59311B688A85LL;
zobrist[0][1][23]=0x192C8272EF69DA7BLL;
zobrist[0][1][24]=0xDAFE51BEF1827126LL;
zobrist[0][1][25]=0xF184F1239689ADD3LL;
zobrist[0][1][26]=0x19A84E84640828EDLL;
zobrist[0][1][27]=0x2995275AAC92CA05LL;
zobrist[0][1][28]=0x566E84196156EECALL;
zobrist[0][1][29]=0xC6A722BF715C3B56LL;
zobrist[0][1][30]=0x670E24B4EEB13C83LL;
zobrist[0][1][31]=0x96C0CF31CD9673LL;
zobrist[0][1][32]=0x2FE033B82F2005FALL;
zobrist[0][1][33]=0x8504D3E768E01A82LL;
zobrist[0][1][34]=0x7CB7554F27F12233LL;
zobrist[0][1][35]=0xB39CCBC62CD00DEELL;
zobrist[0][1][36]=0x7CCE0A496B4972AFLL;
zobrist[0][1][37]=0x799DFEA2DB515A07LL;
zobrist[0][1][38]=0x1DDB4FC145C0AE17LL;
zobrist[0][1][39]=0x7A410301353BCCDLL;
zobrist[0][1][40]=0x372DB3BBBC643EF2LL;
zobrist[0][1][41]=0x3A533BBB2A2D0C7BLL;
zobrist[0][1][42]=0xE7BB676BAA3CB990LL;
zobrist[0][1][43]=0xF7EE7B6AF25CB000LL;
zobrist[0][1][44]=0xB8143F1EBFBA6FE4LL;
zobrist[0][1][45]=0x2F7B6998C1241EDLL;
zobrist[0][1][46]=0xE2E8651D17267601LL;
zobrist[0][1][47]=0xFCD890D69A6AED0ELL;
zobrist[0][1][48]=0x13F41B2F4E3C3D2BLL;
zobrist[0][1][49]=0xA3EA71EA4164065ALL;
zobrist[0][1][50]=0xA360567E995EDA33LL;
zobrist[0][1][51]=0x57630954A91C6A45LL;
zobrist[0][1][52]=0x9D50BE02715BA9B0LL;
zobrist[0][1][53]=0x96AE121C929E6916LL;
zobrist[0][1][54]=0xBFEC3395990DD12CLL;
zobrist[0][1][55]=0x7C93038494DA5BEFLL;
zobrist[0][1][56]=0xAC9A7E3D637F38C3LL;
zobrist[0][1][57]=0x168A00067BD4950LL;
zobrist[0][1][58]=0xEEC48B8A1EAB3297LL;
zobrist[0][1][59]=0xB447F8E383C708EELL;
zobrist[0][1][60]=0x97B618FC68E5F661LL;
zobrist[0][1][61]=0xA284BFA767592E64LL;
zobrist[0][1][62]=0x99ED57B5245AD502LL;
zobrist[0][1][63]=0xBCBBBC3393A08B0ALL;
zobrist[1][0][0]=0x21614A332380F813LL;
zobrist[1][0][1]=0x88E25E189EC33638LL;
zobrist[1][0][2]=0x7CEB09FC10D8235FLL;
zobrist[1][0][3]=0x490D64D32697AD47LL;
zobrist[1][0][4]=0x464C421820FAABD4LL;
zobrist[1][0][5]=0xAD994888C559553ELL;
zobrist[1][0][6]=0x7CDC0EDF17D601AELL;
zobrist[1][0][7]=0x2EB7F8C21691EF1LL;
zobrist[1][0][8]=0x6B753A2C79D966C1LL;
zobrist[1][0][9]=0xF8B3F17DF8A19A3DLL;
zobrist[1][0][10]=0xB33F12E5E03A67C2LL;
zobrist[1][0][11]=0xED008213E1E06212LL;
zobrist[1][0][12]=0xB0D8F32F1CD76E85LL;
zobrist[1][0][13]=0x56C6CAD64B78E05DLL;
zobrist[1][0][14]=0x854300F716F7E69ELL;
zobrist[1][0][15]=0x3A27A02A17AF741LL;
zobrist[1][0][16]=0x79D920A69B0E4ADFLL;
zobrist[1][0][17]=0xFFB85A053D939F2CLL;
zobrist[1][0][18]=0x6A77B7332187928BLL;
zobrist[1][0][19]=0x850DB834276BE750LL;
zobrist[1][0][20]=0xA5830C0263E3CB93LL;
zobrist[1][0][21]=0xD004A9BFC6A84750LL;
zobrist[1][0][22]=0xE5156BEC3C4A478BLL;
zobrist[1][0][23]=0x85F125C66E2F5F68LL;
zobrist[1][0][24]=0xDCE69A79DB47158LL;
zobrist[1][0][25]=0x6254CD9A1D3CBE9ALL;
zobrist[1][0][26]=0xAEC2BD3B87E326E3LL;
zobrist[1][0][27]=0xFE7C4AA5EBF36D62LL;
zobrist[1][0][28]=0x2E261B994BE73B23LL;
zobrist[1][0][29]=0x499BB669DCBD6D5LL;
zobrist[1][0][30]=0x97ACE9A2795FA02LL;
zobrist[1][0][31]=0xBEE5AE34BBB96CF8LL;
zobrist[1][0][32]=0x9B1ABE994B814855LL;
zobrist[1][0][33]=0xEC0F9F2740FD044LL;
zobrist[1][0][34]=0xFBF4F97A86E14140LL;
zobrist[1][0][35]=0x9CA55FB9DD217CCDLL;
zobrist[1][0][36]=0x29827B1B18D4AA3ALL;
zobrist[1][0][37]=0x309A1D2934BFABCFLL;
zobrist[1][0][38]=0xB07D83DF755836CCLL;
zobrist[1][0][39]=0x44D63E9ACFF97442LL;
zobrist[1][0][40]=0x8D807DA62B7852A0LL;
zobrist[1][0][41]=0x60CA02E9F5F9D794LL;
zobrist[1][0][42]=0xC6CB852A481D4EBFLL;
zobrist[1][0][43]=0x6D8849F7D0181330LL;
zobrist[1][0][44]=0x9DB60387B2FEABECLL;
zobrist[1][0][45]=0x90DADE7DDEBBFA14LL;
zobrist[1][0][46]=0xACA1F862243E2DC6LL;
zobrist[1][0][47]=0x11A524F194200258LL;
zobrist[1][0][48]=0x3EE2AC8FCDC2A661LL;
zobrist[1][0][49]=0x559FDF688F4FC95ALL;
zobrist[1][0][50]=0x6257B6CE2695CE73LL;
zobrist[1][0][51]=0x9ECA999B971AE3E5LL;
zobrist[1][0][52]=0xCA7FBDD7D96B191LL;
zobrist[1][0][53]=0xBE70729F267E9585LL;
zobrist[1][0][54]=0x657760C429E5E14LL;
zobrist[1][0][55]=0x580CFE9DC5CA385ELL;
zobrist[1][0][56]=0xD5FDCD35A00D73F9LL;
zobrist[1][0][57]=0xF72DD532D69DCF58LL;
zobrist[1][0][58]=0x265C63885AA1066LL;
zobrist[1][0][59]=0xAB8D6A88E5328778LL;
zobrist[1][0][60]=0x39A3916B91E429BALL;
zobrist[1][0][61]=0x65E7F56D688EB5E8LL;
zobrist[1][0][62]=0x243694E33879399BLL;
zobrist[1][0][63]=0x65A05965B6621426LL;
zobrist[1][1][0]=0xC4416652A9D1BD88LL;
zobrist[1][1][1]=0x1F03AE2E4C1F7F69LL;
zobrist[1][1][2]=0x518CECD667FC4E8ALL;
zobrist[1][1][3]=0xCA0011DC92406383LL;
zobrist[1][1][4]=0xE5864050EE3B95B3LL;
zobrist[1][1][5]=0xE2AD9639BD4E56C1LL;
zobrist[1][1][6]=0xDB5D6D0AD39B8486LL;
zobrist[1][1][7]=0x4EB5884ACBE99F11LL;
zobrist[1][1][8]=0x8D10585F29B08758LL;
zobrist[1][1][9]=0x95CA9746AFDA0F5LL;
zobrist[1][1][10]=0xD8BE20FDE881C881LL;
zobrist[1][1][11]=0xF6D63FEA3E5EC4DCLL;
zobrist[1][1][12]=0x286E1BB9BF27EC49LL;
zobrist[1][1][13]=0x2AF3F77E5E25BB4LL;
zobrist[1][1][14]=0x1E982C34E0005AE2LL;
zobrist[1][1][15]=0x86386BB2CDA2B6B6LL;
zobrist[1][1][16]=0xD650ADA6EBE3B56LL;
zobrist[1][1][17]=0x285345D38878BA9DLL;
zobrist[1][1][18]=0x1C8FAB2B8C036E6LL;
zobrist[1][1][19]=0xDF2A126D2C4EDBD6LL;
zobrist[1][1][20]=0xAB62A17936F288E6LL;
zobrist[1][1][21]=0xB9C78A9862630439LL;
zobrist[1][1][22]=0xA120B575A7F7F67LL;
zobrist[1][1][23]=0x6E1875A4D729F8F5LL;
zobrist[1][1][24]=0x837638D9ECD548DLL;
zobrist[1][1][25]=0x7E1B5352F942524BLL;
zobrist[1][1][26]=0x16A684EF2312DD72LL;
zobrist[1][1][27]=0xC4594C632B640FDFLL;
zobrist[1][1][28]=0xEE3D20FB26F1C645LL;
zobrist[1][1][29]=0x2C8919902BEB54D9LL;
zobrist[1][1][30]=0xB07542436AEA461CLL;
zobrist[1][1][31]=0x39DC4137C777F316LL;
zobrist[1][1][32]=0x29CCADC0391B4633LL;
zobrist[1][1][33]=0x7854A44A89865CA1LL;
zobrist[1][1][34]=0xE08CC96E1D51BEF3LL;
zobrist[1][1][35]=0x1EDDA23513242849LL;
zobrist[1][1][36]=0xA65BBD333D8F6F86LL;
zobrist[1][1][37]=0x9ABE8E14CFCF127ALL;
zobrist[1][1][38]=0x98B3E2968F0E4163LL;
zobrist[1][1][39]=0xED477575C22098BLL;
zobrist[1][1][40]=0xD69660403933E1CELL;
zobrist[1][1][41]=0xA8781FE74356B862LL;
zobrist[1][1][42]=0x9C74CED62CF3B164LL;
zobrist[1][1][43]=0x1D20AEA04465F615LL;
zobrist[1][1][44]=0x2AD86AFB4B390127LL;
zobrist[1][1][45]=0x9F3E0C49EE4DC7A9LL;
zobrist[1][1][46]=0x62D9A553687A42DFLL;
zobrist[1][1][47]=0x144482C97FE826F6LL;
zobrist[1][1][48]=0x5DFFD6642657B4BCLL;
zobrist[1][1][49]=0xD68155D462934150LL;
zobrist[1][1][50]=0x63A394E0B1885622LL;
zobrist[1][1][51]=0x1253DF2ED99052FALL;
zobrist[1][1][52]=0x35FFA6C892ECFB08LL;
zobrist[1][1][53]=0x40A14A38E9B1A4DFLL;
zobrist[1][1][54]=0x2DC7F01EF9103437LL;
zobrist[1][1][55]=0xC437A56A8EB0957LL;
zobrist[1][1][56]=0x1CB821B89DD64D3FLL;
zobrist[1][1][57]=0x6FB0A9885073DDFFLL;
zobrist[1][1][58]=0x18E63CE4ED83AE30LL;
zobrist[1][1][59]=0x8CB40DFC3AEE80C6LL;
zobrist[1][1][60]=0xD4C668F2327F5CCELL;
zobrist[1][1][61]=0xABB08F80D8823454LL;
zobrist[1][1][62]=0x9D0A55D95BEC3F82LL;
zobrist[1][1][63]=0xBCF740975B20061LL;
zobrist[2][0][0]=0xA629057915073217LL;
zobrist[2][0][1]=0x91B3BA1CBF2DBB9BLL;
zobrist[2][0][2]=0xFDAC5EDE368947F8LL;
zobrist[2][0][3]=0x7DA5DFB910C72A93LL;
zobrist[2][0][4]=0xE111239E654C4CD3LL;
zobrist[2][0][5]=0x20B8FA85EA53113DLL;
zobrist[2][0][6]=0x574AF20E578247A9LL;
zobrist[2][0][7]=0x1EAE1CFD527A177BLL;
zobrist[2][0][8]=0x4C61DCB9C7EA47F2LL;
zobrist[2][0][9]=0xD22F16C04731CLL;
zobrist[2][0][10]=0x6704432628BBFB0ELL;
zobrist[2][0][11]=0x1132399347C3DB88LL;
zobrist[2][0][12]=0x84900A9137B33CCCLL;
zobrist[2][0][13]=0xB8282E161508E92LL;
zobrist[2][0][14]=0xBAEFC4E93A4192D2LL;
zobrist[2][0][15]=0xB416FBA04E78CECALL;
zobrist[2][0][16]=0x12C7EBC2E3310C80LL;
zobrist[2][0][17]=0xA4EDA6705C2428A1LL;
zobrist[2][0][18]=0xCE18CD8E84C7DF3ELL;
zobrist[2][0][19]=0x50BB4DFDCDE5377DLL;
zobrist[2][0][20]=0x29EB5273322DF2E6LL;
zobrist[2][0][21]=0xE59957345370440CLL;
zobrist[2][0][22]=0x256B93D47335DE7DLL;
zobrist[2][0][23]=0xA209CBEECE39B160LL;
zobrist[2][0][24]=0xB5C770D883662A33LL;
zobrist[2][0][25]=0xBA137CF044B707D3LL;
zobrist[2][0][26]=0x83D73DF3B478D736LL;
zobrist[2][0][27]=0x5A6C54A22EBAFA8DLL;
zobrist[2][0][28]=0x11D3E50248E60497LL;
zobrist[2][0][29]=0xEE987E50AB04A975LL;
zobrist[2][0][30]=0xB7505118C8529E87LL;
zobrist[2][0][31]=0x3B4F655EB1CE89C4LL;
zobrist[2][0][32]=0x4327EB8F29C9CD82LL;
zobrist[2][0][33]=0x436DCB12822ACC1FLL;
zobrist[2][0][34]=0x5EC5AFD230C23CA4LL;
zobrist[2][0][35]=0x6B235941ED25D787LL;
zobrist[2][0][36]=0xE3B072278A5DEB31LL;
zobrist[2][0][37]=0x6590DDFB51D52698LL;
zobrist[2][0][38]=0x71AD8AAE8DDAAEC7LL;
zobrist[2][0][39]=0x654296569DDB980BLL;
zobrist[2][0][40]=0x341FCF44B81C8202LL;
zobrist[2][0][41]=0x2962E51FABF33D66LL;
zobrist[2][0][42]=0xCEB2C92DD2277212LL;
zobrist[2][0][43]=0x8C90CB8C460A8D17LL;
zobrist[2][0][44]=0xA8F11741A19394C6LL;
zobrist[2][0][45]=0x9487632578D2C969LL;
zobrist[2][0][46]=0xADA54E9D9C05425FLL;
zobrist[2][0][47]=0x7B1D6B801EB6C8B3LL;
zobrist[2][0][48]=0x1E774AA5159431BALL;
zobrist[2][0][49]=0xA07E31B5FD506C92LL;
zobrist[2][0][50]=0xAAF61C94AB397B85LL;
zobrist[2][0][51]=0xB35B21C5A5704FF6LL;
zobrist[2][0][52]=0xF04A62B477ED90B3LL;
zobrist[2][0][53]=0xC590C07576F5319LL;
zobrist[2][0][54]=0xB2CA07E29AC802FLL;
zobrist[2][0][55]=0xA012A6FBA29FE0C5LL;
zobrist[2][0][56]=0xE0733CCD835F1501LL;
zobrist[2][0][57]=0x67275D07CF15E20LL;
zobrist[2][0][58]=0xA269FB354B4FB3F8LL;
zobrist[2][0][59]=0x7664D09856057542LL;
zobrist[2][0][60]=0x5D67CC2960AC4698LL;
zobrist[2][0][61]=0xF68AB50A316A9ELL;
zobrist[2][0][62]=0xCBAC64C238C61ECELL;
zobrist[2][0][63]=0x2A057C19B9FE18CLL;
zobrist[2][1][0]=0x2AC2BB36CD582F92LL;
zobrist[2][1][1]=0x455C11891ABE8594LL;
zobrist[2][1][2]=0x67F738E549AB5C69LL;
zobrist[2][1][3]=0x6939B830C33665F9LL;
zobrist[2][1][4]=0xDFA514D9140170FDLL;
zobrist[2][1][5]=0xE5CBF32E32EA094DLL;
zobrist[2][1][6]=0x3574D03EF4EEEEAALL;
zobrist[2][1][7]=0x2D33EB5D93FDE95LL;
zobrist[2][1][8]=0x32B31F22CC0C7EDALL;
zobrist[2][1][9]=0x3C220AB280EF5D98LL;
zobrist[2][1][10]=0x9BB1C8243D904ECFLL;
zobrist[2][1][11]=0x8C2080AF65A52B83LL;
zobrist[2][1][12]=0xE3B9BAC9B5D243B1LL;
zobrist[2][1][13]=0x78D1688A3FACAD8ELL;
zobrist[2][1][14]=0x655428799BC1396CLL;
zobrist[2][1][15]=0xC79180F197591C63LL;
zobrist[2][1][16]=0x438F4519961B1240LL;
zobrist[2][1][17]=0x7490D5938FC93DDDLL;
zobrist[2][1][18]=0x7B9C87335F8A4EALL;
zobrist[2][1][19]=0x58615114B6AD25D4LL;
zobrist[2][1][20]=0xCF9E63F051373C56LL;
zobrist[2][1][21]=0xE023338275478324LL;
zobrist[2][1][22]=0x7E2C36C1E2A179B2LL;
zobrist[2][1][23]=0x4BADD675DA6E957LL;
zobrist[2][1][24]=0xE0F0BF53E70A9613LL;
zobrist[2][1][25]=0xCCBB36C817EC3570LL;
zobrist[2][1][26]=0xE45D04E11597AF6LL;
zobrist[2][1][27]=0xD191EF37FD6638F0LL;
zobrist[2][1][28]=0x72F889649D67DBB0LL;
zobrist[2][1][29]=0xB3B72765C41A04B8LL;
zobrist[2][1][30]=0x681D86539949304ALL;
zobrist[2][1][31]=0x4191ED23FA4528ALL;
zobrist[2][1][32]=0x77CF16483B92B5D1LL;
zobrist[2][1][33]=0xE11E5FFC7C6F4596LL;
zobrist[2][1][34]=0x871B2EC02B492F05LL;
zobrist[2][1][35]=0x4B73F3DEFE7AA3FBLL;
zobrist[2][1][36]=0x22A1D72A837EA809LL;
zobrist[2][1][37]=0x447EF8C615A3A6LL;
zobrist[2][1][38]=0x5E951D23811BFF35LL;
zobrist[2][1][39]=0xCC7983281873AA65LL;
zobrist[2][1][40]=0xC7EA7BA3FF832B06LL;
zobrist[2][1][41]=0x5BA00F41A2817465LL;
zobrist[2][1][42]=0xD7C82FBCCD98589CLL;
zobrist[2][1][43]=0xC088A31BB7791410LL;
zobrist[2][1][44]=0x4F0ED3967DDE86D5LL;
zobrist[2][1][45]=0xE4F9A97F38345160LL;
zobrist[2][1][46]=0x1D961B7C65F9086BLL;
zobrist[2][1][47]=0x10624E3899D1D82CLL;
zobrist[2][1][48]=0xDB3D6C642B069A45LL;
zobrist[2][1][49]=0x8F060550671BB7BLL;
zobrist[2][1][50]=0x9FD89A0364A6B4DLL;
zobrist[2][1][51]=0xC0089B63E2F0B6F7LL;
zobrist[2][1][52]=0xB1A6F375AE8EB4C4LL;
zobrist[2][1][53]=0x632CCD9FEF95D302LL;
zobrist[2][1][54]=0x8202349D3CB2905ELL;
zobrist[2][1][55]=0xB224370B722EBEBFLL;
zobrist[2][1][56]=0x368932421E02701ELL;
zobrist[2][1][57]=0xC2CA7C4758D1D6E6LL;
zobrist[2][1][58]=0x1634790642952712LL;
zobrist[2][1][59]=0x7C68566981C88B9LL;
zobrist[2][1][60]=0x4084DB730901C4ELL;
zobrist[2][1][61]=0xD2475550D33279C2LL;
zobrist[2][1][62]=0xB9584AD03F195683LL;
zobrist[2][1][63]=0x5F7318C25830A45CLL;
zobrist[3][0][0]=0x919931CB9D802C3LL;
zobrist[3][0][1]=0x12A3A72B91803FDDLL;
zobrist[3][0][2]=0xE04E4B48A1A9AAC0LL;
zobrist[3][0][3]=0xCD96A0DD6006A83FLL;
zobrist[3][0][4]=0xA9A12A0515F3AAC7LL;
zobrist[3][0][5]=0x4BEBD0128E4A3B57LL;
zobrist[3][0][6]=0xC84B082B6089B5B4LL;
zobrist[3][0][7]=0x1F15DB175B0F4EF6LL;
zobrist[3][0][8]=0x3FE1194E5B692BLL;
zobrist[3][0][9]=0x4F1AD8ABA2A05B2ALL;
zobrist[3][0][10]=0xB316A52CBFC89990LL;
zobrist[3][0][11]=0x9A84A599AAF69CCLL;
zobrist[3][0][12]=0x1CFBF9C023BFB839LL;
zobrist[3][0][13]=0xB2CCDF117CD8D882LL;
zobrist[3][0][14]=0xA79B57608DEF8DD0LL;
zobrist[3][0][15]=0x4C73D8D8E86B30CLL;
zobrist[3][0][16]=0xEF1720AC3D63EB12LL;
zobrist[3][0][17]=0xBB22316F1A1396D2LL;
zobrist[3][0][18]=0xE8FAA314E2AE928BLL;
zobrist[3][0][19]=0x5FBD32AF30773D5BLL;
zobrist[3][0][20]=0xCE7BBAD9D5A10DC2LL;
zobrist[3][0][21]=0xBC8AA3A6A681EB4DLL;
zobrist[3][0][22]=0xF5516C7C8B8B08FCLL;
zobrist[3][0][23]=0x45FCA954224EFCF1LL;
zobrist[3][0][24]=0xC100355987C46A8ELL;
zobrist[3][0][25]=0xB424CD5B11B3B48LL;
zobrist[3][0][26]=0xC747D0F6D3C1CF0LL;
zobrist[3][0][27]=0x2690B3CB1EF7EB6LL;
zobrist[3][0][28]=0xB2E5CEA3647B7913LL;
zobrist[3][0][29]=0x9100082F28B22351LL;
zobrist[3][0][30]=0xC9158270F6DFC21BLL;
zobrist[3][0][31]=0xBA4128F0FEF2C947LL;
zobrist[3][0][32]=0xE9DE1BCCB257AAC0LL;
zobrist[3][0][33]=0xE7050CCB9C3ADA0FLL;
zobrist[3][0][34]=0xB5E19899AA081ABALL;
zobrist[3][0][35]=0x3C3DE4A7BA187C06LL;
zobrist[3][0][36]=0x36B02A1A8558E37DLL;
zobrist[3][0][37]=0x45F86FD785D15CCLL;
zobrist[3][0][38]=0xB3DD7848748A3DEDLL;
zobrist[3][0][39]=0x181AAFEA64F54BC0LL;
zobrist[3][0][40]=0xD78B45B8CC69C047LL;
zobrist[3][0][41]=0x507F4C3B287F72ALL;
zobrist[3][0][42]=0x1FE631E986786BC6LL;
zobrist[3][0][43]=0x6E34AE7186D52F25LL;
zobrist[3][0][44]=0xF57F7FDE1E81AF86LL;
zobrist[3][0][45]=0x874B5D3C46D63C55LL;
zobrist[3][0][46]=0x99F0F31F7E235A23LL;
zobrist[3][0][47]=0x653BCFE2339C1AFFLL;
zobrist[3][0][48]=0x4F73231BA14E80A8LL;
zobrist[3][0][49]=0xE243E074C29CB1F6LL;
zobrist[3][0][50]=0xDC5F43595A24E03ALL;
zobrist[3][0][51]=0x234B5C77CF142FC9LL;
zobrist[3][0][52]=0xCE46E717247B05A4LL;
zobrist[3][0][53]=0x6B4AA9615A35842LL;
zobrist[3][0][54]=0xED301679C7C6433ELL;
zobrist[3][0][55]=0x6B7E4279F9249210LL;
zobrist[3][0][56]=0xE0E53400BC00028ELL;
zobrist[3][0][57]=0xCB990E6FD411479DLL;
zobrist[3][0][58]=0x8FA1A25DB73D1F92LL;
zobrist[3][0][59]=0x89AD77CE8C99AD3DLL;
zobrist[3][0][60]=0x56D659DF57B98E1FLL;
zobrist[3][0][61]=0xF6C500ABF7A91E41LL;
zobrist[3][0][62]=0xEBD440F0A5B6C08BLL;
zobrist[3][0][63]=0xB5D86A9ADDA9F454LL;
zobrist[3][1][0]=0xEC29335527987406LL;
zobrist[3][1][1]=0x2BE45BFA9E0ADD8BLL;
zobrist[3][1][2]=0x5DAF9FF7BE474780LL;
zobrist[3][1][3]=0xE782692532D989EDLL;
zobrist[3][1][4]=0xF5A319E3E3591447LL;
zobrist[3][1][5]=0x25624926DF8EA97ELL;
zobrist[3][1][6]=0x7B18CF94E1B4FE78LL;
zobrist[3][1][7]=0xFFABE73A2F38B80CLL;
zobrist[3][1][8]=0x9E1FC109AEC6FCLL;
zobrist[3][1][9]=0x735B0650B8610493LL;
zobrist[3][1][10]=0xD322782023CBE3FCLL;
zobrist[3][1][11]=0x1AA9B977CD216ADFLL;
zobrist[3][1][12]=0x1A68D417E5E6EBF2LL;
zobrist[3][1][13]=0x597CC918AB7551B5LL;
zobrist[3][1][14]=0xB5E61815E10CD1F0LL;
zobrist[3][1][15]=0xAA70058A32D96A5BLL;
zobrist[3][1][16]=0xCF46D0AABD37183ELL;
zobrist[3][1][17]=0x909432666499AE71LL;
zobrist[3][1][18]=0x15242BC73DDCDD86LL;
zobrist[3][1][19]=0xECC86DEED353AC9ALL;
zobrist[3][1][20]=0xC935D87D45763AACLL;
zobrist[3][1][21]=0x27EF66FC839B9C3ELL;
zobrist[3][1][22]=0xC7A1AF76CB52EF3ELL;
zobrist[3][1][23]=0x694F79F0718EC4EELL;
zobrist[3][1][24]=0x7F04415A899AF36ELL;
zobrist[3][1][25]=0x7DDBFAEBC32112C2LL;
zobrist[3][1][26]=0x629D2BFE27EBDE55LL;
zobrist[3][1][27]=0xD90604339EA57A42LL;
zobrist[3][1][28]=0xFA26F4AE2B7D6A29LL;
zobrist[3][1][29]=0x3B076783602FA54CLL;
zobrist[3][1][30]=0x1D9BC548368E0306LL;
zobrist[3][1][31]=0xB4E7FFDCA8BD0A73LL;
zobrist[3][1][32]=0x356E156AFC72302LL;
zobrist[3][1][33]=0x6B167B43AF594DAFLL;
zobrist[3][1][34]=0xB29CD0F25E0D7940LL;
zobrist[3][1][35]=0x7B56413EEDB1C6CELL;
zobrist[3][1][36]=0x91D0AC30D6E3E2A0LL;
zobrist[3][1][37]=0xA284AE9C3E2CF35FLL;
zobrist[3][1][38]=0xC79A72F46BD66BC5LL;
zobrist[3][1][39]=0x515054B2EAB7B587LL;
zobrist[3][1][40]=0xB373BED78D610A49LL;
zobrist[3][1][41]=0x1BE6E8D3D19266FCLL;
zobrist[3][1][42]=0xFA1E7245634A3925LL;
zobrist[3][1][43]=0x8886BACC5E51E764LL;
zobrist[3][1][44]=0xB26718D977CADA15LL;
zobrist[3][1][45]=0x92A7B10681DAFAFDLL;
zobrist[3][1][46]=0xFBDD6D0E69557108LL;
zobrist[3][1][47]=0x4E8E52D659975C39LL;
zobrist[3][1][48]=0xED8E8E5E46D13EE7LL;
zobrist[3][1][49]=0xC8B29204EB143E9DLL;
zobrist[3][1][50]=0x5B9B389A1802676CLL;
zobrist[3][1][51]=0xB6B8F9FA82959965LL;
zobrist[3][1][52]=0xEEF377E7250628ECLL;
zobrist[3][1][53]=0x2BEAFE3DB1F710DLL;
zobrist[3][1][54]=0x906D43D8FD4F966ALL;
zobrist[3][1][55]=0xAD5C102E878733LL;
zobrist[3][1][56]=0x9C0560639298C349LL;
zobrist[3][1][57]=0x68398EA47864703BLL;
zobrist[3][1][58]=0xB89DC32CF00B6D85LL;
zobrist[3][1][59]=0xAFD349AEBB8CE198LL;
zobrist[3][1][60]=0x92AB485AC7622F41LL;
zobrist[3][1][61]=0x3130F16F61476052LL;
zobrist[3][1][62]=0x55F6095571B952ELL;
zobrist[3][1][63]=0x7C24479CF89AFB5ELL;
zobrist[4][0][0]=0xDB2ACCDCBDFD6ALL;
zobrist[4][0][1]=0x28260B3373AE0C34LL;
zobrist[4][0][2]=0xD90C91F365F9B09LL;
zobrist[4][0][3]=0xAF26FED89361B814LL;
zobrist[4][0][4]=0x41829C0539F30FD7LL;
zobrist[4][0][5]=0xE69FF558E1C1B8D5LL;
zobrist[4][0][6]=0xAE92181CCC1588AELL;
zobrist[4][0][7]=0xEF1C936E19CC2690LL;
zobrist[4][0][8]=0x6D267AF7E71E7ADBLL;
zobrist[4][0][9]=0x91EC8402EA1AEB1CLL;
zobrist[4][0][10]=0x4D3F4515BF276BBLL;
zobrist[4][0][11]=0x2495BB647FEA5740LL;
zobrist[4][0][12]=0x6D981121CDD69E25LL;
zobrist[4][0][13]=0x5B7303A76F7CF701LL;
zobrist[4][0][14]=0x758BD3E275101BE3LL;
zobrist[4][0][15]=0xDA9F295723902509LL;
zobrist[4][0][16]=0xA38FF6358194344ELL;
zobrist[4][0][17]=0xC809DE9FE2D06A13LL;
zobrist[4][0][18]=0x4AD36AB3FFC1AC44LL;
zobrist[4][0][19]=0x75732754B5CBCE0LL;
zobrist[4][0][20]=0x2F8034483D1DB2ACLL;
zobrist[4][0][21]=0x85B12E1D67A1FBFFLL;
zobrist[4][0][22]=0x9825A3686022E711LL;
zobrist[4][0][23]=0x8A18DBB256F180CELL;
zobrist[4][0][24]=0x796808ABD19A603LL;
zobrist[4][0][25]=0x34F374D0C096FDB0LL;
zobrist[4][0][26]=0x2E5CCD216B0112C1LL;
zobrist[4][0][27]=0x686742F7F9981115LL;
zobrist[4][0][28]=0x8165E41B5583948ELL;
zobrist[4][0][29]=0xDF30A46645831973LL;
zobrist[4][0][30]=0x3DDD4CE53A8F2227LL;
zobrist[4][0][31]=0x447D17EA2247F352LL;
zobrist[4][0][32]=0x3DB7FD1D3E559419LL;
zobrist[4][0][33]=0x1F53EDCE1BE2EFFLL;
zobrist[4][0][34]=0xB6BF58954489F529LL;
zobrist[4][0][35]=0xB2BFA3CE192B7CB1LL;
zobrist[4][0][36]=0x5386CDD6A9DD2A0LL;
zobrist[4][0][37]=0x4FECC861B5933B27LL;
zobrist[4][0][38]=0xEC9CF67F430FC279LL;
zobrist[4][0][39]=0x130C1A063367EBDDLL;
zobrist[4][0][40]=0x30DCC9483B03E02CLL;
zobrist[4][0][41]=0x9D27DD2DA338AF5LL;
zobrist[4][0][42]=0x1370D4BCB9E8C2CLL;
zobrist[4][0][43]=0x5E2DA467990F988CLL;
zobrist[4][0][44]=0x2492D74DF47A5FB3LL;
zobrist[4][0][45]=0xDC9085A2B7163451LL;
zobrist[4][0][46]=0xA595D1D21E2F4EELL;
zobrist[4][0][47]=0xE9603D5132807922LL;
zobrist[4][0][48]=0x840A7B93AF8EEB8LL;
zobrist[4][0][49]=0x6DD756D2E8C8F520LL;
zobrist[4][0][50]=0xD26BE7B656F21AA4LL;
zobrist[4][0][51]=0xD8F299964EB0E87FLL;
zobrist[4][0][52]=0x61F70069A371EAF4LL;
zobrist[4][0][53]=0xCDD81A166419682BLL;
zobrist[4][0][54]=0x46CEE4FBFA8EE8BFLL;
zobrist[4][0][55]=0x9286389BF1C92B7FLL;
zobrist[4][0][56]=0x18A4D60093108A32LL;
zobrist[4][0][57]=0x5BD1DC099A9423A2LL;
zobrist[4][0][58]=0x2A1C2E66AD5DC763LL;
zobrist[4][0][59]=0x6CFFDE9044D371E4LL;
zobrist[4][0][60]=0xB1EECF819ED93186LL;
zobrist[4][0][61]=0xDE53F2719189BFCLL;
zobrist[4][0][62]=0xA2D7F80853C1BA91LL;
zobrist[4][0][63]=0x3DA050AC9F962453LL;
zobrist[4][1][0]=0xF4ED3EFFA6810875LL;
zobrist[4][1][1]=0xA696A4D1696D78A8LL;
zobrist[4][1][2]=0x74CCAD265E078ECCLL;
zobrist[4][1][3]=0xB52866C55A9B2044LL;
zobrist[4][1][4]=0x8EAFDF65E0D704EBLL;
zobrist[4][1][5]=0x84243CC8EB7B102ELL;
zobrist[4][1][6]=0x83C01169D830E321LL;
zobrist[4][1][7]=0x7B483701623EFFA5LL;
zobrist[4][1][8]=0xB8141ADD5ACD7EE0LL;
zobrist[4][1][9]=0xDAFE50E21FAA4233LL;
zobrist[4][1][10]=0xFECDF55294576E1CLL;
zobrist[4][1][11]=0x620FFE0D79F8A4BFLL;
zobrist[4][1][12]=0xD087D1D489F344BALL;
zobrist[4][1][13]=0xBF8C44FC25B35E39LL;
zobrist[4][1][14]=0xFC59BDA0480D68CFLL;
zobrist[4][1][15]=0xCC31871957657BACLL;
zobrist[4][1][16]=0x98DFE7D6398AEF12LL;
zobrist[4][1][17]=0xD4E6FC2EBC9EE709LL;
zobrist[4][1][18]=0xB42F9A97AD85BA0LL;
zobrist[4][1][19]=0x62C205DA3F6D8169LL;
zobrist[4][1][20]=0x7F81EB96E0A32515LL;
zobrist[4][1][21]=0x68427114A50915E1LL;
zobrist[4][1][22]=0x91C73B5FAECE03F2LL;
zobrist[4][1][23]=0xE89D3FC79894C034LL;
zobrist[4][1][24]=0xD44858C4EC739C9DLL;
zobrist[4][1][25]=0x68236F2C12612629LL;
zobrist[4][1][26]=0x4E22BA7E4879535ALL;
zobrist[4][1][27]=0x97AB3A3DDBEE13BLL;
zobrist[4][1][28]=0x9388B51D687765EDLL;
zobrist[4][1][29]=0x161719C845FA6AFLL;
zobrist[4][1][30]=0xAEF0E5343B21C7ALL;
zobrist[4][1][31]=0x7E558F3FFC81F98BLL;
zobrist[4][1][32]=0xB6424FD926213578LL;
zobrist[4][1][33]=0x8D0B9D06F219A37FLL;
zobrist[4][1][34]=0x9D1923B224C9A7FBLL;
zobrist[4][1][35]=0x31E8974953BEB449LL;
zobrist[4][1][36]=0x1CAC9D1AE47AF255LL;
zobrist[4][1][37]=0xE56F8E1F5CCC5B9DLL;
zobrist[4][1][38]=0x32C72994F628122LL;
zobrist[4][1][39]=0x5B119CF9FD0971B1LL;
zobrist[4][1][40]=0x7D60D174A0C0F52LL;
zobrist[4][1][41]=0x5249C5393919E5DLL;
zobrist[4][1][42]=0xC80CD1469F7B1FC7LL;
zobrist[4][1][43]=0x105E64D34EF3E7DELL;
zobrist[4][1][44]=0x77DAEB50D17817F7LL;
zobrist[4][1][45]=0x57B2307F33D05E02LL;
zobrist[4][1][46]=0xAF8AC87980A49AB4LL;
zobrist[4][1][47]=0xD56E118BB6A70768LL;
zobrist[4][1][48]=0xD0FC6E302AA29712LL;
zobrist[4][1][49]=0x24AC47F61D3749D1LL;
zobrist[4][1][50]=0x2B62A557F16971CALL;
zobrist[4][1][51]=0x500F360AACF492ALL;
zobrist[4][1][52]=0x91EA0009B5F08FFBLL;
zobrist[4][1][53]=0xF0EF5FB199961860LL;
zobrist[4][1][54]=0x7540AE4CE291A6B8LL;
zobrist[4][1][55]=0x846CE08C4C547086LL;
zobrist[4][1][56]=0x3A2D563186EDB93CLL;
zobrist[4][1][57]=0x704E96941CBC62FDLL;
zobrist[4][1][58]=0x2F5D67DE706060BDLL;
zobrist[4][1][59]=0x747B3EF346A1282LL;
zobrist[4][1][60]=0x1342D0ECC7567E9CLL;
zobrist[4][1][61]=0xF20573EC3C2EA77LL;
zobrist[4][1][62]=0xB8711E8220E7BEBFLL;
zobrist[4][1][63]=0xC1C58E384511FA4LL;
zobrist[5][0][0]=0xFC79D65CA3B619F7LL;
zobrist[5][0][1]=0xBC2ABBC4C8192595LL;
zobrist[5][0][2]=0x3411BF78A0E5B183LL;
zobrist[5][0][3]=0xE4ABEDEF3254AA70LL;
zobrist[5][0][4]=0x4FB42F1613472CDCLL;
zobrist[5][0][5]=0x6E821BFD30936527LL;
zobrist[5][0][6]=0xDBB746E559E744D0LL;
zobrist[5][0][7]=0xF23687A311F23E16LL;
zobrist[5][0][8]=0x226CA32D2A4BE1E1LL;
zobrist[5][0][9]=0xE4775A66617F198CLL;
zobrist[5][0][10]=0x1A0D41BB9FB9B1D6LL;
zobrist[5][0][11]=0xCB31F3D517C207C6LL;
zobrist[5][0][12]=0x79BC8532AB2F611LL;
zobrist[5][0][13]=0x8937C5E4FC4EC820LL;
zobrist[5][0][14]=0x8B8E256FAC1E2327LL;
zobrist[5][0][15]=0xFD684CD28DC5BB20LL;
zobrist[5][0][16]=0x5EA277301540CF8CLL;
zobrist[5][0][17]=0x9168D29E6DB674FELL;
zobrist[5][0][18]=0x3DD16129FD19773BLL;
zobrist[5][0][19]=0xC94658B462E15E75LL;
zobrist[5][0][20]=0x3CAE5FA02F881221LL;
zobrist[5][0][21]=0x7AAA6C38E32EFFELL;
zobrist[5][0][22]=0xECB89689B9497E7CLL;
zobrist[5][0][23]=0xDF5996E270DB6C2ELL;
zobrist[5][0][24]=0x6F755FB07B840795LL;
zobrist[5][0][25]=0xCB2182343C607685LL;
zobrist[5][0][26]=0x68B991D140863B7CLL;
zobrist[5][0][27]=0x669D6032BFE8E3DLL;
zobrist[5][0][28]=0x6339F35DDCA06346LL;
zobrist[5][0][29]=0xD121137BF0F37EDLL;
zobrist[5][0][30]=0x41BAF2D151ECEF46LL;
zobrist[5][0][31]=0x5E1CCF85A3D6E4FLL;
zobrist[5][0][32]=0xF1F6758649095BB7LL;
zobrist[5][0][33]=0x45C8ADFB6BB94C09LL;
zobrist[5][0][34]=0xDBF88845D0C87224LL;
zobrist[5][0][35]=0x9FEA174FC8D9C835LL;
zobrist[5][0][36]=0xF00B0688E6203E77LL;
zobrist[5][0][37]=0xF3491922C48CF483LL;
zobrist[5][0][38]=0x28319B4E15288342LL;
zobrist[5][0][39]=0xF0C8A2F455227152LL;
zobrist[5][0][40]=0x42F7EAE7FBCC1FD6LL;
zobrist[5][0][41]=0x29463C6B68EF21A6LL;
zobrist[5][0][42]=0xF0C2E5FC859ACB6FLL;
zobrist[5][0][43]=0x380C8CAD2DD4AB50LL;
zobrist[5][0][44]=0xDA528AA8A9AFC9A4LL;
zobrist[5][0][45]=0xE01A1EACFE32FFB8LL;
zobrist[5][0][46]=0x827E97C879DAF2EDLL;
zobrist[5][0][47]=0x87817B4583FB0A7ELL;
zobrist[5][0][48]=0x798ACC53A81F7CF0LL;
zobrist[5][0][49]=0xE2A80411158ACAB9LL;
zobrist[5][0][50]=0xE37A2A138F3BA8B0LL;
zobrist[5][0][51]=0x473F839D259F7F0LL;
zobrist[5][0][52]=0x676D7465E5F66188LL;
zobrist[5][0][53]=0xE5F8A9F7F5E244F5LL;
zobrist[5][0][54]=0x49EEF32B6400CAFELL;
zobrist[5][0][55]=0xB5E99D5FDC3B16C3LL;
zobrist[5][0][56]=0x772F89205FDF04BCLL;
zobrist[5][0][57]=0x9C4C3FA6E8E3EC50LL;
zobrist[5][0][58]=0xC1C4E9551D6F0BLL;
zobrist[5][0][59]=0xD205E97A7163FE56LL;
zobrist[5][0][60]=0xE4522D67F603C05ELL;
zobrist[5][0][61]=0x63673C697A0B27CALL;
zobrist[5][0][62]=0x15B5959E2E79D93ALL;
zobrist[5][0][63]=0x3990B25DF8D6D957LL;
zobrist[5][1][0]=0x292CBC26292E335LL;
zobrist[5][1][1]=0x281BC5213B5DD099LL;
zobrist[5][1][2]=0x4EFF09A3137DA58FLL;
zobrist[5][1][3]=0x3B28D8AB131F9050LL;
zobrist[5][1][4]=0x553F95BDF1F078C6LL;
zobrist[5][1][5]=0x333E6FFA1CF0774LL;
zobrist[5][1][6]=0x94A8B355B2C65D59LL;
zobrist[5][1][7]=0xBA0E420444A40288LL;
zobrist[5][1][8]=0xDD66CBA1378910D1LL;
zobrist[5][1][9]=0x1DAEA7E4F81C7D9ELL;
zobrist[5][1][10]=0xE4502A5814F43098LL;
zobrist[5][1][11]=0x33EB8D1CE80EA2ALL;
zobrist[5][1][12]=0x6E230DA586CD97E7LL;
zobrist[5][1][13]=0x268BA7011E4F38D5LL;
zobrist[5][1][14]=0xB4FE976443295A63LL;
zobrist[5][1][15]=0x9D3474319497DD4DLL;
zobrist[5][1][16]=0x40645D6869162936LL;
zobrist[5][1][17]=0x1716F5EDF9C55307LL;
zobrist[5][1][18]=0x83E27BB21A6B6D7ELL;
zobrist[5][1][19]=0x8630DC8680EA704FLL;
zobrist[5][1][20]=0x14831DD213029C2CLL;
zobrist[5][1][21]=0x3C237AD5CE628BDLL;
zobrist[5][1][22]=0x451B9C5D647CBF18LL;
zobrist[5][1][23]=0xCEB4CD38423B6664LL;
zobrist[5][1][24]=0x562F70D5EC15F5F0LL;
zobrist[5][1][25]=0x6BE134E6172FC4CCLL;
zobrist[5][1][26]=0x9E613717440A45BLL;
zobrist[5][1][27]=0x30E7930B9EA22BCBLL;
zobrist[5][1][28]=0xE8B1393EA209CBF3LL;
zobrist[5][1][29]=0x881B3A0DD3A19755LL;
zobrist[5][1][30]=0x31C90C63E7B7DFBDLL;
zobrist[5][1][31]=0x56ECB8376ABD295LL;
zobrist[5][1][32]=0x732C94C9884E72BLL;
zobrist[5][1][33]=0xD0B43AD751E7EA24LL;
zobrist[5][1][34]=0xD375181D38BEE45FLL;
zobrist[5][1][35]=0xD2EFD6195EB7F660LL;
zobrist[5][1][36]=0xBC3FCFA7D0396D4ELL;
zobrist[5][1][37]=0xE100C49C8EB20411LL;
zobrist[5][1][38]=0x1DBD4EE07664AF96LL;
zobrist[5][1][39]=0x14AF5C2293B50D2LL;
zobrist[5][1][40]=0x3A5E10DB515E080LL;
zobrist[5][1][41]=0xA111A1DBBBB2EEA4LL;
zobrist[5][1][42]=0xB6B0489174C02583LL;
zobrist[5][1][43]=0xFF74D4A6BBB826FBLL;
zobrist[5][1][44]=0xA5B09C938953E6E8LL;
zobrist[5][1][45]=0xD242BF2EAD8D9817LL;
zobrist[5][1][46]=0xF5FA7E0A53A804EALL;
zobrist[5][1][47]=0xB08B73401E009A15LL;
zobrist[5][1][48]=0xAE29170D5F339D80LL;
zobrist[5][1][49]=0x6890B8AE93F6BE22LL;
zobrist[5][1][50]=0xDC5D51DB1240DA4BLL;
zobrist[5][1][51]=0x5FEBD59F6E2FEA81LL;
zobrist[5][1][52]=0xFD3027612A15FD81LL;
zobrist[5][1][53]=0x5EDD7D6A26EC7456LL;
zobrist[5][1][54]=0xD688414AAC7292ABLL;
zobrist[5][1][55]=0x5AFB775F9538316CLL;
zobrist[5][1][56]=0x3848A15EF7BF8F8CLL;
zobrist[5][1][57]=0x97884E29B11703BLL;
zobrist[5][1][58]=0xED09F3BA3BBE9923LL;
zobrist[5][1][59]=0x33D7D4D36C148899LL;
zobrist[5][1][60]=0xB5AF7ECEF7547D83LL;
zobrist[5][1][61]=0x69C49BDE7D6D5213LL;
zobrist[5][1][62]=0x39EBD4D1E5CEEF34LL;
zobrist[5][1][63]=0xA51F1C881C989F44LL;
zobrist[6][0][0]=0x981F10789AB39E7BLL;
zobrist[6][0][1]=0xE1A3754ABFC7844LL;
zobrist[6][0][2]=0x50BAF1FE7710D907LL;
zobrist[6][0][3]=0xA62D272DFBA5941FLL;
zobrist[6][0][4]=0xF2E0C367CDC76703LL;
zobrist[6][0][5]=0xF39B53625F50F0BALL;
zobrist[6][0][6]=0x733C2FF44E57421FLL;
zobrist[6][0][7]=0x634A8362244AE18BLL;
zobrist[6][0][8]=0xED6C60FE60CA22C2LL;
zobrist[6][0][9]=0xC549D6AB50B9B48ALL;
zobrist[6][0][10]=0x1B61EE0CA06E00BLL;
zobrist[6][0][11]=0x8C82DFF9FFF29A4CLL;
zobrist[6][0][12]=0xA145FF4C122A6C09LL;
zobrist[6][0][13]=0x68818CBFA3873100LL;
zobrist[6][0][14]=0x9954DD3A4B0D5853LL;
zobrist[6][0][15]=0x9583D3BE8D3F2E66LL;
zobrist[6][0][16]=0xCAE10F10D957818DLL;
zobrist[6][0][17]=0xCFFC91B5B2433C62LL;
zobrist[6][0][18]=0xB735690A38A6ACC3LL;
zobrist[6][0][19]=0x9316D71A2816CD25LL;
zobrist[6][0][20]=0x4694DF90E30FB49FLL;
zobrist[6][0][21]=0x96C005B8C111E23DLL;
zobrist[6][0][22]=0xCFCF0D1A968D3EE2LL;
zobrist[6][0][23]=0x78D40361BF3F0C60LL;
zobrist[6][0][24]=0x312C7D69E663DDB6LL;
zobrist[6][0][25]=0xB0553CFCFEB1A21DLL;
zobrist[6][0][26]=0xA913560A9CA9E9CALL;
zobrist[6][0][27]=0x5470A88220F1694LL;
zobrist[6][0][28]=0x3F04D8FC3633E349LL;
zobrist[6][0][29]=0xA5153222C9CCCBF3LL;
zobrist[6][0][30]=0xCD6423689E5B9E2FLL;
zobrist[6][0][31]=0x7D6A4A6DC4711F73LL;
zobrist[6][0][32]=0xB15189C0F87E9A81LL;
zobrist[6][0][33]=0x5B62890F6337A8E2LL;
zobrist[6][0][34]=0xDEA8104F729A2FBBLL;
zobrist[6][0][35]=0x40EA1F14D42A2CD5LL;
zobrist[6][0][36]=0xC775923E5BFC7ECLL;
zobrist[6][0][37]=0xADD6DCB4DCCC227LL;
zobrist[6][0][38]=0x1DD3D7DDBB8D4FDCLL;
zobrist[6][0][39]=0xF33B146DD3FB18E4LL;
zobrist[6][0][40]=0x7A7C343931946C1ALL;
zobrist[6][0][41]=0xB578F6675B446CC1LL;
zobrist[6][0][42]=0x8DA6D42A2AEB1D96LL;
zobrist[6][0][43]=0x94EF5D681592CF37LL;
zobrist[6][0][44]=0x5A7E864B493229FLL;
zobrist[6][0][45]=0x61123544952DC531LL;
zobrist[6][0][46]=0x62264753F7FDD955LL;
zobrist[6][0][47]=0xD9C20EB9A838F49LL;
zobrist[6][0][48]=0xDC71F4A9CDD7E3ELL;
zobrist[6][0][49]=0x8C26B8A8D8608D79LL;
zobrist[6][0][50]=0x3EFC9F435DB93B22LL;
zobrist[6][0][51]=0xF97A60ABD74E3FLL;
zobrist[6][0][52]=0x1605573AC9769948LL;
zobrist[6][0][53]=0x73BD7B3695A4F07ELL;
zobrist[6][0][54]=0xCC8269B454CB90FLL;
zobrist[6][0][55]=0x60EB2E54CB7ED75ELL;
zobrist[6][0][56]=0x2180B3F56EA795E7LL;
zobrist[6][0][57]=0xECF59AA0E9591354LL;
zobrist[6][0][58]=0x3A972FDC79146F96LL;
zobrist[6][0][59]=0x957519C257D3BF17LL;
zobrist[6][0][60]=0xE43AF60D94C3AA21LL;
zobrist[6][0][61]=0x8996321C6A93776LL;
zobrist[6][0][62]=0xA911BACED3FEBF37LL;
zobrist[6][0][63]=0xF6B741199A02127LL;
zobrist[6][1][0]=0xD06CB103E7EED516LL;
zobrist[6][1][1]=0xC4AEDC2A26924E03LL;
zobrist[6][1][2]=0x22F02104D67E2E65LL;
zobrist[6][1][3]=0x128542F99BFF5C3LL;
zobrist[6][1][4]=0x78EC7FED8E3B17AALL;
zobrist[6][1][5]=0x73F3D18864698115LL;
zobrist[6][1][6]=0x30065AED5D3AB63LL;
zobrist[6][1][7]=0x1855D9797F2F577ALL;
zobrist[6][1][8]=0xC0B0BDA89025D2BALL;
zobrist[6][1][9]=0x24821E5FE56FA1B3LL;
zobrist[6][1][10]=0xF6E1FF42C04523FDLL;
zobrist[6][1][11]=0x3F82A276A93AFE3DLL;
zobrist[6][1][12]=0xDA719C33200782FELL;
zobrist[6][1][13]=0x7C8E739717332C2FLL;
zobrist[6][1][14]=0x854056235536EC8ALL;
zobrist[6][1][15]=0x4B119886D8C69E9DLL;
zobrist[6][1][16]=0x92F2AD88B4A22419LL;
zobrist[6][1][17]=0x7645E78D61664680LL;
zobrist[6][1][18]=0x8B625C54669AA63BLL;
zobrist[6][1][19]=0xD16E193FC9228D87LL;
zobrist[6][1][20]=0xFB8494223D7A7B5BLL;
zobrist[6][1][21]=0x8C417D9EB9C9D9D7LL;
zobrist[6][1][22]=0x474A93F1F845AC16LL;
zobrist[6][1][23]=0x3C103E568DF45E47LL;
zobrist[6][1][24]=0x889E6F04947318DFLL;
zobrist[6][1][25]=0xAFE35CB79A660B25LL;
zobrist[6][1][26]=0x19F95CDC384B3970LL;
zobrist[6][1][27]=0xC84817C37FBD2555LL;
zobrist[6][1][28]=0x6C897C9957174476LL;
zobrist[6][1][29]=0x1CA249CD53186E5CLL;
zobrist[6][1][30]=0x5470EC5DF956836DLL;
zobrist[6][1][31]=0xE08C3BA6DD95B017LL;
zobrist[6][1][32]=0x9293EFE21A50FC5ELL;
zobrist[6][1][33]=0x8F42D0F5F234D258LL;
zobrist[6][1][34]=0x9B73AD527C89863FLL;
zobrist[6][1][35]=0x9A5A9BDE5A56AB24LL;
zobrist[6][1][36]=0x60EB6915E93F0121LL;
zobrist[6][1][37]=0x75C5C6A0CB7CD5CCLL;
zobrist[6][1][38]=0x1CE0E6AA6666CB7ELL;
zobrist[6][1][39]=0x45B2A737FD04E37ELL;
zobrist[6][1][40]=0x7829E488CAEA001LL;
zobrist[6][1][41]=0x3EADAD13BE4AE184LL;
zobrist[6][1][42]=0xB5C8521E474828C7LL;
zobrist[6][1][43]=0x627683ADC7A80BD9LL;
zobrist[6][1][44]=0xF46CFA72781383CBLL;
zobrist[6][1][45]=0xB4B83958C0123026LL;
zobrist[6][1][46]=0x118AE4AA1E923FA4LL;
zobrist[6][1][47]=0x94FEF0B1BB213898LL;
zobrist[6][1][48]=0x80B73D124F76EE61LL;
zobrist[6][1][49]=0x79BEF72227C5318LL;
zobrist[6][1][50]=0x956DF97B269CBF03LL;
zobrist[6][1][51]=0x46B726FCB1191E72LL;
zobrist[6][1][52]=0x7F5465C6171EB15DLL;
zobrist[6][1][53]=0xB380A918CB957BDCLL;
zobrist[6][1][54]=0x373FC3CE50052284LL;
zobrist[6][1][55]=0x381EC86ACF603119LL;
zobrist[6][1][56]=0x8D5DBEDAC02FEA29LL;
zobrist[6][1][57]=0xA7C3B5CFB8488E65LL;
zobrist[6][1][58]=0x110551C38EEF823CLL;
zobrist[6][1][59]=0xC9374B464D110512LL;
zobrist[6][1][60]=0x31EB58FA67800E8BLL;
zobrist[6][1][61]=0xB443BBA0B74218DFLL;
zobrist[6][1][62]=0x7CE179AF8A88138LL;
zobrist[6][1][63]=0x3EAD0A358F52BB77LL;
zobristEnPassant[0]=0x50955F13740D1641LL;
zobristEnPassant[1]=0xF4E89682BF41C644LL;
zobristEnPassant[2]=0x68B967833399DD16LL;
zobristEnPassant[3]=0xAB69319E84B2DD52LL;
zobristEnPassant[4]=0x106A0E022C7BDA47LL;
zobristEnPassant[5]=0xCD05AC2DBCE37BFLL;
zobristEnPassant[6]=0x737FF62272D48EE4LL;
zobristEnPassant[7]=0x76F67CE010E0AEC9LL;
zobristEnPassant[8]=0xF614D806D4DD21E4LL;
zobristEnPassant[9]=0x4947CA6D4ECDDFB6LL;
zobristEnPassant[10]=0x9B0EE5334F6C0871LL;
zobristEnPassant[11]=0xCEB1CEEB3CDFFE64LL;
zobristEnPassant[12]=0xEA1A5DE7D65CFC95LL;
zobristEnPassant[13]=0xE9E2BCBCDDFDCE15LL;
zobristEnPassant[14]=0x5E9D0358E8D7E0C4LL;
zobristEnPassant[15]=0xBEED1BE858592656LL;
zobristEnPassant[16]=0x9F82A9F3CC419D41LL;
zobristEnPassant[17]=0xECC27E189CEFC1BALL;
zobristEnPassant[18]=0x836FBD3BFAA485B9LL;
zobristEnPassant[19]=0xE9C6B340426D1106LL;
zobristEnPassant[20]=0xAFF7D9DFCE8F166FLL;
zobristEnPassant[21]=0xCC689A3E4B1D81FLL;
zobristEnPassant[22]=0x4F349FE2F8B88484LL;
zobristEnPassant[23]=0x20A02A415C9BEA7ELL;
zobristEnPassant[24]=0x7CF54E9BCB9970BDLL;
zobristEnPassant[25]=0x1387F6CA44FEBE4DLL;
zobristEnPassant[26]=0x814FBE0425C61299LL;
zobristEnPassant[27]=0xEBEEA64B40A0B83ALL;
zobristEnPassant[28]=0x54CD9C5F23C870BLL;
zobristEnPassant[29]=0xA4BEFAB298455EB1LL;
zobristEnPassant[30]=0xBE6BF9A3EE40D2EALL;
zobristEnPassant[31]=0xBC84B326CC624E5FLL;
zobristEnPassant[32]=0x862BFBFDF869ADC6LL;
zobristEnPassant[33]=0x5CB4DA3A4F6057B2LL;
zobristEnPassant[34]=0xDCBD2F9D7AA4149LL;
zobristEnPassant[35]=0xB2441447F409B7C6LL;
zobristEnPassant[36]=0x80E1442AADA5471ELL;
zobristEnPassant[37]=0x81E1F9ED21DD56LL;
zobristEnPassant[38]=0xA74B32ABEDD3CA7LL;
zobristEnPassant[39]=0x6DCB13BC20FCDDALL;
zobristEnPassant[40]=0xBBC5C763333EBB11LL;
zobristEnPassant[41]=0x5B034A1A233E1EC9LL;
zobristEnPassant[42]=0x38BBBFE99B4FEC23LL;
zobristEnPassant[43]=0xAC89A53FC2FA6D19LL;
zobristEnPassant[44]=0xB9A4ED1771E75850LL;
zobristEnPassant[45]=0x280FADEACB0F0475LL;
zobristEnPassant[46]=0x9275F29F658C792FLL;
zobristEnPassant[47]=0x238D76DC5AC145LL;
zobristEnPassant[48]=0x1F164E8E92498465LL;
zobristEnPassant[49]=0xE612DDA2AEFD7C6DLL;
zobristEnPassant[50]=0x8935EC0770A041A8LL;
zobristEnPassant[51]=0xFA2ACC20A5076B97LL;
zobristEnPassant[52]=0x6A451E235EA9361LL;
zobristEnPassant[53]=0xA31302C11ABC03CFLL;
zobristEnPassant[54]=0x9393F141D16CCDF0LL;
zobristEnPassant[55]=0xDD9B3E401F236726LL;
zobristEnPassant[56]=0xF0439BC9C54F995LL;
zobristEnPassant[57]=0xE9F3F5294D6883CLL;
zobristEnPassant[58]=0x233C75F96AFFDF4LL;
zobristEnPassant[59]=0x6B4AC4CDEA687880LL;
zobristEnPassant[60]=0xCB5A034529C8E2BLL;
zobristEnPassant[61]=0xC97693407879F9E5LL;
zobristEnPassant[62]=0x192D4F2987103AB3LL;
zobristEnPassant[63]=0x9965D518C2C807FALL;
zobristEnPassant[CASEVIDE]=0x202A331F19464B76LL;
zobristBlanc=0x1C6EEEFCA1530610LL;
zobristPetitRoque[BLANC]=0x6C724F8EDE61C892LL;
zobristGrandRoque[BLANC]=0x8B47B2BADE0D72C6LL;
zobristPetitRoque[NOIR]=0x45F5641E1CA42E00LL;
zobristGrandRoque[NOIR]=0x7EC1C4311E83C69ELL;
zobristPhase[DEBUT]=0xD17B79A7207899CDLL;
zobristPhase[MILIEU]=0xEA31F74E80146F44LL;
zobristPhase[FIN]=0x85EED9FC968F6F83LL;
}

/*------------------------------------------------------------------------------------------*/

void init( Plateau *p ) {
/*	p = (Plateau*) malloc( sizeof(Plateau) ); */
	srand(time(NULL)); 
	initFirstBitBitBoard();
	initNbBits();
	initCaseToBitBoard();
	initDeplacementTour();
	initDeplacementCavalier();
	initDeplacementFou();
	initDeplacementRoi();
	initDeplacementDame();
	initDeplacementFouA1H8();
	initDeplacementFouA8H1();	
	initDeplacementTourColonne( );
	initDeplacementTourLigne( );	
	initDeplacementPionCapture();
	initDecaleBitBoard45() ;
	initDecaleBitBoard315() ;
	initDecaleBitBoard90();
	initDecaleBitBoardLigne();
	initPieceToBitBoard(p);
	initFlagCapture();
	initDroiteContenantCases();
	initCasesEntreDeuxCasesDiag();
	initcasesEntreDeuxCasesColLig();
	initCasesEntreDeuxCases();
	initZobrist();
	initPionAcolyte();
	initPionPasse();
	multipv_alloc( &mpv ); /* mpv est une variable globale presente dans multipv.h */
}