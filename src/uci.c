#include <stdio.h>
#include <string.h>

#include "plateau.h"
#include "init.h"
#include "epd.h"
#include "valeur.h"
#include "recherche.h"
#include "evaluation.h"
#include "boolean.h"
#include "hash.h"
#include "listecoup.h"
#include "generecoup.h"
#include "test.h"
#include "timer.h"
#include "util.h"
#include "uci.h"

#include "bitboard.h"
#include "couleur.h"

#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>		// Pour la gestion des threads.
	typedef HANDLE HandleThread;
	DWORD threadID;				
	HANDLE hThread;			// Handle du thread
#else					// Sous LINUX	
	#include <pthread.h>
	typedef  pthread_t HandleThread;
	pthread_t hThread;		// Handle du thread
	int	  codeCreation;		// code retour de la creation du thread.
#endif

	/* Structure contenant les parametes passes au thread en charge de lire l'entree standard */
typedef struct {
	Plateau *p;
	GestionTemps *gt;
} ParamThread;

#define LOG_STDIN 0

char VERSION[]="0.6.17";

		/* Valeurs booleannes utilisees dans lireStdin */
Boolean isUCI = FAUX;	// Si VRAI le mode UCI est actif donc je n'envoie pas le prompt.
Boolean isFritz = VRAI;	// Si je recois Ucinewgame ce n'est pas Fritz
Boolean isNewGame = VRAI;
Boolean testFen = FAUX;
FILE	*entreeStandard;	// Flux pour stocker les commandes reçues sur stdin.
MonTimer tempsCommandesUCI;

Boolean rechercheEnCours = FAUX;	// Vrai si le programme est cherche le meilleur


void envoiIdProg() {
	/* retour chariot sur le premier printf necessaire pour un bon fonctionnement sous Fritz */
  printf("\nid name Heracles %s\n",VERSION);
  printf("id author Franck Tempet\n");
  fflush(stdout);
}
 
/*---------------------------------------------------------------------------*/

void demarreRecherche( Plateau *p , GestionTemps *gt );

/*---------------------------------------------------------------------------*/
void envoiOptions() {
 printf("option name Ponder type check default true\n");
 printf("option name Hash type spin default 32 min 1 max 256\n");
// printf("option name NalimovCache type spin min 1 max 16 default 2\n");
// printf("option name NalimovPath type string default <empty>\n");
// printf("option name MultiPV type spin min 1 max 1 default 1\n");
// printf("option name OwnBook type check default true\n");
// printf("option name Clear Hash type button\n");
 //printf("option name Position Learning type check default false\n");
// printf("option name BookFile type string default dbbook.bin\n");
// printf("option name Book Size type combo default Big var Small var Medium var Big\n");
// printf("option name Selectivity type spin min 0 max 15 default 5\n");
// printf("option name Clear Book Learning type button\n");

 printf("uciok\n");
 fflush(stdout);
}

/*---------------------------------------------------------------------------*/

void initPosition( Plateau *p ,char *pos ) {
	Epd e,eTest;
	char fen[]="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	//init(p);
	//initValeur();
	initEpd( &e , pos );
	plateau_setPosition( p , e );

   fflush(stdout);
}

/*---------------------------------------------------------------------------*/

void lireCoup( Plateau *p, char *pos , int nbMot ,Boolean newGame ) {
  short isMove = 0;
  char *po=pos;
  char *pp;
  AnnuleCoup undo;
  Coup co;
  char cos[20];
  char fenInit[]="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  char fenPos[200];
  int i;

  fenPos[0] = '\0';
  	
  /* Passe le texte position */
  pp = po;
  po = po + strlen(po) + 1;
  nbMot--; 

  while ( nbMot ) {
    if  ( newGame ) {
		if ( !strcmp(po,"startpos") || !strcmp(po,"STARTPOS") ) { 
			initPosition( p , fenInit );
		}

		if ( !strcmp(po,"fen") || !strcmp(po,"FEN") ) {
		  for( i=0 ; i<4 ; i++ ) {
			pp = po;
			po = po + strlen(po) + 1;

			strcat( fenPos , po );
			if (i!=5) strcat( fenPos , " " );
			nbMot--;
		  }		
		  //printf("FEN %s#\n", fenPos );

		  initPosition( p , fenPos );	
		  //printPlateau(p);		  
		}
	}
    if ( !strcmp(po,"moves") ) isMove =1;

    if ( newGame & isMove & strcmp(po,"moves") ) {
	   // BUG AVEC ARENA 1.99 parfois la commande position startpos moves contient le coup '...'
	   // Je test donc si la chaine et bien de la forme lettre chiffre ( controle les 2 premiers caracteres )
		if ( strlen(po) >= 4 ) {
			if ( (po[0] >= 'a') && (po[0] <= 'h') && (po[1] >= '1') && (po[1] <= '8') ) {
					co = chaineToCoup(p,po);
					plateau_joueCoup( p , co , &undo);
		   }
		}
    }

     pp = po;
     po = po + strlen(po) + 1;
     nbMot--; 


  }

  if ( isMove && !newGame ) {
	   // BUG AVEC ARENA 1.99 parfois la commande position startpos moves contient le coup '...'
	   // Je test donc si la chaine et bien de la forme lettre chiffre lettre chiffre
		if ( strlen(po) >= 4 ) { 
			if ( (po[0] >= 'a') && (po[0] <= 'h') && (po[1] >= '1') && (po[1] <= '8') ) {
				co = chaineToCoup(p,pp);
		 	    plateau_joueCoup( p , co , &undo);
			}
	   }
  }
}
/*---------------------------------------------------------------------------*/
void lireGo(  Plateau *p , char *sGo , int nbMot , GestionTemps *gt  ) {
  char 	     *po=sGo;
  AnnuleCoup undo;
  Coup bestCoup;
  Coup bestCoupMinMax; /* Pour utiliser le minmax donc provisoire */
  int score;
  char	coup[10],fen[100];	/* Pour utiliser le minmax donc provisoire */


  if ( nbMot <= 1 ) return;

  timer_initialise( gt );

  	/* Passe au mot suivant */
  po = po + strlen(po) + 1;
  nbMot--; 

  while ( nbMot ) { 
	  
     if ( !strcmp( "ponder" ,  po )  )  timer_setPonder( gt , VRAI );
     if ( !strcmp( "infinite" ,  po ))  timer_setInfinite( gt , VRAI );

     if ( !strcmp( "wtime" ,  po ) ) {
       po = po + strlen(po) + 1;
       nbMot--;
       timer_setTempsBlanc( gt , atoi(po) );
	   goto suivant;
     }

     if ( !strcmp( "btime" ,  po ) ) {
       po = po + strlen(po) + 1;
       nbMot--;
       timer_setTempsNoir( gt , atoi(po) );
	   goto suivant;
     }

     if ( !strcmp( "winc" ,  po ) ) {
       po = po + strlen(po) + 1;
       nbMot--;
       timer_setIncBlanc( gt , atoi(po) );
	   goto suivant;
     }
     
     if ( !strcmp( "binc" ,  po ) ) {
       po = po + strlen(po) + 1;
       nbMot--;
       timer_setIncNoir( gt , atoi(po) );
	   goto suivant;
     }
     
     if ( !strcmp( "movestogo" ,  po ) ) {
       po = po + strlen(po) + 1;
       nbMot--;
       timer_setNbCoupTemps( gt , atoi(po) );
	   goto suivant;
     }
     
     if ( !strcmp( "depth" ,  po ) ) {
       po = po + strlen(po) + 1;
       nbMot--;
       timer_setProfondeur( gt , atoi(po) );
	   goto suivant;
     }
     
     if ( !strcmp( "nodes" ,  po ) ) {
       po = po + strlen(po) + 1;
       nbMot--;
       timer_setNoeud( gt , atoi(po) );
	   goto suivant;
     }
     
     if ( !strcmp( "mate" ,  po ) ) {
       po = po + strlen(po) + 1;
       nbMot--;
       timer_setMat( gt , atoi(po) );
	   goto suivant;
     }
     
     if ( !strcmp( "movetime" ,  po ) ) {
       po = po + strlen(po) + 1;
       nbMot--;
       timer_setTempsParCoup( gt , atoi(po) );
     }
     
suivant:
     po = po + strlen(po) + 1;
     nbMot--;
  }

  timer_initGestionTemps( gt , plateau_getCoteCourant( p ) );


}

/*---------------------------------------------------------------------------*/

void lireSetOption( char *sOption , int nbMot ) {
	char 	     *po=sOption;
	char		 name[50];
	char		 value[500];
	
		/* Passe setoption */
	po = po + strlen(po) + 1;

		/* Passe name */
	po = po + strlen(po) + 1;

		/* Copie le nom de l'option */
	strcpy( name , po );

			/* Passe le nom de l'option */
	po = po + strlen(po) + 1;

		/* Passe value */
	po = po + strlen(po) + 1;

	/* Copie la valeur de l'option */
	strcpy( value , po );

	strupper( name , strlen(name) );

    if ( !strcmp( "HASH" ,  name ) ) {
		hash_free();
		hash_creer( atoi( value ) );
	}
	
}

/*---------------------------------------------------------------------------*/

void commandeHelp() {
	printf("\n");
	printf("setboard startpos.........Set board initial position\n");
	printf("setboard <FEN>............Set board with FEN string\n");
	printf("perft <N>.................Compute perft depth N\n");
	printf("divideperft <N>...........Compute divide depth N\n");
	printf("hashperft <N>.............Compute perft depth N with hash table\n");
	printf("eval......................Evaluation of the position without quiescence search\n");
	printf("qs........................Evaluation of the position with quiescence search\n");
	printf("quit......................Quit heracles\n");
	//printf("qs....................................Quiescence search\n");
	printf("\n");
}
/*---------------------------------------------------------------------------*/

void commandePerft( long (*f)(Plateau *, int) ,Plateau *p ,
					int nbMot , char *infoGui ) {
	   char *cmd;
	   long nbNoeuds =0;
	   MonTimer tps;
	   int prof;

	   if ( nbMot != 2 ) {
		   printf("Usage : [perft|hashperft|divideperft] <depth>\n",nbMot);
	   } else {
	   	cmd = infoGui + strlen(infoGui)+1;
	   	prof = atoi( cmd );

		timer_reset( tps );
		timer_start( tps );
		nbNoeuds = f( p , prof );
		timer_stop( tps );

		printf("\nNodes : %ld   time : %8.3f s\n\n", nbNoeuds , ((float)timer_delta( tps )/1000) );

	   }
}

/*---------------------------------------------------------------------------*/

void commandeSetBoard( Plateau *p , char *infoGui ) {
   char infoGui2[500];
   char *po=infoGui;
   int nbMot,i;

   infoGui2[0]='\0';

   hash_effacer();
   hashRepet_effacer();

        /* Passe le texte setBoard */
  po = po + strlen(po) + 1;
  
  if ( !strcmp( "startpos" ,  po ) ) {
	strcat( infoGui2 , "position startpos");
    nbMot = espaceToFinChaine( infoGui2 );
  }
  else {
	  strcat( infoGui2 , "position fen " );
	  strcat( infoGui2 , po );
	  strcat( infoGui2 , " " );
	  for( i=0 ; i<6 ; i++ ) {
		 po = po + strlen(po) + 1;
		 strcat( infoGui2 , po );
		 if (i!=5) strcat( infoGui2 , " " );
	  }
			
	  nbMot = espaceToFinChaine( infoGui2 );

  }

  lireCoup( p , infoGui2 , nbMot , VRAI ); 
}
/*---------------------------------------------------------------------------*/

void demarreRecherche( Plateau *p , GestionTemps *gt ) {
   Coup bestCoup;
   int	score;
   AnnuleCoup undo;


   /*
   int i;
   char coup[20];

   for( i=0 ; i<plateau_getNbCoup(p) ; i++ ) {
      printCoup( plateau_getCoup( p , i ) , coup );
	  printf("%s\n",coup);
   }
  */



   bestCoup = iterateDeepening( p , gt , &score ); 

   #ifdef STATRECHERCHE
	    afficheStats();
   #endif

			/* Ne joue pas le coup sur le plateau si en mode Ponder ou pas de coup possible */
   if ( (bestCoup != PAS_DE_COUP) && (!timer_getPonder(gt)) ) { 
     plateau_joueCoup( p, bestCoup , &undo);
     // TODO FAUT IL EFFACER LA HASHTABLE APRES UNE RECHERCHE ?
     // mon premier test me dit que non mais c'est a creuser.
     // Je pense que plus on avance dans la partie plus c'est penalisant.
     hash_effacer();
  }
}

/*---------------------------------------------------------------------------*/

#if defined(_WIN32) || defined(_WIN64)

  DWORD WINAPI demarreThreadRecherche( LPVOID lpParam ) {
    demarreRecherche( ((ParamThread*)lpParam )->p , ((ParamThread*)lpParam )->gt );
    rechercheEnCours = FAUX;
  }

#else

  static void *demarreThreadRecherche( void * lpParam ) {
     demarreRecherche( ((ParamThread*)lpParam )->p , ((ParamThread*)lpParam )->gt );
     rechercheEnCours = FAUX;
  }

#endif

/*---------------------------------------------------------------------------*/

void creerUnThread( void *fonction , void *parametres ) {

   #if defined(_WIN32) || defined(_WIN64)
	hThread = CreateThread(NULL, 0, fonction , parametres , 0, &threadID);
   #else
	codeCreation = pthread_create ( &hThread , NULL,  fonction , parametres );
   #endif

}	 

/*---------------------------------------------------------------------------*/
void attendreFinDuThread( HandleThread h ) {

   #if defined(_WIN32) || defined(_WIN64)
 	if ( rechercheEnCours ) WaitForSingleObject(h, INFINITE  );
   #else 
	if ( rechercheEnCours ) pthread_join ( h , NULL);
   #endif
}

/*---------------------------------------------------------------------------*/
void lireStdin( Plateau *p , GestionTemps *gt ) {
  char infoGui[5000];
  char *cmd,*ptr;
  char fileFen[50];
  char lCoup[600];
  int nbMot,prof;
  ListeCoup lc;
  ParamThread paramThread;
   
 
  paramThread.p	= p;
  paramThread.gt= gt;
  
  fgets( infoGui , 5000 , stdin );
  
  ptr = strchr( infoGui , '\n' );
  if (ptr != NULL)   *ptr = '\0';

  

#if LOG_STDIN
   timer_stop( tempsCommandesUCI );
   // Temps reel en mode ponder , ajoute une marge d'une seconde si je ne suis pas en mode ponder
   if  ( timer_getPonder( gt) )
	    fprintf( entreeStandard , "sleep %ld\n" ,timer_delta( tempsCommandesUCI ) );
   else
	    fprintf( entreeStandard , "sleep %ld\n" ,timer_delta( tempsCommandesUCI )+1000 );	 

   timer_reset( tempsCommandesUCI );
   timer_start( tempsCommandesUCI );
   fprintf( entreeStandard , infoGui);
   fprintf( entreeStandard , "\n" );
   fflush( entreeStandard );
#endif

   //printf("Stdin : %s\n" , infoGui ) ; fflush(stdout);
   nbMot = espaceToFinChaine( infoGui );

   if ( !strncmp( "go" , infoGui , 2 ) )  {
	   lireGo( p , infoGui , nbMot , gt ); 
           attendreFinDuThread( hThread );
 	   rechercheEnCours = VRAI;  // Important de fixer le flag avant de lancer le thread.
	   creerUnThread( demarreThreadRecherche , &paramThread ); 
#if 0
	   #if defined(_WIN32) || defined(_WIN64)
  			hThread = CreateThread(NULL, 0, demarreThreadRecherche , &paramThread, 0, &threadID);
	   #endif
#endif
   }

   if ( !strcmp( "ponderhit" , infoGui ) ) {
	  timer_reset( timer_getTimer(gt) ); 
	  timer_start( timer_getTimer(gt) );
	  timer_setPonder( gt , FAUX );
   }

   /* TODO : supprimer eval apres les tests et l'include evaluation.h */
   if ( !strcmp( "eval" , infoGui ) ) { 
	   printf("Score %d\n",evaluation(p,plateau_getCoteCourant(p),MOINS_INFINI,PLUS_INFINI)); }

   if ( !strcmp( "qs" , infoGui ) ) { 
      timer_setTempsMoyenTheorique( gt , TEMPS_INFINI );  
      printf("Score (quiescence search) %d\n",  rechercheCalme(p,-32000,32000,plateau_getCoteCourant(p),gt,0)); 
   }

   if ( !strcmp( "uci" , infoGui ) ) { envoiIdProg(); envoiOptions(); isUCI=VRAI; }

   if ( !strcmp( "isready" , infoGui ) ) { printf("readyok\n"); fflush(stdout); }

   if ( !strcmp( "ucinewgame" , infoGui ) )  { 
       attendreFinDuThread( hThread );
	   isNewGame=VRAI; 
	   hash_effacer();
	   hashRepet_effacer();
	   //debugRepet_clear( );
	   isFritz = VRAI;				// OBLIGER CAR AVEC POLYGLOT AVEC l'option BOOK activé je peux recevoir 2 commandes position qui ne se suivent pas en terme de coup car le N premiers coups proviennent du livre puis le P suivant non et ensuite le q suivant à nouveau du livre. 
	   if ( infoRecherche.genEpd ) fclose( infoRecherche.fileEpd );
	   infoRecherche.genEpd = FAUX; // TODO A MODIF PAR FAUX
	   //infoRecherche.fileEpd = fopen( "genepd.txt" , "a" ); // TODO A SUPPRIMER
   }

   if ( !strcmp( "position" , infoGui ) )  {
        timer_reset( timer_getTimer(gt) ); 
		timer_start( timer_getTimer(gt) );

        attendreFinDuThread( hThread );

			/* Si je recois cette commande en mode ponder c'est  que la prediction
			   d"heracles est fausse. il faut donc annuler le dernier coup joue */
        if  ( timer_getPonder( gt) )  plateau_annuleDernierCoup(p);

		/* Fritz n'envoie pas d'UcinewGame .... */
        if ( isFritz ) {
		   isNewGame=VRAI; 
		   hash_effacer();
		   hashRepet_effacer();
		   //debugRepet_clear( );
		}

		lireCoup( p , infoGui , nbMot , isNewGame ); 
		isNewGame=FAUX;
   }

   if ( !strncmp( "stop" , infoGui , 4 ) ) {
	   timer_setStopRecherche( gt , VRAI );
			/*  Attendre l'arret de la recherche avant de lire l'entree standard */
           attendreFinDuThread( hThread );
   } 


   if ( !strncmp( "quit" , infoGui , 4 ) )  {
	   #if LOG_STDIN
			fclose( entreeStandard );
	   #endif

	   if ( rechercheEnCours ) {
   	      timer_setStopRecherche( gt , VRAI );
              attendreFinDuThread( hThread );
	   }
	   exit(0);
   }


   if ( !strncmp( "setoption" , infoGui , 9 ) )  {
	   lireSetOption( infoGui , nbMot );
   }

   if ( !strncmp( "generecoup" , infoGui , 10 ) ) { 
	   listecoup_videListeCoup( &lc );
	   
	   genereCoup( &lc , p , BLANC );
	   printListeCoup( &lc , lCoup );
	   printf("Coups BLANC : %s\n", lCoup );
	   
	   listecoup_videListeCoup( &lc );
	   
   	   genereCoup( &lc , p , NOIR );
	   printListeCoup( &lc , lCoup );
	   printf("Coups NOIR : %s\n", lCoup );

   }

   if ( !strncmp( "generecapture" , infoGui , 10 ) ) { 
	   listecoup_videListeCoup( &lc );
	   
	   genereCoupCapture( &lc , p , BLANC );
	   printListeCoup( &lc , lCoup );
	   printf("Coups BLANC : %s\n", lCoup );
	   
	   listecoup_videListeCoup( &lc );
	   
	   genereCoupCapture( &lc , p , NOIR );
	   printListeCoup( &lc , lCoup );
	   printf("Coups NOIR : %s\n", lCoup );
   }

   if ( !strncmp( "perft" , infoGui , 5 ) ) 
		commandePerft( perft , p , nbMot , infoGui );


   if ( !strncmp( "divideperft" , infoGui , 11 ) )
	   		commandePerft( perftDivise , p , nbMot , infoGui );

   if ( !strncmp( "hashperft" , infoGui , 9 ) ) 
	   		commandePerft( perftHash , p , nbMot , infoGui );

   if ( !strncmp( "setboard" , infoGui , 8 ) ) {
	   isNewGame=VRAI; 
	   hash_effacer();
	   hashRepet_effacer();
	   debugRepet_clear( );
	   commandeSetBoard( p , infoGui  );
   }
 

   if ( !strncmp( "genereepd" , infoGui , 9 ) ) { 
	   if ( nbMot != 2 ) {
		   printf("Usage : genereepd <nom du fichier>\n",nbMot);
		   fflush( stdout );
	   } else {
	     cmd = infoGui + strlen(infoGui)+1;
	     infoRecherche.genEpd = VRAI;
	     infoRecherche.fileEpd = fopen( cmd , "a" );
	   }
   }

   if ( !strncmp( "help" , infoGui , 4 ) ) commandeHelp(); 

   if ( !strncmp( "boardkey" , infoGui , 4 ) ) { 
		 printf("\nBoard key : ");
		 printBitBoardHexa( plateau_getCle( p ) );
		 printf("\n");
	 }

   if ( !strncmp( "testgencoup" , infoGui , 11 ) ) testGenerateurCoup( p ); 

   if ( !strncmp( "loadfen" , infoGui , 7 ) ) {
	   strcpy( fileFen , infoGui + strlen(infoGui)+1 );
	   testFen = VRAI;
   }

   if ( !strncmp( "unloadfen" , infoGui , 7 ) ) testFen = FAUX;

   if ( !strncmp( "testalphabeta" , infoGui , 13 ) ) {
	   lireGo( p , infoGui , nbMot , gt  );
	   lanceTest( p, gt , fileFen , testFen , testAlphaBeta );
   }

   if ( !strncmp( "comptenoeud" , infoGui , 13 ) ) {
	   lireGo( p , infoGui , nbMot , gt  );
	   lanceTest( p, gt , fileFen , testFen , compteNbNoeud );
   }

   
   fflush(stdout);

}
/*---------------------------------------------------------------------------*/
void wait( Plateau *p , GestionTemps *gt ) {

 
  
 /* TODO : ME DEPLACER DANS InitPosition apres fin test */
 //init(p);
 //initValeur();

#if LOG_STDIN
 entreeStandard = fopen( "stdin.txt" , "a" );
 timer_reset( tempsCommandesUCI );
 timer_start( tempsCommandesUCI );
#endif


 fprintf(stdout,"Heracles %s by Franck Tempet\n",VERSION);
 fprintf(stdout,"Tape help for commands\n\n");

 fflush( stdout );
 
  #ifdef STATRECHERCHE
	    initStats();
  #endif

 timer_initialise( gt );

 while (1) {
   if ( !isUCI ) { fprintf(stdout,"Heracles : "); fflush( stdout ); }

   lireStdin( p , gt );
 }

}


