#ifndef KILLER_H
#define KILLER_H

#include "constante.h"
#include "coup.h"

#define NB_KILLER 3

typedef Coup Killer[MAX_PROFONDEUR][NB_KILLER];

#define killer_add(  k , c , p )  if ( !coup_isCapture( c ) ) { \
  	 			    if ( k[p][0] != c ) { \
	    				k[p][2] = k[p][1];    \
	    				k[p][1] = k[p][0];	\
	    				k[p][0] = c;\
  	  			    } \
       			          } \

#define killer_reset( k )  for( i=0 ; i<MAX_PROFONDEUR ; i++ ) { \
				  k[i][0] = PAS_DE_COUP; \
				  k[i][1] = PAS_DE_COUP; \
				  k[i][2] = PAS_DE_COUP; \
			   } \

#define killer_get( k , p , i)	k[p][i]

#endif

