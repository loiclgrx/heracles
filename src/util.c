#if defined(_WIN32) || defined(_WIN64)
	#include <conio.h>
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/time.h>
#endif


#include <stdlib.h>
#include <stdio.h>
#include "type.h"
#include "boolean.h"


void strupper( char *s , int len ) {
  int i;

  for( i = 0 ; i < len ; i++ ) {
	  if ( (s[i] >= 'a')  && ( s[i] <= 'z') ) { 
	     s[i] = s[i] + 'A' - 'a';
	  }
  }
}

Boolean lireLigneFichier(FILE * file, char string[], int size) {

 //  char * ptr;

   if (fgets(string,size,file) == NULL) return FAUX;

   // suppress '\n'

 //  ptr = strchr(string,'\n');
 //  if (ptr != NULL) *ptr = '\0';

   return VRAI;
}



unsigned long long int rand64() {  
	unsigned long long int r =0,p=1;
	int i;
	r= rand() ^ ((unsigned long long int)rand() << 15) ^
				((unsigned long long int)rand() << 30) ^
				((unsigned long long int)rand() << 45) ^
				((unsigned long long int)rand() << 60);
	/*for(i=0;i<64;i++) {
		if (rand()%2) r|=p;
		p <<= 1;
	}*/
	

	printBitBoard( r );
	printBitBoard( r );
	return r;
	
}

Boolean inputDisponible() {
// return _kbhit();  // ne marche pas

#if defined(_WIN32) || defined(_WIN64)
 

    static int      init = 0,
                    pipe;
    static HANDLE   inh;
    DWORD           dw;


//#if defined(FILE_CNT)
  //  if (stdin->_cnt > 0)
    //    return stdin->_cnt;
//#endif
    if (!init) {
        init = 1;
        inh = GetStdHandle(STD_INPUT_HANDLE);
        pipe = !GetConsoleMode(inh, &dw);
        if (!pipe) {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
        }
    }
    if (pipe) {
        if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
            return 1;
        return dw;
    } else {
        GetNumberOfConsoleInputEvents(inh, &dw);
        return dw <= 1 ? 0 : dw;
    }

#else // Sous linux

   struct timeval tv = { 0, 0 };
   fd_set readfds;

   FD_ZERO(&readfds);
   FD_SET(STDIN_FILENO, &readfds);

   return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) == 1;

#endif

}
