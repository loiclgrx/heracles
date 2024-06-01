#include "valeur.h"
#include "piece.h"

short valeurPiece[7];
short valeurPaireFou;


void initValeurPiece() {
  valeurPiece[ PION ] = 100; 
  valeurPiece[ CAVALIER ] = 325; 
  valeurPiece[ FOU ] = 325; 
  valeurPiece[ TOUR ] = 500; 
  valeurPiece[ DAME ] = 975; 
  valeurPiece[ ROI ] = 0;  
  valeurPaireFou	 = 50;

	// Avant
/*  valeurPiece[ PION ] = 100; 
  valeurPiece[ CAVALIER ] = 300; 
  valeurPiece[ FOU ] = 300; 
  valeurPiece[ TOUR ] = 500; 
  valeurPiece[ DAME ] = 900; 
  valeurPiece[ ROI ] = 0; */
}

void initValeur() {
  initValeurPiece();
}
