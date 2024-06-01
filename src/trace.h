#ifndef TRACE_H
#define TRACE_H

#include <stdio.h>

#define DEBUG 0

/***************************************/
/* TRACE LA GENERATION DES COUPS  */
/***************************************/


//#define TRACE_GENCOUP( t )  		printf(t);fflush(stdout);
//#define TRACE_GENCOUP1( t ,p1)  	printf(t,p1);fflush(stdout);
//#define TRACE_GENCOUP2( t ,p1,p2)  	printf(t,p1,p2);fflush(stdout);


#define TRACE_GENCOUP( t )
#define TRACE_GENCOUP1( t ,p1)
#define TRACE_GENCOUP2( t ,p1,p2)


/**********************************/
/* TRACE DE JOUE ET ANNULE COUP   */
/**********************************/

//#define TRACE_JOUECOUP( t )  		printf(t);fflush(stdout);
//#define TRACE_JOUECOUP1( t ,p1)  	printf(t,p1);fflush(stdout);
//#define TRACE_JOUECOUP2( t ,p1,p2)  	printf(t,p1,p2);fflush(stdout);

#define TRACE_JOUECOUP( t )
#define TRACE_JOUECOUP1( t ,p1)
#define TRACE_JOUECOUP2( t ,p1,p2)

/**********************************/
/* TRACE L'ALGO DE RECHERCHE      */
/**********************************/

//#define TRACE_RECHERCHE( t )  	fprintf(stderr , t);fflush(stdout);
//#define TRACE_RECHERCHE1( t ,p1)  	fprintf(stderr , t,p1);fflush(stdout);
//#define TRACE_RECHERCHE2( t ,p1,p2)  	fprintf(stderr , t,p1,p2);fflush(stdout);
//#define TRACE_RECHERCHE3( t ,p1,p2,p3) 	fprintf(stderr , t,p1,p2,p3);fflush(stdout);

#define TRACE_RECHERCHE( t )
#define TRACE_RECHERCHE1( t ,p1)
#define TRACE_RECHERCHE2( t ,p1,p2)

/************************/
/* TRACE LES HASHTABLES */
/************************/

//#define TRACE_HASH( t )  	printf(t);fflush(stdout);
//#define TRACE_HASH1( t ,p1)  	printf(t,p1);fflush(stdout);
//#define TRACE_HASH2( t ,p1,p2) printf(t,p1,p2);fflush(stdout);

#define TRACE_HASH( t )
#define TRACE_HASH1( t ,p1)
#define TRACE_HASH2( t ,p1,p2)

/**************/
/* TRACE EVAL */
/**************/

//#define TRACE_EVAL( t )  	printf(t);fflush(stdout);
//#define TRACE_EVAL1( t ,p1)  	printf(t,p1);fflush(stdout);
//#define TRACE_EVAL2( t ,p1,p2)  printf(t,p1,p2);fflush(stdout);
//#define TRACE_EVAL3( t ,p1,p2,p3)  printf(t,p1,p2,p3);fflush(stdout);

#define TRACE_EVAL( t )
#define TRACE_EVAL1( t ,p1)
#define TRACE_EVAL2( t ,p1,p2)
#define TRACE_EVAL3( t ,p1,p2,p3)


#endif /* TRACE_H */
