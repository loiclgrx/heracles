#ifndef BITBOARD_H
#define BITBOARD_H

#include "type.h"

typedef int64 BitBoard;

extern unsigned char firstBit[ 65536 ];

void 		printBitBoard( BitBoard );
void 		printBitBoardLigne ( BitBoard  );
void 		printBitBoardHexa ( BitBoard  );
void 		initFirstBitBitBoard();
void 		initCaseToBitBoard();
BitBoard	rotation90BitBoard( BitBoard );
BitBoard	rotation45BitBoard( BitBoard );
BitBoard	rotation315BitBoard( BitBoard );
//unsigned char getFirstBit( BitBoard  );

#define getFirstBit( b )  ((b>>48) ? ( firstBit[ b>>48 ]+48 ) :\
                                   ( (b>>32)? ( firstBit[ b>>32 ]+32) :\
                                     ((b>>16)? ( firstBit[ b>>16 ]+16 ) : firstBit[ b ] ) ) )


/* Donne le BitBoard correspondant à une case de l'echiquier */
extern BitBoard caseToBitBoard[ 65 ];
extern BitBoard caseToBitBoard90[ 65 ];
extern BitBoard caseToBitBoard45[ 65 ];
extern BitBoard caseToBitBoard315[ 65 ];

/* Donne le BitBoard correspondant à toutes les cases sauf celle utilisee comme indice */
extern BitBoard complementCaseToBitBoard[ 65 ];
extern BitBoard complementCaseToBitBoard90[ 65 ];
extern BitBoard complementCaseToBitBoard45[ 65 ];
extern BitBoard complementCaseToBitBoard315[ 65 ];

#define videBit( b , n )    		(b &= complementCaseToBitBoard[ n ] ) 
#define videBit90( b , n )			(b &= complementCaseToBitBoard90[n] )
#define videBit45( b , n )			(b &= complementCaseToBitBoard45[n] )
#define videBit315( b , n )			(b &= complementCaseToBitBoard315[n] )




#endif /* BITBOARD_H */
