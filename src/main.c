 
#include <stdio.h>  	
#include <string.h>
#include <time.h>
#include "plateau.h"
#include "init.h"
#include  "bitboard.h"
#include "case.h"
#include "piece.h" 
#include "listecoup.h"
#include "epd.h"  
#include "couleur.h"
#include "generecoup.h"
#include "trace.h"
#include "constante.h"
#include "test.h"
#include "annulecoup.h"
#include "evaluation.h"
#include "valeur.h"
#include "recherche.h"
#include "uci.h"
#include "util.h"
#include "hash.h"
#include "hasheval.h"
#include "multipv.h"
#include "timer.h"
#include "constante.h"

#include "pdr.h"

//#include <sys/resource.h>

int main(int argc, char *argv[] ) { 
	Epd e;
	Plateau p;
	char fen[]="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	GestionTemps gt;
	//MultiPv mpv;
#if 0
	/* Fixe le core dump a 128 Mo */
 struct rlimit lim;
lim.rlim_cur=1024*1024*128; // 128 MB
lim.rlim_max=1024*1024*128;
 setrlimit(RLIMIT_CORE,&lim); 
#endif
   setbuf(stdin,NULL);
   setbuf(stdout,NULL);
   setvbuf(stdin,NULL,_IONBF,0);
   setvbuf(stdout,NULL,_IONBF,0);

	//multipv_alloc( &mpv );

	init(&p);
	initValeur();

	initEpd( &e , fen );
	plateau_setPosition( &p , e );
	hash_creer( 32 );
	hashRepet_effacer();

	hasheval_creer( &hashPion[ BLANC ] , 1000 ); // 500 Ko
	hasheval_effacer( &hashPion[ BLANC ]  );

	hasheval_creer( &hashPion[ NOIR ] , 1000 ); // 500 Ko
	hasheval_effacer( &hashPion[ NOIR ]  );

	hasheval_creer( &hashPionMixte , 1000 ); // 500 Ko
	hasheval_effacer( &hashPionMixte  );

	//exemples(&p);
	wait(&p,&gt);	

	return 1;
	
}
