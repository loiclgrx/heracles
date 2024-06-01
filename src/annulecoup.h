#ifndef ANNULECOUP_H
#define ANNULECOUP_H

#include "case.h"
#include "boolean.h"

typedef struct {
	Case ep;
	Boolean isPetitRoque[2];
	Boolean isGrandRoque[2];
} AnnuleCoup;

	/**********/
	/* SETEUR */
	/**********/

#define annulecoup_setEp( a , e )		((a->ep)=e)
#define annulecoup_setPetitRoque( a , b , j )	((a->isPetitRoque)[j]=b)
#define annulecoup_setGrandRoque( a , b , j )	((a->isGrandRoque)[j]=b)

	/**********/
	/* GETEUR */
	/**********/

#define annulecoup_getEp( a )			(a->ep)
#define annulecoup_isPetitRoque( a , j )	((a->isPetitRoque)[j])
#define annulecoup_isGrandRoque( a , j )	((a->isGrandRoque)[j])

#endif /* ANNULECOUP_H */
