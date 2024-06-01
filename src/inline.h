#ifndef INLINE_H
#define INLINE_H
#if 0
inline unsigned char getFirstBit( BitBoard b ) {
	if ( b>>48) return firstBit[ b>>48 ]+48;
	if ( b>>32) return firstBit[ b>>32 ]+32;	
	if ( b>>16) return firstBit[ b>>16 ]+16;		
	return firstBit[ b ];
}
#endif
#endif
