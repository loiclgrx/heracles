#ifndef RECHERCHE_H
#define RECHERCHE_H

#include <stdio.h>
#include "timer.h"
#include "listecoup.h"

extern long 		nbFeuilles;  // Nombre de noeuds analysees lors de la recherche


typedef struct {
	Boolean genEpd;		/* A la fin de chaque coup Heracles gerenere la position analysee
						   au format fen et son meilleur coup */
	FILE*	fileEpd;	/* descripteur du fichier fichierEpd */
} InfoRecherche;


typedef struct {
	int	numeroCoup;			// indique le numero du coup en cours de recherche
	int bestCoup;			// meilleur coup actuel
	int bestScore;			// donne le meilleur score actuel
	int numeroBestCoup;		// donne le numero du meilleur coup 
	int profondeur;			// indique le profondeur courant de l'iterate deepening
	long nbNoeudsTotal;		// nombre total de noeuds examinés lors de la recherche
	long nbNoeudsCoup;		// nombre de noeuds examinés pour le coup actuel ( pour le trie )
} InfoRechercheRacine;

extern InfoRecherche infoRecherche;
extern InfoRechercheRacine infoRechercheRacine;

Coup iterateDeepening( Plateau *p , GestionTemps *gt , int *score);

void rechercheRacine( Plateau *p , GestionTemps *gt , int alpha , int beta ); 
void initStats();
void afficheStats();



int alphaBeta( Plateau *p , short profondeur , int alpha , int beta , Couleur c , ListeCoup *pmv ,
	       GestionTemps *gt ,Boolean nullMoveAutorise );

long minmax( Plateau *p, short profondeur , Couleur co , Coup *c ,GestionTemps *gt  );

typedef enum { HASH , AUTRE_COUP , PLUS_DE_COUP } OrdreGenerationCoup;

#define recherche_getNbNoeuds		infoRechercheRacine.nbNoeudsTotal

#define MOINS_INFINI -32500
#define PLUS_INFINI   32500

#undef STATRECHERCHE

#ifdef STATRECHERCHE
  #define positionMeilleurCoup( t , n ) t[n]++;
	#define nbAppelGenCoup( t )						t++;
  #define STOCKE_DELTASCORE( bestScore , prof )										\
			if ( prof == 1 ) scoreProfPrec = bestScore; 								\
			else { 																											\
				if ( abs(scoreProfPrec -  bestScore) >= 100 )							\
					deltaScore[ 100  ]++;																		\
				else {																										\
					deltaScore[  abs(scoreProfPrec - bestScore) ]++; 		\
				}																													\
				scoreProfPrec = bestScore;																\
			}																						
#else
	#define positionMeilleurCoup( t , n ) 
	#define nbAppelGenCoup( t )
  #define STOCKE_DELTASCORE( bestScore , prof )	
#endif

#endif /* RECHERCHE_H */
