#include <string.h>
#include "case.h" 

char *nomCase[] = {"a1" , "b1" , "c1" , "d1" , "e1" , "f1" , "g1" , "h1" ,
				   "a2" , "b2" , "c2" , "d2" , "e2" , "f2" , "g2" , "h2" ,
				   "a3" , "b3" , "c3" , "d3" , "e3" , "f3" , "g3" , "h3" ,
				   "a4" , "b4" , "c4" , "d4" , "e4" , "f4" , "g4" , "h4" ,
				   "a5" , "b5" , "c5" , "d5" , "e5" , "f5" , "g5" , "h5" ,
				   "a6" , "b6" , "c6" , "d6" , "e6" , "f6" , "g6" , "h6" ,
				   "a7" , "b7" , "c7" , "d7" , "e7" , "f7" , "g7" , "h7" ,
				   "a8" , "b8" , "c8" , "d8" , "e8" , "f8" , "g8" , "h8" , "-" };


/*  Donne pour une case donnee la case equivalente dans une roration à 45 */
unsigned char case45[] = { A1, B2, C3, D4, E5, F6, G7, H8,
						   A2, B3, C4, D5, E6, F7, G8, H1,
						   A3, B4, C5, D6, E7, F8, G1, H2,
						   A4, B5, C6, D7, E8, F1, G2, H3,
						   A5, B6, C7, D8, E1, F2, G3, H4,
						   A6, B7, C8, D1, E2, F3, G4, H5,
						   A7, B8, C1, D2, E3, F4, G5, H6,
						   A8, B1, C2, D3, E4, F5, G6, H7};       
								

/*  Donne pour une case donnee la case equivalente dans une roration à 315 */

unsigned char case315[] = { A1, B8, C7, D6,E5, F4, G3, H2,
							A2, B1, C8, D7, E6, F5, G4, H3,
							A3, B2, C1, D8, E7, F6, G5, H4,
							A4, B3, C2, D1, E8, F7, G6, H5,
							A5, B4, C3, D2, E1, F8, G7, H6,
							A6, B5, C4, D3, E2, F1, G8, H7,
							A7, B6, C5, D4, E3, F2, G1, H8,
							A8, B7, C6, D5, E4, F3, G2, H1};  

/*  Donne pour une case donnee la case equivalente dans une roration à 90 */

/*unsigned char case90[] = { A8, A7, A6, A5,A4, A3, A2, A1,
			 B8, B7, B6, B5, B4, B3, B2, B1,
			 C8, C7, C6, C5, C4, C3, C2, C1,
			 D8, D7, D6, D5, D4, D3, D2, D1,
			 E8, E7, E6, E5, E4, E3, E2, E1,
			F8, F7, F6, F5, F4, F3, F2, F1,
			G8, G7, G6, G5, G4, G3, G2, G1,
			H8, H7, H6, H5, H4, H3, H2, H1};  */

unsigned char case90[] = { A1, A2, A3, A4,A5, A6, A7, A8,
						   B1, B2, B3, B4, B5, B6, B7, B8,
						   C1, C2, C3, C4, C5, C6, C7, C8,
						   D1, D2, D3, D4, D5, D6, D7, D8,
						   E1, E2, E3, E4, E5, E6, E7, E8,
						   F1, F2, F3, F4, F5, F6, F7, F8,
						   G1, G2, G3, G4, G5, G6, G7, G8,
						   H1, H2, H3, H4, H5, H6, H7, H8}; 										

/*-----------------------------------------------------------------------------------------------------*/

Case case_getNomToCase( char *c ) {
	char lc[3];
	Case i;
	
	strcpy( lc , c );
	if ( lc[0]>='A' && lc[0]<='Z' ) lc[0] = 'a' + (lc[0]-'A' );
	
	for ( i=A1 ; i<=H8 ; i++ ) {
		if (!(strcmp( nomCase[i] , lc ))) break;
	}
	
	return i;
}					

/*-----------------------------------------------------------------------------------------------------*/

Case case_getCoordonneeToCase( short c, short l ) {
	return (l*8)+c;
}
