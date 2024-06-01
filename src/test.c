#include <stdio.h>
#include <string.h>
#include "coup.h"
#include "plateau.h"
#include "generecoup.h"
#include "listecoup.h"
#include "couleur.h"
#include "test.h"
#include "annulecoup.h"
#include "bitboard.h"
#include "trace.h"
#include "valeur.h"
#include "evaluation.h"
#include "init.h"
#include "timer.h"
#include "recherche.h"
#include "util.h"



#define NBTEST  126
#define MAXPERFT 6

long perftTest[NBTEST][MAXPERFT];
char *epdTest[NBTEST];

char lCoup[2000];
char coup[10];
char fen[100];	/* POUR DEBUG => A SUPPRIMER */

//long nbCapture = 0;
int64	  nbNoeudsTotal;  // Pour chaque test compte le nombre de noeuds et le temps total.
long	  tempsTotal;

void initTest();


void testAlphaBeta( Plateau *p , GestionTemps *gt  ) {
	int		  j,scoreAlphaBeta,scoreMinMax;
	Coup	  coupAlphaBeta,coupMinMax;
	int		  prof;


	prof = timer_getProfondeur( gt ) + 1;

    for ( j=1 ; j < prof ; j++ ) {
	  fprintf(stderr,"depth %d : " , j );
	  timer_setProfondeur( gt , j );
	  hash_effacer();
      hashRepet_effacer();

	  coupAlphaBeta = iterateDeepening( p , gt , &scoreAlphaBeta );
	  printCoup( coupAlphaBeta , coup );
	  fprintf(stderr,"AlphaBeta %s (%d) ", coup , scoreAlphaBeta );
#if 0
	  timer_setTempsMoyenTheorique( gt , TEMPS_INFINI ); 
	  scoreMinMax = minmax( p, j , plateau_getCoteCourant(p) , &coupMinMax ,gt);
	  printCoup( coupMinMax , coup );
	  fprintf(stderr,"MinMax %s (%d) ", coup , scoreMinMax );

	  if ( scoreMinMax != scoreAlphaBeta ) 
		  fprintf(stderr , "\t[ ERROR ]\n");
	  else
#endif
		  fprintf(stderr,"\n");

	  fflush( stderr );
    }
}
/*-----------------------------------------------------------------------*/

void compteNbNoeud( Plateau *p , GestionTemps *gt ) {
    int		  scoreAlphaBeta;
	Coup	  coupAlphaBeta;
	MonTimer  mt;

	timer_reset( mt );
	timer_start( mt );

    coupAlphaBeta = iterateDeepening( p , gt , &scoreAlphaBeta );

	timer_stop( mt );

	nbNoeudsTotal += recherche_getNbNoeuds;
	tempsTotal	  += (int)timer_delta(mt);

	plateau_getFen( p , fen );
	printCoup( coupAlphaBeta , coup );
	fprintf(stderr, "%s %s %d %d %ld\n" , fen , coup , scoreAlphaBeta ,(int)timer_delta(mt) , recherche_getNbNoeuds );
	fflush( stderr );

}

/*-----------------------------------------------------------------------*/

void lanceTest( Plateau *p ,GestionTemps* gt , char *fichier , Boolean isFen , void (*f)( Plateau * ,GestionTemps*)  ) {
	FILE	*fileHdl;
	char fen[1000];
	char epd[1000];
	Epd	 e;
	int i;

	nbNoeudsTotal = tempsTotal = 0;

	if ( isFen ) {

		fileHdl =loadEpd( fichier );

		if ( fileHdl == NULL ) {
			printf("Impossible d'ouvrir le fichier\n");
			return;
		}
		i=0;
		while ( lireLigneFichier( fileHdl , epd , 900 ) ) {
		  strcpy( fen , epd );
		  initEpd( &e , fen );
		  plateau_setPosition( p , e );
		  fprintf(stderr,"\nPosition %d : %s\n", i , epd ); fflush( stdout);

   		  hash_effacer();
		  hashRepet_effacer();

		  f( p , gt );
		  i++;
		}
		unloadEpd( fileHdl );
	}
	else {
		initTest();
		for ( i=10 ; i<NBTEST ; i++ ) {
		  strcpy( fen , epdTest[i] );
		  initEpd( &e , fen );
		  plateau_setPosition( p , e );
		  fprintf(stderr,"\nPosition %d : %s\n", i , epdTest[i] ); 

   		  hash_effacer();
		  hashRepet_effacer();

		  f( p , gt );
		}
	}

	fprintf(stderr, "\nNoeuds examinés : %lld\n", nbNoeudsTotal );
	fprintf(stderr, "Temps total : %ld ms \n", tempsTotal );

	fflush( stderr );

	#ifdef STATRECHERCHE
	     afficheStats();
	#endif 

	

}

/*-----------------------------------------------------------------------*/

int compteCapture( ListeCoup *lc ) {
	int i,nb=0;

	for(i=0;i<listecoup_getNbCoup(lc);i++) 
	  if ( coup_isCapture( listecoup_getCoup(lc,i) ) ) nb++;	

	return nb;
}

/*-----------------------------------------------------------------------*/

long perft( Plateau *p, short profondeur  ) {
  register int i;
  ListeCoup lc;
  //ListeCoup lcCapture;
  AnnuleCoup undo;
  long nbFeuilles = 0;

  
  		#if DEBUG
		  Plateau p1,p2;
		  char fen[100];
  		  char coup[10];
		#endif

  if ( profondeur== 0 )  return 1; 

  listecoup_videListeCoup(&lc);

  //listecoup_videListeCoup(&lcCapture);
  
		#if DEBUG  
		     p1 = *p;
		#endif

  
  genereCoup( &lc , p , plateau_getCoteCourant(p) );

#if 0
	/* Test le generateur de capture à partir du generateur global
       Le generateur de captures ne gere pas les positions en echec
	   => pas de comparaison dans ce cas la */

  	/* si pas en echec compare le nombre de captures */
  if ( plateau_pieceAttaqueCase( p, plateau_getEmplacementRoi( p , co ) ,
		       		    couleur_oppose( co ) ) ) {
	  ;
  } 
  else { 
	  genereCoupCapture( &lcCapture , p , co );
	  if ( listecoup_getNbCoup( &lcCapture ) != compteCapture( &lc ) ) {
		 printf(" %d contre %d \n",listecoup_getNbCoup( &lcCapture ),compteCapture( &lc ));
		 plateau_getFen( p , fen ); 
		 printListeCoup( &lcCapture , lCoup );
		 printf("fen : %s\n",fen );
		 printf("Gencapture : %s\n", lCoup );
		 fflush(stdout);
		 exit(0);
	  } else nbCapture+=listecoup_getNbCoup( &lcCapture );
  }
#endif

  
		#if DEBUG  
		     if ( comparePlateau(p,&p1) ) { 
			     printf("Erreur sur la generation de coup\n"); 
			     exit(0);
		     }
		#endif
  for ( i=0 ; i<listecoup_getNbCoup(&lc) ; i++ ) {

		#if DEBUG
		     p1 = *p;
		    /* printf("profondeur %d - %d/%d : ",5-profondeur,i,listecoup_getNbCoup(&lc)); */

		     plateau_getFen(p,fen);
		     printf("%d %s bm  ",profondeur,fen);
		     printCoup(listecoup_getCoup( &lc , i) , coup );
		     printf("%s ",coup);
		#endif

     plateau_joueCoup( p , listecoup_getCoup( &lc , i) , &undo );
     		#if DEBUG
		       printf(" %lld\n", p->cle);
     			p2=*p;
     		#endif
     nbFeuilles +=perft( p , profondeur-1  );

     plateau_annuleCoup( p , listecoup_getCoup( &lc , i ) , &undo );


     		#if DEBUG
		        printf(" %lld\n", p->cle);
		     if ( comparePlateau(p,&p1) ) { 
			     printf("Erreur sur joue et annule coup\n"); 
			  /*   printf("position initiale\n"); 
		     	     printPlateau(&p1);
			     printf("position apres le coup\n"); 
		     	     printPlateau(&p2);
			     printf("\nposition apres avoir annuler le coup\n"); 
		     	     printPlateau(p); */
			     exit(0);
		     } 
			
		     int j,materielBlanc,materielNoir;
		     Piece pi;

		     materielBlanc = materielNoir = 0;

		     for( j=A1 ; j<=H8 ; j++ ) {
			     pi = plateau_getPiece(p,j); 
			     if ( piece_getNom(pi) != PASDEPIECE ) {
				     if ( piece_getCouleur(pi) == BLANC )
					     materielBlanc += getValeurPiece( piece_getNom(pi) );
				     else
					     materielNoir += getValeurPiece( piece_getNom(pi) );
			     }
		     }

		     if ( (p->materiel[BLANC] != materielBlanc) || (p->materiel[NOIR] != materielNoir) ) {
			printf("Erreur sur le calcul du materiel\n");     
			printf("Valeur du plateau BLANC %d NOIR %d\n",p->materiel[BLANC],p->materiel[NOIR]);     
			printf("Valeur recalcule BLANC %d NOIR %d\n",materielBlanc,materielNoir);     
			exit(0);
		     }
		     
		#endif
  } 

  return nbFeuilles;

}

/*-------------------------------------------------------------------------------------*/
/* Pratique pour reperer progressivement la position qui pose probleme au
   generateur de coups.
   */
long perftDivise( Plateau *p, short profondeur ) {
  int i;
  long nbFeuilles=0,nbFeuillesTotal=0;
  ListeCoup lc;
  AnnuleCoup undo;

  listecoup_videListeCoup(&lc);

  genereCoup( &lc , p , plateau_getCoteCourant(p) );

  for ( i=0 ; i<listecoup_getNbCoup(&lc) ; i++ ) {

     nbFeuilles = 0 ;
     plateau_joueCoup( p , listecoup_getCoup( &lc , i) , &undo );
 
     nbFeuilles +=perft( p , profondeur-1 );
     nbFeuillesTotal += nbFeuilles;

     plateau_annuleCoup( p , listecoup_getCoup( &lc , i ) , &undo );

	 printCoup( listecoup_getCoup( &lc , i) , coup );
	 printf("%s %ld\n", coup , nbFeuilles );
  } 

  return nbFeuillesTotal;
}

/*-------------------------------------------------------------------------------------*/

long perftHash( Plateau *p, short profondeur ) {
  register int i;
  long nbFeuilles=0;
  ListeCoup lc;
  AnnuleCoup undo;

  if (profondeur == 0 ) return 1;

  nbFeuilles = hash_getHash( plateau_getCle( p ) , profondeur );

  if ( nbFeuilles != 0 ) return nbFeuilles;

  listecoup_videListeCoup(&lc);

  genereCoup( &lc , p , plateau_getCoteCourant(p) );

  for ( i=0 ; i<listecoup_getNbCoup(&lc) ; i++ ) {

     plateau_joueCoup( p , listecoup_getCoup( &lc , i) , &undo );
 
     nbFeuilles +=perftHash( p , profondeur-1 );

     plateau_annuleCoup( p , listecoup_getCoup( &lc , i ) , &undo );
  } 

  hash_put( plateau_getCle( p ) , PAS_DE_COUP , profondeur , nbFeuilles , HASH_FEUILLE );
  return nbFeuilles;
}

/*-------------------------------------------------------------------------------------*/


void perf( Plateau *p ) {
  long i,j;
  ListeCoup lc;
  AnnuleCoup undo;
  char fen[60]= "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Epd epd;

  
  initEpd( &epd , fen ); 
  
  plateau_setPosition( p, epd );
  listecoup_videListeCoup(&lc);

  for( i=0 ; i<4000000 ; i++ ) { 
	genereCoup( &lc , p ,BLANC  );  
	for( j=0 ; j<listecoup_getNbCoup(&lc) ; j++ ) {
		plateau_joueCoup(   p , listecoup_getCoup( &lc , j ) , &undo );
		plateau_annuleCoup( p , listecoup_getCoup( &lc , j ) , &undo );
	}
	listecoup_videListeCoup(&lc);
  }

}
/*---------------------------------------------------------------------------------------*/

void testGenerateurCoup( Plateau *p ) {
	Epd e;
    int i,j;
	long nbNoeud;
	char fen[100];
	MonTimer tps;
	MonTimer tpsTotal;
	int minute,seconde;

	initTest();

    timer_reset( tpsTotal );
    timer_start( tpsTotal );

	for ( i=0 ; i<NBTEST ; i++ ) {
	  strcpy( fen , epdTest[i] );
      initEpd( &e , fen );
	  plateau_setPosition( p , e );
	
	  timer_pause( tpsTotal );
	  minute  = (timer_delta( tpsTotal )/1000)/60;
	  seconde = (timer_delta( tpsTotal ) - (minute * 60000))/1000;

	  printf("\nPosition (%d/%d) : time %d mn %d s\n", i+1 , NBTEST,minute,seconde );
	  printf("Fen : %s\n",  epdTest[i] );

	  for ( j=0 ; j < MAXPERFT ; j++ ) {
		  if ( perftTest[i][j] == 0 ) continue;
   	      printf("Perft %d : %ld ", j+1 , perftTest[i][j] );

  		  timer_reset( tps );
		  timer_start( tps );

		  nbNoeud = perftHash( p , j+1 );
		  
		  timer_stop( tps );

		  if ( nbNoeud > 9999 ) printf("\t"); else printf("\t\t");

		  if ( nbNoeud == perftTest[i][j] )
			  printf("[ OK ]\ttime : %8.3f s\n",((float)timer_delta( tps )/1000));
		  else {
			  printf("[ ERROR ]  %ld\n",nbNoeud);
			  exit(1);
		  }
	  }
	}
	timer_stop( tpsTotal );

    minute  = (timer_delta( tpsTotal )/1000)/60;
    seconde = (timer_delta( tpsTotal ) - (minute * 60000))/1000;

	printf("\nTime : %d mn %d s\n", minute,seconde );
}

/*---------------------------------------------------------------------------------------*/

void initTest( ) {

epdTest[0]="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
epdTest[1]="r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
epdTest[2]="4k3/8/8/8/8/8/8/4K2R w K - 0 1";
epdTest[3]="4k3/8/8/8/8/8/8/R3K3 w Q - 0 1";
epdTest[4]="4k2r/8/8/8/8/8/8/4K3 w k - 0 1";
epdTest[5]="r3k3/8/8/8/8/8/8/4K3 w q - 0 1";
epdTest[6]="4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1";
epdTest[7]="r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1";
epdTest[8]="8/8/8/8/8/8/6k1/4K2R w K - 0 1";
epdTest[9]="8/8/8/8/8/8/1k6/R3K3 w Q - 0 1";
epdTest[10]="4k2r/6K1/8/8/8/8/8/8 w k - 0 1";
epdTest[11]="r3k3/1K6/8/8/8/8/8/8 w q - 0 1";
epdTest[12]="r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
epdTest[13]="r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1";
epdTest[14]="r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1";
epdTest[15]="r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1";
epdTest[16]="1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1";
epdTest[17]="2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1";
epdTest[18]="r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1";
epdTest[19]="4k3/8/8/8/8/8/8/4K2R b K - 0 1";
epdTest[20]="4k3/8/8/8/8/8/8/R3K3 b Q - 0 1";
epdTest[21]="4k2r/8/8/8/8/8/8/4K3 b k - 0 1";
epdTest[22]="r3k3/8/8/8/8/8/8/4K3 b q - 0 1";
epdTest[23]="4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1";
epdTest[24]="r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1";
epdTest[25]="8/8/8/8/8/8/6k1/4K2R b K - 0 1";
epdTest[26]="8/8/8/8/8/8/1k6/R3K3 b Q - 0 1";
epdTest[27]="4k2r/6K1/8/8/8/8/8/8 b k - 0 1";
epdTest[28]="r3k3/1K6/8/8/8/8/8/8 b q - 0 1";
epdTest[29]="r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1";
epdTest[30]="r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1";
epdTest[31]="r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1";
epdTest[32]="r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1";
epdTest[33]="1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1";
epdTest[34]="2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1";
epdTest[35]="r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1";
epdTest[36]="8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1";
epdTest[37]="8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1";
epdTest[38]="8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1";
epdTest[39]="K7/8/2n5/1n6/8/8/8/k6N w - - 0 1";
epdTest[40]="k7/8/2N5/1N6/8/8/8/K6n w - - 0 1";
epdTest[41]="8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1";
epdTest[42]="8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1";
epdTest[43]="8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1";
epdTest[44]="K7/8/2n5/1n6/8/8/8/k6N b - - 0 1";
epdTest[45]="k7/8/2N5/1N6/8/8/8/K6n b - - 0 1";
epdTest[46]="B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1";
epdTest[47]="8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1";
epdTest[48]="k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1";
epdTest[49]="K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1";
epdTest[50]="B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1";
epdTest[51]="8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1";
epdTest[52]="k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1";
epdTest[53]="K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1";
epdTest[54]="7k/RR6/8/8/8/8/rr6/7K w - - 0 1";
epdTest[55]="R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1";
epdTest[56]="7k/RR6/8/8/8/8/rr6/7K b - - 0 1";
epdTest[57]="R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1";
epdTest[58]="6kq/8/8/8/8/8/8/7K w - - 0 1";
epdTest[59]="6KQ/8/8/8/8/8/8/7k b - - 0 1";
epdTest[60]="K7/8/8/3Q4/4q3/8/8/7k w - - 0 1";
epdTest[61]="6qk/8/8/8/8/8/8/7K b - - 0 1";
epdTest[62]="6KQ/8/8/8/8/8/8/7k b - - 0 1";
epdTest[63]="K7/8/8/3Q4/4q3/8/8/7k b - - 0 1";
epdTest[64]="8/8/8/8/8/K7/P7/k7 w - - 0 1";
epdTest[65]="8/8/8/8/8/7K/7P/7k w - - 0 1";
epdTest[66]="K7/p7/k7/8/8/8/8/8 w - - 0 1";
epdTest[67]="7K/7p/7k/8/8/8/8/8 w - - 0 1";
epdTest[68]="8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1";
epdTest[69]="8/8/8/8/8/K7/P7/k7 b - - 0 1";
epdTest[70]="8/8/8/8/8/7K/7P/7k b - - 0 1";
epdTest[71]="K7/p7/k7/8/8/8/8/8 b - - 0 1";
epdTest[72]="7K/7p/7k/8/8/8/8/8 b - - 0 1";
epdTest[73]="8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1";
epdTest[74]="8/8/8/8/8/4k3/4P3/4K3 w - - 0 1";
epdTest[75]="4k3/4p3/4K3/8/8/8/8/8 b - - 0 1";
epdTest[76]="8/8/7k/7p/7P/7K/8/8 w - - 0 1";
epdTest[77]="8/8/k7/p7/P7/K7/8/8 w - - 0 1";
epdTest[78]="8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1";
epdTest[79]="8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1";
epdTest[80]="8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1";
epdTest[81]="k7/8/3p4/8/3P4/8/8/7K w - - 0 1";
epdTest[82]="8/8/7k/7p/7P/7K/8/8 b - - 0 1";
epdTest[83]="8/8/k7/p7/P7/K7/8/8 b - - 0 1";
epdTest[84]="8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1";
epdTest[85]="8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1";
epdTest[86]="8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1";
epdTest[87]="k7/8/3p4/8/3P4/8/8/7K b - - 0 1";
epdTest[88]="7k/3p4/8/8/3P4/8/8/K7 w - - 0 1";
epdTest[89]="7k/8/8/3p4/8/8/3P4/K7 w - - 0 1";
epdTest[90]="k7/8/8/7p/6P1/8/8/K7 w - - 0 1";
epdTest[91]="k7/8/7p/8/8/6P1/8/K7 w - - 0 1";
epdTest[92]="k7/8/8/6p1/7P/8/8/K7 w - - 0 1";
epdTest[93]="k7/8/6p1/8/8/7P/8/K7 w - - 0 1";
epdTest[94]="k7/8/8/3p4/4p3/8/8/7K w - - 0 1";
epdTest[95]="k7/8/3p4/8/8/4P3/8/7K w - - 0 1";
epdTest[96]="7k/3p4/8/8/3P4/8/8/K7 b - - 0 1";
epdTest[97]="7k/8/8/3p4/8/8/3P4/K7 b - - 0 1";
epdTest[98]="k7/8/8/7p/6P1/8/8/K7 b - - 0 1";
epdTest[99]="k7/8/7p/8/8/6P1/8/K7 b - - 0 1";
epdTest[100]="k7/8/8/6p1/7P/8/8/K7 b - - 0 1";
epdTest[101]="k7/8/6p1/8/8/7P/8/K7 b - - 0 1";
epdTest[102]="k7/8/8/3p4/4p3/8/8/7K b - - 0 1";
epdTest[103]="k7/8/3p4/8/8/4P3/8/7K b - - 0 1";
epdTest[104]="7k/8/8/p7/1P6/8/8/7K w - - 0 1";
epdTest[105]="7k/8/p7/8/8/1P6/8/7K w - - 0 1";
epdTest[106]="7k/8/8/1p6/P7/8/8/7K w - - 0 1";
epdTest[107]="7k/8/1p6/8/8/P7/8/7K w - - 0 1";
epdTest[108]="k7/7p/8/8/8/8/6P1/K7 w - - 0 1";
epdTest[109]="k7/6p1/8/8/8/8/7P/K7 w - - 0 1";
epdTest[110]="3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1";
epdTest[111]="7k/8/8/p7/1P6/8/8/7K b - - 0 1";
epdTest[112]="7k/8/p7/8/8/1P6/8/7K b - - 0 1";
epdTest[113]="7k/8/8/1p6/P7/8/8/7K b - - 0 1";
epdTest[114]="7k/8/1p6/8/8/P7/8/7K b - - 0 1";
epdTest[115]="k7/7p/8/8/8/8/6P1/K7 b - - 0 1";
epdTest[116]="k7/6p1/8/8/8/8/7P/K7 b - - 0 1";
epdTest[117]="3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1";
epdTest[118]="8/Pk6/8/8/8/8/6Kp/8 w - - 0 1";
epdTest[119]="n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1";
epdTest[120]="8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1";
epdTest[121]="n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1";
epdTest[122]="8/Pk6/8/8/8/8/6Kp/8 b - - 0 1";
epdTest[123]="n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1";
epdTest[124]="8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1";
epdTest[125]="n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1";

perftTest[0][0]=20; perftTest[0][1]=400; perftTest[0][2]=8902; perftTest[0][3]=197281; perftTest[0][4]=4865609; perftTest[0][5]=119060324;
perftTest[1][0]=48; perftTest[1][1]=2039; perftTest[1][2]=97862; perftTest[1][3]=4085603; perftTest[1][4]=193690690; perftTest[1][5]=0;
perftTest[2][0]=15; perftTest[2][1]=66; perftTest[2][2]=1197; perftTest[2][3]=7059; perftTest[2][4]=133987; perftTest[2][5]=764643;
perftTest[3][0]=16; perftTest[3][1]=71; perftTest[3][2]=1287; perftTest[3][3]=7626; perftTest[3][4]=145232; perftTest[3][5]=846648;
perftTest[4][0]=5; perftTest[4][1]=75; perftTest[4][2]=459; perftTest[4][3]=8290; perftTest[4][4]=47635; perftTest[4][5]=899442;
perftTest[5][0]=5; perftTest[5][1]=80; perftTest[5][2]=493; perftTest[5][3]=8897; perftTest[5][4]=52710; perftTest[5][5]=1001523;
perftTest[6][0]=26; perftTest[6][1]=112; perftTest[6][2]=3189; perftTest[6][3]=17945; perftTest[6][4]=532933; perftTest[6][5]=2788982;
perftTest[7][0]=5; perftTest[7][1]=130; perftTest[7][2]=782; perftTest[7][3]=22180; perftTest[7][4]=118882; perftTest[7][5]=3517770;
perftTest[8][0]=12; perftTest[8][1]=38; perftTest[8][2]=564; perftTest[8][3]=2219; perftTest[8][4]=37735; perftTest[8][5]=185867;
perftTest[9][0]=15; perftTest[9][1]=65; perftTest[9][2]=1018; perftTest[9][3]=4573; perftTest[9][4]=80619; perftTest[9][5]=413018;
perftTest[10][0]=3; perftTest[10][1]=32; perftTest[10][2]=134; perftTest[10][3]=2073; perftTest[10][4]=10485; perftTest[10][5]=179869;
perftTest[11][0]=4; perftTest[11][1]=49; perftTest[11][2]=243; perftTest[11][3]=3991; perftTest[11][4]=20780; perftTest[11][5]=367724;
perftTest[12][0]=26; perftTest[12][1]=568; perftTest[12][2]=13744; perftTest[12][3]=314346; perftTest[12][4]=7594526; perftTest[12][5]=179862938;
perftTest[13][0]=25; perftTest[13][1]=567; perftTest[13][2]=14095; perftTest[13][3]=328965; perftTest[13][4]=8153719; perftTest[13][5]=195629489;
perftTest[14][0]=25; perftTest[14][1]=548; perftTest[14][2]=13502; perftTest[14][3]=312835; perftTest[14][4]=7736373; perftTest[14][5]=184411439;
perftTest[15][0]=25; perftTest[15][1]=547; perftTest[15][2]=13579; perftTest[15][3]=316214; perftTest[15][4]=7878456; perftTest[15][5]=189224276;
perftTest[16][0]=26; perftTest[16][1]=583; perftTest[16][2]=14252; perftTest[16][3]=334705; perftTest[16][4]=8198901; perftTest[16][5]=198328929;
perftTest[17][0]=25; perftTest[17][1]=560; perftTest[17][2]=13592; perftTest[17][3]=317324; perftTest[17][4]=7710115; perftTest[17][5]=185959088;
perftTest[18][0]=25; perftTest[18][1]=560; perftTest[18][2]=13607; perftTest[18][3]=320792; perftTest[18][4]=7848606; perftTest[18][5]=190755813;
perftTest[19][0]=5; perftTest[19][1]=75; perftTest[19][2]=459; perftTest[19][3]=8290; perftTest[19][4]=47635; perftTest[19][5]=899442;
perftTest[20][0]=5; perftTest[20][1]=80; perftTest[20][2]=493; perftTest[20][3]=8897; perftTest[20][4]=52710; perftTest[20][5]=1001523;
perftTest[21][0]=15; perftTest[21][1]=66; perftTest[21][2]=1197; perftTest[21][3]=7059; perftTest[21][4]=133987; perftTest[21][5]=764643;
perftTest[22][0]=16; perftTest[22][1]=71; perftTest[22][2]=1287; perftTest[22][3]=7626; perftTest[22][4]=145232; perftTest[22][5]=846648;
perftTest[23][0]=5; perftTest[23][1]=130; perftTest[23][2]=782; perftTest[23][3]=22180; perftTest[23][4]=118882; perftTest[23][5]=3517770;
perftTest[24][0]=26; perftTest[24][1]=112; perftTest[24][2]=3189; perftTest[24][3]=17945; perftTest[24][4]=532933; perftTest[24][5]=2788982;
perftTest[25][0]=3; perftTest[25][1]=32; perftTest[25][2]=134; perftTest[25][3]=2073; perftTest[25][4]=10485; perftTest[25][5]=179869;
perftTest[26][0]=4; perftTest[26][1]=49; perftTest[26][2]=243; perftTest[26][3]=3991; perftTest[26][4]=20780; perftTest[26][5]=367724;
perftTest[27][0]=12; perftTest[27][1]=38; perftTest[27][2]=564; perftTest[27][3]=2219; perftTest[27][4]=37735; perftTest[27][5]=185867;
perftTest[28][0]=15; perftTest[28][1]=65; perftTest[28][2]=1018; perftTest[28][3]=4573; perftTest[28][4]=80619; perftTest[28][5]=413018;
perftTest[29][0]=26; perftTest[29][1]=568; perftTest[29][2]=13744; perftTest[29][3]=314346; perftTest[29][4]=7594526; perftTest[29][5]=179862938;
perftTest[30][0]=26; perftTest[30][1]=583; perftTest[30][2]=14252; perftTest[30][3]=334705; perftTest[30][4]=8198901; perftTest[30][5]=198328929;
perftTest[31][0]=25; perftTest[31][1]=560; perftTest[31][2]=13592; perftTest[31][3]=317324; perftTest[31][4]=7710115; perftTest[31][5]=185959088;
perftTest[32][0]=25; perftTest[32][1]=560; perftTest[32][2]=13607; perftTest[32][3]=320792; perftTest[32][4]=7848606; perftTest[32][5]=190755813;
perftTest[33][0]=25; perftTest[33][1]=567; perftTest[33][2]=14095; perftTest[33][3]=328965; perftTest[33][4]=8153719; perftTest[33][5]=195629489;
perftTest[34][0]=25; perftTest[34][1]=548; perftTest[34][2]=13502; perftTest[34][3]=312835; perftTest[34][4]=7736373; perftTest[34][5]=184411439;
perftTest[35][0]=25; perftTest[35][1]=547; perftTest[35][2]=13579; perftTest[35][3]=316214; perftTest[35][4]=7878456; perftTest[35][5]=189224276;
perftTest[36][0]=14; perftTest[36][1]=195; perftTest[36][2]=2760; perftTest[36][3]=38675; perftTest[36][4]=570726; perftTest[36][5]=8107539;
perftTest[37][0]=11; perftTest[37][1]=156; perftTest[37][2]=1636; perftTest[37][3]=20534; perftTest[37][4]=223507; perftTest[37][5]=2594412;
perftTest[38][0]=19; perftTest[38][1]=289; perftTest[38][2]=4442; perftTest[38][3]=73584; perftTest[38][4]=1198299; perftTest[38][5]=19870403;
perftTest[39][0]=3; perftTest[39][1]=51; perftTest[39][2]=345; perftTest[39][3]=5301; perftTest[39][4]=38348; perftTest[39][5]=588695;
perftTest[40][0]=17; perftTest[40][1]=54; perftTest[40][2]=835; perftTest[40][3]=5910; perftTest[40][4]=92250; perftTest[40][5]=688780;
perftTest[41][0]=15; perftTest[41][1]=193; perftTest[41][2]=2816; perftTest[41][3]=40039; perftTest[41][4]=582642; perftTest[41][5]=8503277;
perftTest[42][0]=16; perftTest[42][1]=180; perftTest[42][2]=2290; perftTest[42][3]=24640; perftTest[42][4]=288141; perftTest[42][5]=3147566;
perftTest[43][0]=4; perftTest[43][1]=68; perftTest[43][2]=1118; perftTest[43][3]=16199; perftTest[43][4]=281190; perftTest[43][5]=4405103;
perftTest[44][0]=17; perftTest[44][1]=54; perftTest[44][2]=835; perftTest[44][3]=5910; perftTest[44][4]=92250; perftTest[44][5]=688780;
perftTest[45][0]=3; perftTest[45][1]=51; perftTest[45][2]=345; perftTest[45][3]=5301; perftTest[45][4]=38348; perftTest[45][5]=588695;
perftTest[46][0]=17; perftTest[46][1]=278; perftTest[46][2]=4607; perftTest[46][3]=76778; perftTest[46][4]=1320507; perftTest[46][5]=22823890;
perftTest[47][0]=21; perftTest[47][1]=316; perftTest[47][2]=5744; perftTest[47][3]=93338; perftTest[47][4]=1713368; perftTest[47][5]=28861171;
perftTest[48][0]=21; perftTest[48][1]=144; perftTest[48][2]=3242; perftTest[48][3]=32955; perftTest[48][4]=787524; perftTest[48][5]=7881673;
perftTest[49][0]=7; perftTest[49][1]=143; perftTest[49][2]=1416; perftTest[49][3]=31787; perftTest[49][4]=310862; perftTest[49][5]=7382896;
perftTest[50][0]=6; perftTest[50][1]=106; perftTest[50][2]=1829; perftTest[50][3]=31151; perftTest[50][4]=530585; perftTest[50][5]=9250746;
perftTest[51][0]=17; perftTest[51][1]=309; perftTest[51][2]=5133; perftTest[51][3]=93603; perftTest[51][4]=1591064; perftTest[51][5]=29027891;
perftTest[52][0]=7; perftTest[52][1]=143; perftTest[52][2]=1416; perftTest[52][3]=31787; perftTest[52][4]=310862; perftTest[52][5]=7382896;
perftTest[53][0]=21; perftTest[53][1]=144; perftTest[53][2]=3242; perftTest[53][3]=32955; perftTest[53][4]=787524; perftTest[53][5]=7881673;
perftTest[54][0]=19; perftTest[54][1]=275; perftTest[54][2]=5300; perftTest[54][3]=104342; perftTest[54][4]=2161211; perftTest[54][5]=44956585;
perftTest[55][0]=36; perftTest[55][1]=1027; perftTest[55][2]=29215; perftTest[55][3]=771461; perftTest[55][4]=20506480; perftTest[55][5]=525169084;
perftTest[56][0]=19; perftTest[56][1]=275; perftTest[56][2]=5300; perftTest[56][3]=104342; perftTest[56][4]=2161211; perftTest[56][5]=44956585;
perftTest[57][0]=36; perftTest[57][1]=1027; perftTest[57][2]=29227; perftTest[57][3]=771368; perftTest[57][4]=20521342; perftTest[57][5]=524966748;
perftTest[58][0]=2; perftTest[58][1]=36; perftTest[58][2]=143; perftTest[58][3]=3637; perftTest[58][4]=14893; perftTest[58][5]=391507;
perftTest[59][0]=2; perftTest[59][1]=36; perftTest[59][2]=143; perftTest[59][3]=3637; perftTest[59][4]=14893; perftTest[59][5]=391507;
perftTest[60][0]=6; perftTest[60][1]=35; perftTest[60][2]=495; perftTest[60][3]=8349; perftTest[60][4]=166741; perftTest[60][5]=3370175;
perftTest[61][0]=22; perftTest[61][1]=43; perftTest[61][2]=1015; perftTest[61][3]=4167; perftTest[61][4]=105749; perftTest[61][5]=419369;
perftTest[62][0]=2; perftTest[62][1]=36; perftTest[62][2]=143; perftTest[62][3]=3637; perftTest[62][4]=14893; perftTest[62][5]=391507;
perftTest[63][0]=6; perftTest[63][1]=35; perftTest[63][2]=495; perftTest[63][3]=8349; perftTest[63][4]=166741; perftTest[63][5]=3370175;
perftTest[64][0]=3; perftTest[64][1]=7; perftTest[64][2]=43; perftTest[64][3]=199; perftTest[64][4]=1347; perftTest[64][5]=6249;
perftTest[65][0]=3; perftTest[65][1]=7; perftTest[65][2]=43; perftTest[65][3]=199; perftTest[65][4]=1347; perftTest[65][5]=6249;
perftTest[66][0]=1; perftTest[66][1]=3; perftTest[66][2]=12; perftTest[66][3]=80; perftTest[66][4]=342; perftTest[66][5]=2343;
perftTest[67][0]=1; perftTest[67][1]=3; perftTest[67][2]=12; perftTest[67][3]=80; perftTest[67][4]=342; perftTest[67][5]=2343;
perftTest[68][0]=7; perftTest[68][1]=35; perftTest[68][2]=210; perftTest[68][3]=1091; perftTest[68][4]=7028; perftTest[68][5]=34834;
perftTest[69][0]=1; perftTest[69][1]=3; perftTest[69][2]=12; perftTest[69][3]=80; perftTest[69][4]=342; perftTest[69][5]=2343;
perftTest[70][0]=1; perftTest[70][1]=3; perftTest[70][2]=12; perftTest[70][3]=80; perftTest[70][4]=342; perftTest[70][5]=2343;
perftTest[71][0]=3; perftTest[71][1]=7; perftTest[71][2]=43; perftTest[71][3]=199; perftTest[71][4]=1347; perftTest[71][5]=6249;
perftTest[72][0]=3; perftTest[72][1]=7; perftTest[72][2]=43; perftTest[72][3]=199; perftTest[72][4]=1347; perftTest[72][5]=6249;
perftTest[73][0]=5; perftTest[73][1]=35; perftTest[73][2]=182; perftTest[73][3]=1091; perftTest[73][4]=5408; perftTest[73][5]=34822;
perftTest[74][0]=2; perftTest[74][1]=8; perftTest[74][2]=44; perftTest[74][3]=282; perftTest[74][4]=1814; perftTest[74][5]=11848;
perftTest[75][0]=2; perftTest[75][1]=8; perftTest[75][2]=44; perftTest[75][3]=282; perftTest[75][4]=1814; perftTest[75][5]=11848;
perftTest[76][0]=3; perftTest[76][1]=9; perftTest[76][2]=57; perftTest[76][3]=360; perftTest[76][4]=1969; perftTest[76][5]=10724;
perftTest[77][0]=3; perftTest[77][1]=9; perftTest[77][2]=57; perftTest[77][3]=360; perftTest[77][4]=1969; perftTest[77][5]=10724;
perftTest[78][0]=5; perftTest[78][1]=25; perftTest[78][2]=180; perftTest[78][3]=1294; perftTest[78][4]=8296; perftTest[78][5]=53138;
perftTest[79][0]=8; perftTest[79][1]=61; perftTest[79][2]=483; perftTest[79][3]=3213; perftTest[79][4]=23599; perftTest[79][5]=157093;
perftTest[80][0]=8; perftTest[80][1]=61; perftTest[80][2]=411; perftTest[80][3]=3213; perftTest[80][4]=21637; perftTest[80][5]=158065;
perftTest[81][0]=4; perftTest[81][1]=15; perftTest[81][2]=90; perftTest[81][3]=534; perftTest[81][4]=3450; perftTest[81][5]=20960;
perftTest[82][0]=3; perftTest[82][1]=9; perftTest[82][2]=57; perftTest[82][3]=360; perftTest[82][4]=1969; perftTest[82][5]=10724;
perftTest[83][0]=3; perftTest[83][1]=9; perftTest[83][2]=57; perftTest[83][3]=360; perftTest[83][4]=1969; perftTest[83][5]=10724;
perftTest[84][0]=5; perftTest[84][1]=25; perftTest[84][2]=180; perftTest[84][3]=1294; perftTest[84][4]=8296; perftTest[84][5]=53138;
perftTest[85][0]=8; perftTest[85][1]=61; perftTest[85][2]=411; perftTest[85][3]=3213; perftTest[85][4]=21637; perftTest[85][5]=158065;
perftTest[86][0]=8; perftTest[86][1]=61; perftTest[86][2]=483; perftTest[86][3]=3213; perftTest[86][4]=23599; perftTest[86][5]=157093;
perftTest[87][0]=4; perftTest[87][1]=15; perftTest[87][2]=89; perftTest[87][3]=537; perftTest[87][4]=3309; perftTest[87][5]=21104;
perftTest[88][0]=4; perftTest[88][1]=19; perftTest[88][2]=117; perftTest[88][3]=720; perftTest[88][4]=4661; perftTest[88][5]=32191;
perftTest[89][0]=5; perftTest[89][1]=19; perftTest[89][2]=116; perftTest[89][3]=716; perftTest[89][4]=4786; perftTest[89][5]=30980;
perftTest[90][0]=5; perftTest[90][1]=22; perftTest[90][2]=139; perftTest[90][3]=877; perftTest[90][4]=6112; perftTest[90][5]=41874;
perftTest[91][0]=4; perftTest[91][1]=16; perftTest[91][2]=101; perftTest[91][3]=637; perftTest[91][4]=4354; perftTest[91][5]=29679;
perftTest[92][0]=5; perftTest[92][1]=22; perftTest[92][2]=139; perftTest[92][3]=877; perftTest[92][4]=6112; perftTest[92][5]=41874;
perftTest[93][0]=4; perftTest[93][1]=16; perftTest[93][2]=101; perftTest[93][3]=637; perftTest[93][4]=4354; perftTest[93][5]=29679;
perftTest[94][0]=3; perftTest[94][1]=15; perftTest[94][2]=84; perftTest[94][3]=573; perftTest[94][4]=3013; perftTest[94][5]=22886;
perftTest[95][0]=4; perftTest[95][1]=16; perftTest[95][2]=101; perftTest[95][3]=637; perftTest[95][4]=4271; perftTest[95][5]=28662;
perftTest[96][0]=5; perftTest[96][1]=19; perftTest[96][2]=117; perftTest[96][3]=720; perftTest[96][4]=5014; perftTest[96][5]=32167;
perftTest[97][0]=4; perftTest[97][1]=19; perftTest[97][2]=117; perftTest[97][3]=712; perftTest[97][4]=4658; perftTest[97][5]=30749;
perftTest[98][0]=5; perftTest[98][1]=22; perftTest[98][2]=139; perftTest[98][3]=877; perftTest[98][4]=6112; perftTest[98][5]=41874;
perftTest[99][0]=4; perftTest[99][1]=16; perftTest[99][2]=101; perftTest[99][3]=637; perftTest[99][4]=4354; perftTest[99][5]=29679;
perftTest[100][0]=5; perftTest[100][1]=22; perftTest[100][2]=139; perftTest[100][3]=877; perftTest[100][4]=6112; perftTest[100][5]=41874;
perftTest[101][0]=4; perftTest[101][1]=16; perftTest[101][2]=101; perftTest[101][3]=637; perftTest[101][4]=4354; perftTest[101][5]=29679;
perftTest[102][0]=5; perftTest[102][1]=15; perftTest[102][2]=102; perftTest[102][3]=569; perftTest[102][4]=4337; perftTest[102][5]=22579;
perftTest[103][0]=4; perftTest[103][1]=16; perftTest[103][2]=101; perftTest[103][3]=637; perftTest[103][4]=4271; perftTest[103][5]=28662;
perftTest[104][0]=5; perftTest[104][1]=22; perftTest[104][2]=139; perftTest[104][3]=877; perftTest[104][4]=6112; perftTest[104][5]=41874;
perftTest[105][0]=4; perftTest[105][1]=16; perftTest[105][2]=101; perftTest[105][3]=637; perftTest[105][4]=4354; perftTest[105][5]=29679;
perftTest[106][0]=5; perftTest[106][1]=22; perftTest[106][2]=139; perftTest[106][3]=877; perftTest[106][4]=6112; perftTest[106][5]=41874;
perftTest[107][0]=4; perftTest[107][1]=16; perftTest[107][2]=101; perftTest[107][3]=637; perftTest[107][4]=4354; perftTest[107][5]=29679;
perftTest[108][0]=5; perftTest[108][1]=25; perftTest[108][2]=161; perftTest[108][3]=1035; perftTest[108][4]=7574; perftTest[108][5]=55338;
perftTest[109][0]=5; perftTest[109][1]=25; perftTest[109][2]=161; perftTest[109][3]=1035; perftTest[109][4]=7574; perftTest[109][5]=55338;
perftTest[110][0]=7; perftTest[110][1]=49; perftTest[110][2]=378; perftTest[110][3]=2902; perftTest[110][4]=24122; perftTest[110][5]=199002;
perftTest[111][0]=5; perftTest[111][1]=22; perftTest[111][2]=139; perftTest[111][3]=877; perftTest[111][4]=6112; perftTest[111][5]=41874;
perftTest[112][0]=4; perftTest[112][1]=16; perftTest[112][2]=101; perftTest[112][3]=637; perftTest[112][4]=4354; perftTest[112][5]=29679;
perftTest[113][0]=5; perftTest[113][1]=22; perftTest[113][2]=139; perftTest[113][3]=877; perftTest[113][4]=6112; perftTest[113][5]=41874;
perftTest[114][0]=4; perftTest[114][1]=16; perftTest[114][2]=101; perftTest[114][3]=637; perftTest[114][4]=4354; perftTest[114][5]=29679;
perftTest[115][0]=5; perftTest[115][1]=25; perftTest[115][2]=161; perftTest[115][3]=1035; perftTest[115][4]=7574; perftTest[115][5]=55338;
perftTest[116][0]=5; perftTest[116][1]=25; perftTest[116][2]=161; perftTest[116][3]=1035; perftTest[116][4]=7574; perftTest[116][5]=55338;
perftTest[117][0]=7; perftTest[117][1]=49; perftTest[117][2]=378; perftTest[117][3]=2902; perftTest[117][4]=24122; perftTest[117][5]=199002;
perftTest[118][0]=11; perftTest[118][1]=97; perftTest[118][2]=887; perftTest[118][3]=8048; perftTest[118][4]=90606; perftTest[118][5]=1030499;
perftTest[119][0]=24; perftTest[119][1]=421; perftTest[119][2]=7421; perftTest[119][3]=124608; perftTest[119][4]=2193768; perftTest[119][5]=37665329;
perftTest[120][0]=18; perftTest[120][1]=270; perftTest[120][2]=4699; perftTest[120][3]=79355; perftTest[120][4]=1533145; perftTest[120][5]=28859283;
perftTest[121][0]=24; perftTest[121][1]=496; perftTest[121][2]=9483; perftTest[121][3]=182838; perftTest[121][4]=3605103; perftTest[121][5]=71179139;
perftTest[122][0]=11; perftTest[122][1]=97; perftTest[122][2]=887; perftTest[122][3]=8048; perftTest[122][4]=90606; perftTest[122][5]=1030499;
perftTest[123][0]=24; perftTest[123][1]=421; perftTest[123][2]=7421; perftTest[123][3]=124608; perftTest[123][4]=2193768; perftTest[123][5]=37665329;
perftTest[124][0]=18; perftTest[124][1]=270; perftTest[124][2]=4699; perftTest[124][3]=79355; perftTest[124][4]=1533145; perftTest[124][5]=28859283;
perftTest[125][0]=24; perftTest[125][1]=496; perftTest[125][2]=9483; perftTest[125][3]=182838; perftTest[125][4]=3605103; perftTest[125][5]=71179139;

}
