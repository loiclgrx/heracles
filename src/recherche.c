#include <setjmp.h>
#include "evaluation.h"
#include "listecoup.h"
#include "trace.h"
#include "generecoup.h"
#include "init.h"
#include "type.h"
#include "hash.h"
#include "coup.h"
#include "recherche.h"
#include "multipv.h"
#include "valeur.h"
#include "timer.h"
#include "historytable.h"
#include "killer.h"


#define DEBUG_ALPHABETA 0
#define USE_HASHTABLE   1
#define USE_TEMPS	1
#define USE_NULL_MOVE  1


	// Contient le nombre de fois que le meilleur coup a ete trouve à la position n   
	// indice 0 meilleur prsent dans la table de hachage
	// indice 1 meilleur coup trouve en premier ....
 	long statCut[MAX_COUPS];	
	// Nombre de fois que l'on appel le generateur de coup 
	long nbGenCoup = 0;
	// l'indice du tableau correspondant au delta entre 2 iterations
	// ex: 1 pour 1  20 pour 20 pts  100 pour les deltas > 100 point
	// Ce tableau permet de connaitre la repartition des deltas
	long  deltaScore[101];
	int   scoreProfPrec;	// permet de scoker le score de l'iteration precedente



InfoRecherche infoRecherche;	/*  indique si l'on trace dans un fichier le fen + coup */

InfoRechercheRacine infoRechercheRacine;	 

Pv				*pvCourant;			/* Pointe sur la variante courante */

HistoryTable 	historyTable;
Killer			killer;

jmp_buf 		env;				/* utilise pour arreter la recherhe lorsque le temps est ecoule */

char 	lCoup[2000]; 			/* TODO : a supprimer utilise pour le DEBUG */
char 	coup[10];				/* TODO : a supprimer utilise pour le DEBUG */
char 	fen[100];				/* TODO : a SUPPRIMER car utilise pour le debug */
int 	afficheTrace = 0;		/* TODO : a supprimer utilise pour le DEBUG */

ListeCoup pvCourante;	/* TODO : pour le debogage : contient la variation courante de la recherche */
char 	pvDebug[150];	/* TODO : pour le debogage : contient la variation a tracer */
char	fenDebug[150];	/* TODO POUR DEBUG : contient la position a tracer si stocke dans HashTable */

Boolean		debugGlobal=FAUX;

int testIsVarianteDebug() {
	int len1,len2,len;

	/* compare la variante courante avec la variante a deboger */
	printListeCoup( &pvCourante , lCoup );
	len1 = strlen( lCoup );
	len2 = strlen( pvDebug );

	if (len1 > len2 ) len = len2; else len =len1;
	
	return strncmp( lCoup , pvDebug , len );
}
/*--------------------------------------------------------------------------------------*/

void afficheStats() {
	int i,j,total=0,total2=0,total3=0;

  printf("Meilleur coup en position N ( N = 0 => meilleur coup dans la table de hashage )\n");

	for( i=0 ; i<50 ; i++ ) printf("%d\t: %ld\n", i , statCut[ i ] );

	printf("\nNombre de noeuds examines\t     : %ld\n", infoRechercheRacine.nbNoeudsTotal );
	printf("Nombre d'appel au generateur de coup : %ld\n", nbGenCoup );

	for( i=0 ; i<101 ; i++ ) total2 += deltaScore[i]; 

	for( i=0 ; i<10 ; i++ ) {
		total = 0;
	  for( j=0 ; j<10 ; j++ ) total += deltaScore[ i*10+j ];
		total3 += total;
		printf("Delta score %d : %d ( %f\% ) \n", i , total ,(float)((float)total3/(float)total2) );
	}

	printf("Delta score 10 : %ld\n", deltaScore[100] );

}

void initStats() {
	int i;

  for( i = 0 ; i<MAX_COUPS ; i++ ) statCut[i] = 0;
  for( i = 0 ; i< 101 ; i++ ) deltaScore[i] = 0;

}

/*--------------------------------------------------------------------------------------*/
void traceHashPut( Plateau *p , Coup bestCoup , short profondeur , int scorePos , char *message) {
	char fenCourant[100];
	int len;

	len = strlen( fenDebug );

	plateau_getFen( p , fenCourant );

	if (strncmp( fenDebug , fenCourant , len )) return; 

	printf("################## PUT ########################\n");
	printf("Hash Table Put : %s\n",message);
	printListeCoup( &pvCourante , lCoup );
	printf("PV : %s\n", lCoup );
	printf("Profondeur : %d\n", profondeur );
	printf("Score : %d\n", scorePos );
	printCoup( bestCoup , coup );
	printf("Coup %s\n", coup);
	printf("################## END PUT ####################\n");
	fflush(stdout);
}
/*--------------------------------------------------------------------------------------*/
traceHashGet( PositionHash *ph , Plateau *p, char *message) {

	if ( !afficheTrace ) return;
	if ( testIsVarianteDebug() ) return;

	printf("################## GET ########################\n");
	printf("Hash Table Get : %s\n",message);

	switch ( positionHash_getFlag( ph ) ){
		case HASH_EXACT : printf("HASH EXACT\n");
				  break;
		case HASH_BETA : printf("HASH BETA\n");
				  break;
		case HASH_FEUILLE : printf("HASH FEUILLE\n");
				  break;
	}

	printListeCoup( &pvCourante , lCoup );
	printf("PV : %s\n", lCoup );
	printf("Hash score : %d\n", positionHash_getScore( ph) );
	printf("Hash profondeur : %d\n", positionHash_getProfondeur( ph) );
	printCoup( positionHash_getCoup( ph ) , coup );
	printf("Hash coup : %s\n", coup );
	printf("################## END GET ####################\n");
	fflush(stdout);
		
}
/*--------------------------------------------------------------------------------------*/

traceVariante( Plateau *p,int alpha,int beta,ListeCoup *meilleurVariante, int val) {
	int i;

	if ( !afficheTrace ) return;

	if ( testIsVarianteDebug() ) return;

	printf("PV courante : %s\n", lCoup );
	printf("PV debug : %s\n", pvDebug );

	printCoup(  listecoup_getCoup( &pvCourante , listecoup_getNbCoup( &pvCourante )-1) , coup );
	printf("Coup %s\n", coup ); 
	plateau_getFen( p , fen );
	printf("Fen %s\n", fen ); 
	printListeCoup( meilleurVariante , lCoup );
	printf("PV %s\n", lCoup ); 
	printf("alphaBeta [ %d , %d ]\n", alpha , beta ); 
	printf("Val %d\n", val ); 
	printf("-------------------------------------------------------------------------\n");
	fflush(stdout);

}
/*--------------------------------------------------------------------------------------*/
tracePosition( Plateau *p , Coup c , ListeCoup *mv , int val , int alpha , int beta ) {
  char fenCourant[100];
  int len;

  len = strlen( fenDebug );

  plateau_getFen( p , fenCourant );

  if (strncmp( fenDebug , fenCourant , len )) return; 

  printf("------------------------- TRACE POSITION --------------------------------\n");
  printCoup(  c , coup );
  printf("Coup           : %s\n", coup ); 
  printf("Fen            : %s\n", fenCourant ); 
  printf("[alpha,beta]   : [ %d , %d ]\n", alpha , beta ); 
  printListeCoup( mv , lCoup );
  printf("Variante       : %s %s\n", coup , lCoup ); 
  printf("Score variante : %d\n", val ); 
  printf("-------------------------------------------------------------------------\n");
  fflush(stdout);

}


/*--------------------------------------------------------------------------------------*/

int rechercheCalme( Plateau *p ,int alpha , int beta , Couleur c , GestionTemps *gt , int profondeur) {
  int i,val,bestScore=MOINS_INFINI;
  AnnuleCoup undo;
  ListeCoup lc;
  BitBoard pieceAttaqueRoi;
  
  infoRechercheRacine.nbNoeudsTotal++; 

  if ( infoRechercheRacine.nbNoeudsTotal & 1 ) {
  	if ( timer_arreteRecherche( p , gt ) ) longjmp(env,1); 
  }

  pieceAttaqueRoi =plateau_pieceAttaqueCase( p, plateau_getEmplacementRoi( p , c ) ,
		       		      					    couleur_oppose( c ) );

  /* Si position en Echec genere tous les coups possibles */
  /* TODO : Il faut egalement gerer l'echec et PAT */

  if ( pieceAttaqueRoi ) { 
	    listecoup_videListeCoup( &lc );
		genereCoupEchappeEchec( &lc , p , c , pieceAttaqueRoi );
				/* Echec et MAT ? */
		if ( listecoup_getNbCoup( &lc ) == 0 )  return -10000-profondeur;
		goto debut_recherche;
  }
		  
		/* Pas d'echecs */
		
   val = evaluation( p  , c ,alpha , beta );  
   
   if (val > bestScore) {
	   bestScore = val;
	   if (val > alpha) {
		  alpha = val;
		  if (val >= beta) return val;
	  }
  }
		  
   /*if ( val >= beta)
		return val;
   
	if (val > alpha)
		alpha = val;*/

 //  if ( profondeur < -30 ) return val;

     	/* Encore assez de temps ? */


   listecoup_videListeCoup( &lc );
   genereCoupCapture( &lc , p, plateau_getCoteCourant(p) ); 

   listecoup_noteCapture( &lc );
   listecoup_trie( &lc , 0 , 1 );

debut_recherche:
										
   for ( i=0 ; i < listecoup_getNbCoup(&lc) ; i++ ) {

      plateau_joueCoup( p , listecoup_getCoup( &lc , i) , &undo );
	  
      val = -rechercheCalme( p , -beta, -alpha , plateau_getCoteCourant(p) , gt , profondeur - 1 );
	  
      plateau_annuleCoup( p , listecoup_getCoup( &lc , i ) , &undo );

	  if (val > bestScore) {
		   bestScore = val;
		   if (val > alpha) {
			  alpha = val;
			  if (val >= beta) return val;
		  }
	  }
		  
  /*    if (val > alpha) {
		if (val >= beta)  return val; 
		alpha = val;
      }*/
	  
      listecoup_trie( &lc , i+1 , 1 );
   }

   return bestScore;
}
/*--------------------------------------------------------------------------------------*/

int alphaBetaSimple( Plateau *p , short profondeur , int alpha , int beta , Couleur c , ListeCoup *pmv ,GestionTemps *gt , Boolean nullMoveAutorise ) {
 ListeCoup lc , mv;
 AnnuleCoup undo;
 int i,val;
 Coup bestCoup;
 int scorePos = MOINS_INFINI;


                       /*------------------------------------------------------------------*/
                       /* Si la position a deja ete rencontree 3 fois alors c'est match nul */
                       /*------------------------------------------------------------------*/

 infoRechercheRacine.nbNoeudsTotal++;
       /* Ne rien mettre avant l'initialisation de pmv. Pour eviter de retourner un score dans initialiser la liste des coups */
 listecoup_videListeCoup( pmv );

 if ( hashRepet_get( p->cle ) == 3 )   return 0;


                       /*-------------------------------------------------------------------------*/
                       /* Je suis a la fin de la branche je retourne donc le score de la position */
                       /*-------------------------------------------------------------------------*/

  if (profondeur == 0 ) {
    scorePos = rechercheCalme( p , alpha , beta , c , gt , profondeur);
    return scorePos;
  }

                       /*--------------------------*/
                       /*  Encore assez de temps ? */
                       /*--------------------------*/

  if ( infoRechercheRacine.nbNoeudsTotal & 1 ) {
    if ( timer_arreteRecherche( p ,gt ) ) longjmp(env,1);
  }
  
#if USE_NULL_MOVE 		

  if ( profondeur > 2 && !estEchec(p) && nullMoveAutorise ) {
	
  	plateau_joueNullMove( p , (&undo) );
  	val =  -alphaBetaSimple( p , profondeur -1 - 2 ,   -beta , -beta+1  , plateau_getCoteCourant(p) , &mv ,gt ,FAUX ); 
	plateau_annuleNullMove( p , (&undo) );
	
	if (val >= beta)  return val;  
  }
  
#endif

 listecoup_videListeCoup( &lc );
 genereCoup( &lc , p , plateau_getCoteCourant( p ) );

  if (  listecoup_getNbCoup(&lc) == 0 ) {
     if (plateau_pieceAttaqueCase( p, 
	  							   plateau_getEmplacementRoi( p ,  
															  plateau_getCoteCourant( p ) ) ,
		       		      			 couleur_oppose( plateau_getCoteCourant( p ) ) ) 
								   ) {
	  	 return (( plateau_getCoteCourant( p )== c )? -10000-profondeur : 10000+profondeur );
	 }
	 else {  /* c'est PAT */
		 return 0; /* TODO : revoir la valeur du PAT */
	 }
  }


 	/* Affecte une note à chaque coup puis les trie */

 listecoup_note(  p , profondeur , &lc , NULL , historyTable , killer ,pvCourant );							
 listecoup_trie( &lc , 0 , 1);  /* stocke les NB_TRIE meilleurs coups en debut de liste */

 bestCoup = listecoup_getCoup( &lc , 0 );

                       /*----------------------------------*/
                       /*  Boucle sur chaque coup possible */
                       /*----------------------------------*/

   for ( i=0 ; i < listecoup_getNbCoup(&lc) ; i++ ) {
	   plateau_joueCoup( p , listecoup_getCoup( &lc , i) , &undo );
	   val = - alphaBetaSimple( p , profondeur - 1 ,   -beta ,  -alpha , plateau_getCoteCourant(p) , &mv ,gt,VRAI );
	   plateau_annuleCoup( p , listecoup_getCoup( &lc , i ) , &undo );

	   if ( val >= beta ) {
			   listecoup_setCoup( pmv , 0 , listecoup_getCoup( &lc , i ) );
			   listecoup_setCle( pmv , 0 , plateau_getCle(p) );
			   listecoup_copieN( &mv , pmv , 1 );
			   return val;
	   }

	   if ( val > scorePos ) {
		   scorePos = val;
		   bestCoup = listecoup_getCoup( &lc , i);

		   listecoup_setCoup( pmv , 0 , listecoup_getCoup( &lc , i ) );
		   listecoup_setCle( pmv , 0 , plateau_getCle(p) );
		   listecoup_copieN( &mv , pmv , 1 );

		   if ( val > alpha )    alpha = val;				
	   }
	   listecoup_trie(  &lc , i+1 , 1 );
   }   // Fin boucle sur les coups

   return scorePos;
}


/*--------------------------------------------------------------------------------------*/
int alphaBeta( Plateau *p , short profondeur , int alpha , int beta , Couleur c , ListeCoup *pmv ,
	       GestionTemps *gt , Boolean nullMoveAutorise) {
	/* Alpha est la meilleur note actuel pour le joueur courant
	 * Beta la meilleur note du joueur courant au noeud superieur
	 * A chaque noeud on tente de maximiser la valeur alpha 
	 * sans depasser la valeur beta car si j'ameliore mon score alpha> beta
	 * le joueur adverse ne jouera pas ce coup car mauvais pour lui */
	
  ListeCoup lc , mv;
  AnnuleCoup undo;
  int i,val;
  PositionHash *ph=NULL;
  Coup bestCoup;
  int scorePos = MOINS_INFINI;
  short flagHash = HASH_ALPHA;
  OrdreGenerationCoup ordreGenerationCoup;

  			/*------------------------------------------------------------------*/
  			/* Si la position a deja ete rencontre 3 fois alors c'est match nul */
  			/*------------------------------------------------------------------*/
		  
  infoRechercheRacine.nbNoeudsTotal++; 
	/* Ne rien mettre avant l'initialisation de pmv. Pour eviter de retourner un score dans initialiser la liste des coups */
  listecoup_videListeCoup( pmv );

  if ( hashRepet_get( p->cle ) == 3 )   return 0;  

#if USE_NULL_MOVE 		

  if ( profondeur > 2 && !estEchec(p) && nullMoveAutorise && plateau_nullMovePossible(p) ) {
	
  	plateau_joueNullMove( p , (&undo) );
  	val =  -alphaBeta( p , profondeur -1 - 2 ,   -beta , -beta+1  , plateau_getCoteCourant(p) , &mv ,gt ,FAUX ); 
	plateau_annuleNullMove( p , (&undo) );
	
	if (val >= beta)  return val;  
  }
  
#endif
 
#if USE_HASHTABLE
 			/*-------------------------------------------------*/
 			/* REGARDE SI LA POSITION EXISTE DANS LA HASHTABLE */
 			/*-------------------------------------------------*/

		 /* TODO : avant de retourner le score present dans la hash table il faut
		  * verifier que la position n'est pas une repetition de coup car dans
		  * ce cas le score est de 0  verifier si ca peut se produire si on vide la
		  * table de hachage au debut de chaque recherche .....*/ 

  switch ( hash_get( plateau_getCle(p) , alpha , beta , profondeur , &ph ) ) {
	  case HASH_CUT  :  
			    listecoup_addCoup2( pmv , positionHash_getCoup( ph ) );
			    listecoup_setCle( pmv , listecoup_getNbCoup(pmv)-1 , plateau_getCle(p) );
				#if DEBUG_ALPHABETA
    					traceHashGet(ph,p,"Hash Cut");	// TODO : POUR DEBUG
				#endif
					positionMeilleurCoup( statCut , 0 );
			    return positionHash_getScore( ph );
			    break;
		#if 0
	  case HASH_ALPHA :  
			     if (positionHash_getScore( ph ) < beta) beta=positionHash_getScore( ph );
			     break;
	  case HASH_BETA  :  
			     if (positionHash_getScore( ph )>alpha) alpha=positionHash_getScore( ph );
			     break;
		#endif
	  case HASH_BETA  :  
	  case HASH_ALPHA :  
	  case HASH_SORT  :  
			     break;
  }

 /* fin recup hashTable */
#endif

  
 			/*-------------------------------------------------------------------------*/
 			/* Je suis a la fin de la branche je retourne donc le score de la position */
 			/*-------------------------------------------------------------------------*/
  
  if (profondeur == 0 ) { 
	  scorePos = rechercheCalme( p , alpha , beta , c , gt , profondeur);
	  
	  //scorePos =  evaluation( p  , c );  
	  //hash_put( plateau_getCle( p ) , PAS_DE_COUP , 0 , scorePos , HASH_FEUILLE );
																									#if DEBUG_ALPHABETA
																										traceHashPut( p ,PAS_DE_COUP , 0 , scorePos , "" );
	  																								//	printListeCoup( &pvCourante , lCoup );
																									//	printf("PV %s : %d\n", lCoup ,scorePos); fflush( stdout) ;
																									#endif
	  return scorePos;
  }
		
 			/*--------------------------*/
 			/*  Encore assez de temps ? */
 			/*--------------------------*/

  if ( infoRechercheRacine.nbNoeudsTotal & 1 ) {
    if ( timer_arreteRecherche( p ,gt ) ) longjmp(env,1); 
	}

 			/*-----------------------------*/
 			/*  Genere les coups possibles */
 			/*-----------------------------*/

			// On joue d'abord le coup présent dans la table de hachage
  ordreGenerationCoup = HASH;	

coup_suivant:


  listecoup_videListeCoup( &lc );

  switch ( ordreGenerationCoup ) {
    case HASH	:  
			 if ( ph != NULL ) { 
			   listecoup_addCoup2( &lc , ph->meilleurCoup ); 
			   goto debut_analyse_coup;
			 }
			 else {
			   ordreGenerationCoup++ ;
			   goto coup_suivant;
			 }
			 break;

    case AUTRE_COUP	:  genereCoup( &lc , p , plateau_getCoteCourant( p ) );
			   nbAppelGenCoup( nbGenCoup );
  }

							
  /* Retourne un mauvais score si pas de coup possible et si en echec => MAT !!! */
  /* TODO : Modifier genereCoup pour qu'il retourne lui même si c'est MAT ou PAT 
   * pour evite de retester si je suis ou non en echec */

 			/*-------------------------------------------*/
 			/*  Aucun coup possible => soit MAT soit PAT */
 			/*-------------------------------------------*/

  if (  listecoup_getNbCoup(&lc) == 0 ) {
	   if (plateau_pieceAttaqueCase( p, 
									 plateau_getEmplacementRoi( p ,  
																plateau_getCoteCourant( p ) ) ,
		       		      			 couleur_oppose( plateau_getCoteCourant( p ) ) ) 
								   ) {
	  	 return (( plateau_getCoteCourant( p )== c )? -10000-profondeur : 10000+profondeur );
	   }
	   else {  /* c'est PAT */
		 return 0; /* TODO : revoir la valeur du PAT */
	   }
  }
					
							
		
	/* Affecte une note à chaque coup puis les trie */

  listecoup_note(  p , profondeur , &lc , ph , historyTable , killer ,pvCourant );
											
  listecoup_trie( &lc , 0 , 1);  /* stocke les NB_TRIE meilleurs coups en debut de liste */
				
debut_analyse_coup:

  bestCoup = listecoup_getCoup( &lc , 0 );

 			/*----------------------------------*/
 			/*  Boucle sur chaque coup possible */
 			/*----------------------------------*/
							
    for ( i=0 ; i < listecoup_getNbCoup(&lc) ; i++ ) {

		if ( (ordreGenerationCoup != HASH)  &&  (ph != NULL) ) {
			if ( listecoup_getCoup( &lc , i) == ph->meilleurCoup ) continue;
		}

		plateau_joueCoup( p , listecoup_getCoup( &lc , i) , &undo );

					#if DEBUG_ALPHABETA
    					listecoup_addCoup2( &pvCourante , listecoup_getCoup( &lc , i) );
					#endif
	if ( i>3 ) {
		val = - alphaBeta( p , profondeur - 1 ,   -alpha-1 ,  -alpha , plateau_getCoteCourant(p) , &mv ,gt,VRAI);
        if ((val > alpha) && (val < beta)) 
			val = - alphaBeta( p , profondeur - 1 ,   -beta ,  -alpha , plateau_getCoteCourant(p) , &mv ,gt,VRAI);
     } else
		val = - alphaBeta( p , profondeur - 1 ,   -beta ,  -alpha , plateau_getCoteCourant(p) , &mv ,gt,VRAI);



     						#if DEBUG_ALPHABETA
     								traceVariante(p,alpha,beta,&mv,val);
							#endif
					
		plateau_annuleCoup( p , listecoup_getCoup( &lc , i ) , &undo );

																		#if DEBUG_ALPHABETA
				listecoup_supprimeDernierCoup( &pvCourante );
				tracePosition( p , listecoup_getCoup( &lc , i ) , &mv , val , alpha , beta );
				listecoup_supprimeDernierCoup( &pvCourante );	
				#endif


		if ( val >= beta ) { 
			hash_put( plateau_getCle( p ) , listecoup_getCoup( &lc , i) , profondeur , val , HASH_BETA ); 
			historytable_note( historyTable , listecoup_getCoup( &lc , i) , p , profondeur );
			
			killer_add( killer , listecoup_getCoup( &lc , i) , profondeur );

			listecoup_setCoup( pmv , 0 , listecoup_getCoup( &lc , i ) );
			listecoup_setCle( pmv , 0 , plateau_getCle(p) );
			listecoup_copieN( &mv , pmv , 1 );

			return val;
		}

		if ( val > scorePos ) {
			scorePos = val;
			bestCoup = listecoup_getCoup( &lc , i);

			listecoup_setCoup( pmv , 0 , listecoup_getCoup( &lc , i ) );
			listecoup_setCle( pmv , 0 , plateau_getCle(p) );
			listecoup_copieN( &mv , pmv , 1 );
		
    		if ( val > alpha ) {
				alpha = val;
				flagHash = HASH_EXACT;
			} 
		} 

	  listecoup_trie(  &lc , i+1 , 1 );
  
  }	// Fin boucle sur les coups
  
  ordreGenerationCoup++;	// Pour générer les coups suivants

  if ( ordreGenerationCoup != PLUS_DE_COUP ) goto coup_suivant;

	  																			#if DEBUG_ALPHABETA
																										printListeCoup( &mv , lCoup );
																										traceHashPut( p , bestCoup , profondeur , scorePos , lCoup );
																									#endif

  hash_put( plateau_getCle( p ) , bestCoup , profondeur , scorePos , flagHash ); 
  historytable_note( historyTable , bestCoup , p , profondeur );

  return scorePos;
}

/*-----------------------------------------------------------------------------*/

void rechercheRacine( Plateau *p , GestionTemps *gt , int alpha , int beta ) {
  int val,i,prof,k,j,ip;
  ListeCoup meilleurVariante,lc,lcPv;
  AnnuleCoup undo;
  char coup[10];
  PositionHash ph;


  listecoup_videListeCoup( &meilleurVariante );

  
				/* TODO Bug si pas le temps d'analyser a la profondeur 1*/
  infoRechercheRacine.bestCoup = PAS_DE_COUP;
  infoRechercheRacine.bestScore = MOINS_INFINI;

					/*--------------------------*/
					/*   Analyse chaque coup    */
					/*--------------------------*/
 
  for ( i=0 ; i < multipv_getNbPV( &mpv ) ; i++ ) {

	if (infoRechercheRacine.profondeur > 4) {
  	  printCoup( multipv_getPremierCoup( &mpv , i ) , coup );
   	  printf("info currmove %s currmovenumber %d\n", coup , i+1);
	  fflush(stdout);
 	} 
		

	infoRechercheRacine.numeroCoup 	= i;
	infoRechercheRacine.nbNoeudsCoup = infoRechercheRacine.nbNoeudsTotal;

	plateau_joueCoup( p , multipv_getPremierCoup( &mpv , i ) , &undo );
															#if DEBUG_ALPHABETA
   		listecoup_addCoup2(&pvCourante ,  multipv_getPremierCoup( &mpv , i )  );
	 #endif   

	pvCourant = multipv_getPv( &mpv , i );
	pv_coupSuivant( pvCourant );


	if ( i>1 ) {
		val = - alphaBeta( p , infoRechercheRacine.profondeur-1 , -alpha-1 , -alpha , 
		 				   plateau_getCoteCourant(p) , &meilleurVariante , gt ,VRAI); 

        if ((val > alpha) && (val < beta)) 
			val = - alphaBeta( p , infoRechercheRacine.profondeur-1 , -beta , -alpha , 
				 			   plateau_getCoteCourant(p) , &meilleurVariante , gt ,VRAI ); 

     } else
		val = - alphaBeta( p , infoRechercheRacine.profondeur-1 , -beta , -alpha , 
		 			   plateau_getCoteCourant(p) , &meilleurVariante , gt ,VRAI); 

 															#if DEBUG_ALPHABETA
	   traceVariante(p,alpha,beta,&meilleurVariante,val);
	 #endif

	plateau_annuleCoup( p , multipv_getPremierCoup(&mpv,i) ,&undo );

															#if DEBUG_ALPHABETA
     		listecoup_supprimeDernierCoup( &pvCourante );
	 #endif

	listecoup_videListeCoup( &lcPv );
	listecoup_addCoup2( &lcPv , multipv_getPremierCoup(&mpv,i) );
	listecoup_copieN( &meilleurVariante , &lcPv , 1 );

			/* TODO : ne pas ecraser la variante actuelle si la variante
			  actuelle est une valeur Exacte et la nouvelle variante n'est pas exacte */
		
	multipv_setPV( &mpv , i , &lcPv , val );
	multipv_setNbNoeud( &mpv , i , infoRechercheRacine.nbNoeudsTotal - infoRechercheRacine.nbNoeudsCoup );

	if ( val >= beta ) return val;

	if ( val > infoRechercheRacine.bestScore )  { 
		 infoRechercheRacine.bestCoup  =  multipv_getPremierCoup( &mpv , i );
		 infoRechercheRacine.bestScore = val;
			 
		 if ( val > alpha ) {		// Normalement inutile car alpha toujours = à bestScoreProf
			 alpha = val;
			
			 infoRechercheRacine.numeroBestCoup = i;
		 }

	} 
	     
  } /* For recherche chaque coup */

}

/*-------------------------------------------------------------------------------*/
void initScoreRacine( Plateau *p , GestionTemps *gt ) {
	AnnuleCoup undo;
	ListeCoup lcPv;
	int i,val;

	for ( i=0 ; i < multipv_getNbPV( &mpv ) ; i++ ) {

		plateau_joueCoup( p , multipv_getPremierCoup( &mpv , i ) , &undo );
															
   		val = - rechercheCalme( p , MOINS_INFINI , PLUS_INFINI , plateau_getCoteCourant(p) , gt , 0 );

		plateau_annuleCoup( p , multipv_getPremierCoup(&mpv,i) ,&undo );
												
		listecoup_videListeCoup( &lcPv );
		listecoup_addCoup2( &lcPv , multipv_getPremierCoup(&mpv,i) );
	
		multipv_setPV( &mpv , i , &lcPv , val );
	}
}
/*-------------------------------------------------------------------------------*/
Coup iterateDeepening( Plateau *p , GestionTemps *gt , int *score) {
	int i,j,k,window,windowPrec;
	int prof,alpha,beta;
	int nbCoupJoue;
	ListeCoup lc;
	MonTimer tps;

  	/* DEFINIR ICI LA VARIANTE A TRACER */
  strcpy( pvDebug  , "g2h3 e4f3 h3g4 f3g4 c1c4" );
			// Pour tracer le put dans la hashTable
  strcpy( fenDebug , "7k/1r6/8/8/8/8/R7/6K1 b - - 3 2 " );  

  timer_reset( tps );
  timer_start( tps );

	infoRechercheRacine.numeroCoup = 0;
	infoRechercheRacine.bestScore	 = MOINS_INFINI;
	infoRechercheRacine.nbNoeudsTotal	 = 0;
	infoRechercheRacine.nbNoeudsCoup	 = 0;

			/*---------------------------------------*/
			/* Vide la table historique et de killer */
			/* TODO : tester en les vidant a chaque  */
			/* iteration														 */
			/*---------------------------------------*/

  historytable_reset( historyTable );
  killer_reset( killer );

			/*-----------------------------------------------------------------------------*/
			/* Genere les coups de la racine et initialie chaque variante avec chaque coup */
			/*-----------------------------------------------------------------------------*/

  listecoup_videListeCoup( &lc );
  genereCoup( &lc , p , plateau_getCoteCourant( p ) );
  multipv_init( &mpv , &lc );

			/*--------------------------------------*/
			/* Initialise le score à partir d'un QS */
			/* puis trie les coups					*/
			/*--------------------------------------*/

 // initScoreRacine( p , gt );
 // multipv_trie( &mpv );
 // multipvPrint( &mpv );

			/*-----------------------------------------------------*/
			/* Sauvegarde le nombre de coups joues sur le plateau	 */
			/* afin de pouvoir annuler tous les coups suivants a la*/
			/* fin de la recherche																 */
			/*-----------------------------------------------------*/

  nbCoupJoue = plateau_getNbCoup( p );

			/*------------------------------------------*/
			/* On regarde si la recherche a ete arretee */
			/* brutalement pour une raison de temps 		*/
			/* Si oui on annule les coups joues lors de	*/
			/* la recherche.														*/
			/*------------------------------------------*/

	if ( setjmp(env) != 0 ) {		
		while ( plateau_getNbCoup( p ) != nbCoupJoue )  plateau_annuleDernierCoup(p);
	  goto fin_recherche;
	}

			/*-------------------*/
			/* Iterate Deepening */
			/*-------------------*/
 	alpha = MOINS_INFINI;
 	beta  = PLUS_INFINI;

  for (prof=1 ; prof<MAX_PROFONDEUR ; prof++ ) {
   	alpha = MOINS_INFINI;
 	beta  = PLUS_INFINI;
  	printf("info depth %d nodes %ld\n", prof , infoRechercheRacine.nbNoeudsTotal );
	fflush(stdout);

relance_recherche:
		infoRechercheRacine.profondeur		 = prof;
		infoRechercheRacine.numeroBestCoup	 = 0;
		infoRechercheRacine.numeroBestCoup	 = 0;

	//	printf("[%d,%d]\n",alpha,beta);

 		rechercheRacine( p , gt , alpha , beta );

					/* Trie chaque coup apres chaque profondeur */
		multipv_setProfondeur( &mpv , infoRechercheRacine.numeroBestCoup , prof );
		multipv_trie( &mpv );
	//	multipvPrint( &mpv );

  //	printf("info depth %d cp %d nodes %ld\n", prof , multipv_getScore(&mpv,0) ,																				    infoRechercheRacine.nbNoeudsTotal );
//		fflush(stdout);
#if 0
	  if ( infoRechercheRacine.bestScore <= alpha || infoRechercheRacine.bestScore >= beta ) {
 			alpha = MOINS_INFINI;
 			beta  = PLUS_INFINI;
			goto relance_recherche;
		}
		else {
		 //window = abs( scoreProfPrec - infoRechercheRacine.bestScore );
		 //if ( window > windowPrec ) windowPrec = window;

		 //if ( window < 20 ) window+=15;
		 //if ( window >= 20 && window< 40 ) window+=10;
		 //if ( window >=40  ) window+=5; 

	  	 alpha = infoRechercheRacine.bestScore - 30;
	  	 beta  = infoRechercheRacine.bestScore + 30;
   	  }
#endif

	  STOCKE_DELTASCORE( infoRechercheRacine.bestScore , prof );

		 			/* Arret si go depth N */
	  if ( prof == timer_getProfondeur(gt) )  break;

		if ( prof > 6 ) {
		  printListeCoup( pv_getListeCoup(multipv_getPv(&mpv,0))  , lCoup ); 
		  printf("info score cp %d depth %d nodes %ld pv %s\n", 
		     				multipv_getScore( &mpv , 0 ) ,
		     				prof, 
		     				infoRechercheRacine.nbNoeudsTotal , 
		     				lCoup);
		  fflush( stdout );
		}


  }

fin_recherche:

				/*----------------------------------------------------------*/
				/* Si la recherche ne c'est pas arrete sur le premier coup  */
				/* on effectue un trie sur mpv pour tenir compte						*/
				/* de la recherche partielle 																*/	
				/* ---------------------------------------------------------*/

  if ( (infoRechercheRacine.numeroCoup> 0) && 
			  (infoRechercheRacine.numeroCoup <multipv_getNbPV( &mpv )) ) {	
	 	 multipv_setProfondeur( &mpv , infoRechercheRacine.numeroBestCoup , 
				 									  infoRechercheRacine.profondeur );
     multipv_trie( &mpv );
  }

  timer_stop( tps );

  printListeCoup( pv_getListeCoup(multipv_getPv(&mpv,0))  , lCoup ); 
   // TODO BUG : tester que timer_delta( tps ) != 0 car sinon division par zero
  printf("info score cp %d depth %d nodes %ld pv %s\n", 
		     			multipv_getScore( &mpv , 0 ) ,
		     			prof, 
		     			infoRechercheRacine.nbNoeudsTotal , 
		     		//	(int)((nbFeuilles/timer_delta( tps))*1000),
		     		//	timer_delta( tps),
		     			lCoup);

  fflush( stdout );
     
 
  if ( multipv_getNbPV( &mpv ) == 0 ) return PAS_DE_COUP;

  printf("bestmove ");
  printCoup( multipv_getPremierCoup( &mpv , 0 ) , coup ); 
  printf("%s",coup);

  if ( listecoup_getNbCoup( pv_getListeCoup( multipv_getPv(&mpv,0) )  ) > 1 ) {
	printCoup( listecoup_getCoup( pv_getListeCoup( multipv_getPv(&mpv,0) ) ,1 ) , coup ); 
	printf(" ponder %s",coup);
  }

  printf("\n");
  fflush( stdout );

  if ( infoRecherche.genEpd ) {
	  plateau_getFen( p , fen );
	  fprintf( infoRecherche.fileEpd , "%s bm %s;\n", fen , coup );
  }


  *score = multipv_getScore( &mpv , 0 );
  return multipv_getPremierCoup( &mpv , 0 );

}
/*-------------------------------------------------------------------------------*/

long minmax( Plateau *p, short profondeur , Couleur co , Coup *c ,GestionTemps *gt) {
  int i;
  ListeCoup lc;
  AnnuleCoup undo;
  int eval = 0;
  int bestScore,qsScore;
  Coup bestCoup,coupFils;
  char coup[10];
  int j;
  
  if ( profondeur== 0 ) {
    *c = PAS_DE_COUP;
    //return evaluation(p,co);
	//plateau_getFen( p , fen );
	qsScore=  rechercheCalme( p , MOINS_INFINI , PLUS_INFINI , co , gt , profondeur);
//	printf("MM %s : %d\n",fen , qsScore ); fflush( stdout );
	return qsScore;
  }

  bestScore = MOINS_INFINI;

  listecoup_videListeCoup(&lc);
  
  genereCoup( &lc , p , co ) ;

			/* GESTION DU MAT ET DU PAT */
  if (  listecoup_getNbCoup(&lc) == 0 ) {
	   if (plateau_pieceAttaqueCase( p,  
		                            plateau_getEmplacementRoi(  p ,  
																plateau_getCoteCourant( p ) ) ,
		       		      										couleur_oppose( plateau_getCoteCourant( p ) ) ) 
			                        ) {
		*c = PAS_DE_COUP;
	  	return (( plateau_getCoteCourant( p )== co )? -10000-profondeur : 10000+profondeur );
	   }
	   else {  /* c'est PAT */
		   *c = PAS_DE_COUP;
		   return 0; /* TODO : revoir la valeur du PAT */
	   }

  }


  for ( i=0 ; i<listecoup_getNbCoup(&lc) ; i++ ) {

     plateau_joueCoup( p , listecoup_getCoup( &lc , i) , &undo );
     eval = -minmax( p , profondeur-1 , couleur_oppose(co) , &coupFils ,gt );
     plateau_annuleCoup( p , listecoup_getCoup( &lc , i ) , &undo );

    /* 	  for(j=0;j<=4-profondeur;j++) printf(" ");
	  printCoup( listecoup_getCoup( &lc , i ) , coup );
	  printf("coup : %s  score %d\n", coup , eval); */

	 if ( eval > bestScore ) {
		bestScore = eval;
		bestCoup  = listecoup_getCoup( &lc , i);
	 }

  } 


  *c = bestCoup;
  return bestScore;
}

