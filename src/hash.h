#ifndef HASH_H
#define HASH_H

#include "type.h"
#include "coup.h"
#include "boolean.h"

	/*--------------------------------------------------------*/
	/* DEFINE pour la hash table de detection des repetitions */
	/* ATTENTION CHAQUE DEFINE EST LIE.LA MODIF D'UN, ENTRAINE*/
	/* OBLIGATOIREMENT LA MODIF DES AUTRES 					  */
	/*--------------------------------------------------------*/
#define TAILLE_HASH_REPET	131072	/* 1024 * 128 = 131072
									   10 bits de la cle de la position( 1024 )
									   128 positions rencontrees lors du match (partie+analyse) 
									   Je pars du principe qu'il ne peut pas y avoir plus de 128
									   collisions */
#define NB_POS_MAX		128	/* 150 positions rencontrees lors du match (partie+analyse) 
							   ATTENTION si modif de TAILLE_HASH_REPET modifier egalement
							   NB_POS_MAX */
#define NB_POS_MAX_BIN		7	/* 2^NB_POS_MAX_BIN = NB_POS_MAX */
#define MASK_INDEX_REPET	0x3FFLL

	/*-----------------------------------------------*/
	/* Definition de la hash table pour l'alphabeta	 */
	/*-----------------------------------------------*/

#define HASH_INDEFINI 	0
#define HASH_BETA 	1
#define HASH_ALPHA 	2
#define HASH_EXACT 	3
#define HASH_FEUILLE 	4

#define HASH_NOTHING	5 /* retourner si la positon n'est pas dans la hashTable */
#define HASH_SORT		6 /* retourner si la position est dans Hash et permet de trie les coups */
#define HASH_CUT		7 /* retourner si la position est dans Hash et provoque une coupure */


/* TODO : Modifier le type du score mettre int16 au lieu de int */

/* La hache table est un tableau de PositionHash */

typedef struct {
  int64 cle;		
  Coup	meilleurCoup;	
  sint8	profondeur;	
  int	score; 
  Boolean  flag;	/* HASH_BETA ou HASH_EXACT ou HASH_FEUILLE */
} PositionHash;

#define positionHash_getCle( ph ) 		( ph->cle )
#define positionHash_getCoup( ph ) 		( ph->meilleurCoup )
#define positionHash_getProfondeur( ph ) 	( ph->profondeur )
#define positionHash_getScore( ph ) 		( ph->score )
#define positionHash_getFlag( ph ) 		( ph->flag )

#define hash_getCle( i ) 		( hash.ph[i].cle )
#define hash_getCoup( i ) 		( hash.ph[i].meilleurCoup )
#define hash_getProfondeur( i ) 	( hash.ph[i].profondeur )
#define hash_getScore( i ) 		( hash.ph[i].score )
#define hash_getFlag( i ) 		( hash.ph[i].flag )
#define hash_getTaille	 		  hash.taille 
#define hash_getMaskIndex		  hash.maskIndex 
#define hash_getTauxRemplissage 	  hash.tauxRemplissage 
#define hash_getNbCollisions	 	  hash.nbCollisions 
#define hash_getNbRejet		 	  hash.nbRejet 
#define hash_setCle( i , c ) 		( hash.ph[i].cle = c )
#define hash_setMeilleurCoup( i , c ) 	( hash.ph[i].meilleurCoup = c )
#define hash_setProfondeur( i , d ) 	( hash.ph[i].profondeur = d )
#define hash_setScore( i , s ) 		( hash.ph[i].score = s )
#define hash_setFlag( i , f ) 		( hash.ph[i].flag = f )
#define hash_setMaskIndex( m ) 		( hash.maskIndex = m )
#define hash_setTaille( t ) 		( hash.taille = t )
#define hash_setTauxRemplissage( t )  	( hash.tauxRemplissage = t )
#define hash_setNbCollisions( t )  	( hash.nbCollisions = t )
#define hash_setNbRejet( t )  		( hash.nbRejet = t )
#define hash_incrementeTauxRemplissage 	( hash.tauxRemplissage++ )
#define hash_incrementeNbCollisions 	( hash.nbCollisions++ )
#define hash_incrementeNbRejet	 	( hash.nbRejet++ )

/* Table de hachage pour stocker les positions rencontrees lors de la recherche:
 *  Le malloc est réalisé dans la fonction hash_creer */

typedef struct {
	PositionHash 	*ph;		/* Liste des positions */
	int	 			taille;		/* Taile de la hache table ( puissance de 2 ) */
	int64			maskIndex;	/* Mask qui permet de determine l'index de la position
									a partir de la cle */
	int				tauxRemplissage;	
	int				nbCollisions;
	int				nbRejet;
} Hash;

extern Hash hash;

/* Return Vrai si pas de problème lors de la création */
extern Boolean hash_creer( int tailleMB );

/* Desalloue la zone memoire de la table de hachage */
extern void hash_free();

/* Return Vrai si pas de problème lors de l'effacement */
extern Boolean hash_effacer( );

/* Return l'index de la position dans la table */
extern int hash_put( int64 cle , Coup co , sint8 prof , int score , int flag );

/* Return soit HASH_CUT ou HASH_SORT */
extern int hash_get( int64 cle , int alpha , int beta ,short profondeur ,PositionHash **ph );

extern int hash_getHash( int64 cle , short profondeur );

	/*---------------------------------------------------------------*/
	/* Definition de la hash table pour la detection des repetitions */
	/*---------------------------------------------------------------*/

/* Table de hachage pour memoriser le nombre de fois qu'une position a ete rencontree afin de
 *  detecter les parties nulles pour cause de repetition */
typedef struct {
	int64 cle;
	short nb;
} PositionRepetition;

typedef struct {
	PositionRepetition posRepet[TAILLE_HASH_REPET]; 
							/* va contenir l'ensemble des positions jouer
							    et celle en cours d'analyse. 65536 
							    positions pour environ 50 positions 
							    afin de reduire les collisions.*/
	short maskRepet;
} HashRepetition;

extern HashRepetition hashRepet;

/* Return Vrai si pas de problème lors de l'effacement */
extern Boolean hashRepet_effacer( );

/* Stocke la cle dans la hash table. Si elle existe deja incremente de 1 sa valeur nb.
 * retourne l'index de la cle dans la hash table */
extern int hashRepet_put( int64 cle );

/* Decremente de 1 le nombre de fois que la position a ete rencontree */
extern void hashRepet_remove( int64 cle );

/* Return le nombre de fois que la cle a ete rencontre lors de la partie. */
extern short hashRepet_get( int64 cle );

/* test si le contenu de la table de hachage est correct 
 * pas de pos avec un nb < 0 et > prof*/ 
extern void hashRepet_test( int profondeur );

		/*-------------------------------------------------*/
		/* Structure pour tester la repetition de position */
		/*-------------------------------------------------*/

typedef struct {
	PositionRepetition posRepet[500]; 
	short nbPos;
} DebugRepetition;

/*  Tableau pour tester la version Hash de la repetition de positions */
extern DebugRepetition debugRepet;


extern void debugRepet_clear(  );
extern void debugRepet_put( int64 cle ) ;
extern void debugRepet_remove( int64 cle );
extern Boolean debugRepet_get( int64 cle );

#endif
