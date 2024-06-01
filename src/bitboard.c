#include <stdio.h>
#include "bitboard.h"
#include "case.h" 
#include "init.h"



/* Donne le BitBoard correspondant à une case de l'echiquier */
BitBoard caseToBitBoard[ 65 ];
BitBoard caseToBitBoard90[ 65 ];
BitBoard caseToBitBoard45[ 65 ];
BitBoard caseToBitBoard315[ 65 ];

 
/* Donne le BitBoard correspondant à toutes les cases sauf celle utilisee comme indice */
BitBoard complementCaseToBitBoard[ 65 ];
BitBoard complementCaseToBitBoard90[ 65 ];
BitBoard complementCaseToBitBoard45[ 65 ];
BitBoard complementCaseToBitBoard315[ 65 ];


/* Contient la position du premier bit à 1 pour les entiers de 0 à 65536 */
unsigned char firstBit[ 65536 ];

#if 0
unsigned char getFirstBit( BitBoard b ) {
	if ( b>>48) return firstBit[ b>>48 ]+48;
	if ( b>>32) return firstBit[ b>>32 ]+32;	
	if ( b>>16) return firstBit[ b>>16 ]+16;		
	return firstBit[ b ];
}
#endif

void 	printBitBoardHexa ( BitBoard b ) {
   int i;
   int bb;
	
   for( i=3 ; i>=0 ; i-- ) {
	bb = (int)((b>>(i*16)) & 0xFFFF) ;
	printf("%X",bb);
   }
}
/*------------------------------------------------------------------------------------------*/

void printBitBoard(BitBoard b) {
  int i;
  int j;
  char s[17]; 
  BitBoard t = 1;

  t=t<<63;
  s[16] = '\0';
  printf("   A B C D E F G H\n");
  printf("   ________________\n");
/*  for(i=0;i<8;i++) {
   	for ( j=0 ; j<8 ; j++ ) {
		  if (b&1) {
		    s[14-2*j]='1';
		    s[15-2*j]=' '; 
		 }
		 else {
		    s[14-2*j]='0';
		    s[15-2*j]=' '; 
		}  
		  b=b>>1;
     } 
     printf("%d- %s\n",i+1,s);
  }	 */
  for(i=0;i<8;i++) {
   	for ( j=0 ; j<8 ; j++ ) {
		  if (b&t) {
		    s[14-2*j]='1';
			s[15-2*j]=' '; 
		 }
		 else {
		    s[14-2*j]='0';
		    s[15-2*j]=' '; 
		}  
		  b=b<<1;
		     } 
     printf("%d- %s\n",8-i,s);
  }	 
}

/*------------------------------------------------------------------------------------------------*/

void printBitBoardLigne( BitBoard b ) {
	int i;
	char s[64];

	
	for(i=A1;i<=H8;i++) {
		if (b & 1) s[i]='1'; else s[i]='0';
		b = b >>1;
	}
	
	for(i=H8;i>=A1;i--) {
	  printf("%c",s[i]);
	  if (!( i %8 )) printf(" ");
	}
	printf("\n");
}
/*------------------------------------------------------------------------------------------------*/
 
void initFirstBitBitBoard() {
	int i,p,t;
	
	p=0;
	t=1;
	
	for ( i=1 ; i<65536 ; i++ ) {
		while (!( i & t )) {
			t=t<<1;
			p++;
		}
		firstBit[i]=p;
		p=0;
		t=1;
	}
}

/*------------------------------------------------------------------------------------------------*/

BitBoard	rotation90BitBoard( BitBoard b ) {
	 Case i;
	 BitBoard rb =0;
	
	for (i=A1; i <= H8; i++ ) {
		  if ( b & getCaseToBitBoard( case_getRotationCase90( i ) ) ) rb |= getCaseToBitBoard( i );
	}
	
   return rb;	
}

/*------------------------------------------------------------------------------------------------*/

BitBoard			rotation45BitBoard( BitBoard b ) {
	 Case i;
	 BitBoard rb =0;
	
	for (i=A1; i <= H8; i++ ) {
		  if ( b & getCaseToBitBoard( case_getRotationCase45( i ) ) ) rb |= getCaseToBitBoard( i );
	}
	
   return rb;	
}

/*------------------------------------------------------------------------------------------------*/

BitBoard			rotation315BitBoard( BitBoard b ) {
	 Case i;
	 BitBoard rb =0;
	
	for (i=A1; i <= H8; i++) {
	  if ( b & getCaseToBitBoard( case_getRotationCase315( i ) ) ) rb |= getCaseToBitBoard( i );
	}
	
   return rb;	
}

/*------------------------------------------------------------------------------------------------*/

void initCaseToBitBoard() {
	Case i;
	BitBoard b = 1;
	
	for(  i=A1 ; i<=H8 ; i++ ) {
		caseToBitBoard[ i ] = b;
		complementCaseToBitBoard[ i ] = ~b;
		
		caseToBitBoard90[ case_getRotationCase90(i) ] = b;
		complementCaseToBitBoard90[ case_getRotationCase90(i) ] = ~b;

		caseToBitBoard45[ case_getRotationCase45(i) ] = b;
		complementCaseToBitBoard45[ case_getRotationCase45(i) ] = ~b;

		caseToBitBoard315[ case_getRotationCase315(i) ] = b;
		complementCaseToBitBoard315[ case_getRotationCase315(i) ] = ~b;

		b = b << 1;
	}
	caseToBitBoard[ 64 ] = 0;
	complementCaseToBitBoard[ 64 ] = caseToBitBoard[ 64 ];	
	
	caseToBitBoard45[ 64 ] = 0;
	complementCaseToBitBoard45[ 64 ] = caseToBitBoard45[ 64 ];	

	caseToBitBoard90[ 64 ] = 0;
	complementCaseToBitBoard90[ 64 ] = caseToBitBoard90[ 64 ];	

	caseToBitBoard315[ 64 ] = 0;
	complementCaseToBitBoard315[ 64 ] = caseToBitBoard315[ 64 ];	
	
}

