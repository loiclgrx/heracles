#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include "epd.h"
#include "piece.h"
#include "couleur.h"

/*----------------------------------------------------------------------------*/ 
FILE*  loadEpd( char* fichier ) {
	return fopen( fichier , "r" );
}

/*----------------------------------------------------------------------------*/
int	nextEpd( FILE *hdl , Epd *e ) {
	char epd[1000];

	if ( !lireLigneFichier( hdl , epd , 800 ) ) return FAUX;

	printf( "%s\n" , epd );
}

/*----------------------------------------------------------------------------*/
int			unloadEpd( FILE *hdl ) {
	return fclose( hdl );
}

/*----------------------------------------------------------------------------*/

int espaceToFinChaine( char * s ) {
	char *p;
	int nbMot = 0;
	
	p=s;
	
	while (*p!='\0') {
		if (*p==' ') { *p='\0'; nbMot++; }
		p++;
	}
	return nbMot+1;
}

/*-----------------------------------------------------------------------------------------*/

void lirePosition( Epd *epd , char *pos) {
	char *p;
	short c,l,i,j;
	Piece piece;
	
	piece_setNom( piece, PASDEPIECE );
	piece_setCouleur( piece , NOIR );

	for(i=0;i<8;i++)
		for(j=0;j<8;j++)
				(*epd).echiquier[i][ j ] = piece;
	c =0;
	l =7;
	
	p=pos;

	while (*p!='\0') {		
		if (*p>='1' && *p<='8') {c+=(*p-'0'); p++  ;continue; }
		if (*p=='/') 				  {c=0; l--; p++;  continue; }
		
		if (*p=='k') piece_setNom( piece, ROI);
		if (*p=='q') piece_setNom( piece, DAME);
		if (*p=='b') piece_setNom( piece, FOU);
		if (*p=='n') piece_setNom( piece, CAVALIER);		
		if (*p=='r') piece_setNom( piece, TOUR);		
		if (*p=='p') piece_setNom( piece, PION);								
		
		if (*p=='K') piece_setNom( piece, ROI);
		if (*p=='Q') piece_setNom( piece, DAME);
		if (*p=='B') piece_setNom( piece, FOU);
		if (*p=='N') piece_setNom( piece, CAVALIER);		
		if (*p=='R') piece_setNom( piece, TOUR);		
		if (*p=='P') piece_setNom( piece, PION);								
		
		if (*p>='a' && *p<='z') piece_setCouleur( piece , NOIR );
		if (*p>='A' && *p<='Z') piece_setCouleur( piece , BLANC );		
		
		(*epd).echiquier[c][ l ] = piece;

		p++; c++;
	}

}

/*-----------------------------------------------------------------------------------------*/

void lireCouleur( Epd *epd , char *co) {
	
	if (*co=='w') (*epd).cote = BLANC; else (*epd).cote = NOIR;
}

/*-----------------------------------------------------------------------------------------*/
void lireRoque( Epd *epd , char *ro) {
   char *p;

     (*epd)	.petitRoque[ BLANC ] = FAUX;
     (*epd)	.petitRoque[ NOIR ] = FAUX;
     (*epd)	.grandRoque[ BLANC ] = FAUX;
     (*epd)	.grandRoque[ NOIR ] = FAUX;

   if (*ro=='-')  	return;

  p=ro;

  while (*p!='\0') {
	if (*p=='K')      (*epd).petitRoque[ BLANC ] = VRAI;
	if (*p=='Q')     (*epd).grandRoque[ BLANC ] = VRAI;	
	if (*p=='k')      (*epd).petitRoque[ NOIR ] = VRAI;
	if (*p=='q')     (*epd).grandRoque[ NOIR ] = VRAI;		
	p++;
  }

}

/*-----------------------------------------------------------------------------------------*/

void lireEnPassant( Epd *epd , char *ep ) {


	if ( *ep=='-') (*epd).ep =CASEVIDE; else 	(*epd).ep = case_getNomToCase( ep );

}

/*-----------------------------------------------------------------------------------------*/

void initEpd( Epd *epd ,char * sepd ) {
	char *p;

	espaceToFinChaine(sepd);

	lirePosition( epd , sepd );

	p= sepd + strlen( sepd ) + 1;
	lireCouleur( epd , p );	
	
	p+=strlen( p ) + 1;
	lireRoque( epd , p );		
	
	p+=strlen( p ) + 1;
	lireEnPassant( epd , p );	
}
