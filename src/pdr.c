#include "pdr.h"

void exemples(Plateau * p)
{
	/*
		Le code ci-dessous est un ensemble d'exemples pour comprendre la mani�re d'utiliser les
		BitBoard.
	*/

	Epd e;
	// Position pr�sente dans le document PDF
	char fen[] = "r2q1rk1/ppp1ppbp/2n3p1/1B1n4/3P2b1/4PN2/PP3PPP/RNBQ1RK1 w - - 2 9";

	initEpd(&e, fen);
	plateau_setPosition(p, e);

	printf("\n\nAffiche le bitboard des pions blancs sous forme d'echiquier\n");
	printBitBoard(p->bbPion[BLANC]);

	printf("\n\nAffiche le bitboard des pions blancs et noirs sous forme d'echiquier\n");
	printBitBoard(p->bbPion[BLANC] | p->bbPion[NOIR]);

	printf("\n\nAffiche le bitboard des pions blancs et noirs en ligne ( mode binaire )\n");
	printBitBoardLigne(p->bbPion[BLANC] | p->bbPion[NOIR]);

	printf("\n\nAffiche le bitboard des pions blancs et noirs en ligne ( mode hexadecimal )\n");
	printBitBoardHexa(p->bbPion[BLANC] | p->bbPion[NOIR]);

	printf("\n\nAffiche  les cases o� se trouvent les cavaliers blancs\n");
	BitBoard bb = p->bbCavalier[BLANC];		// Copie de l'entier de 64 bits dans bb ( ce bitboard contient l'emplacement des cavaliers BLANC )
	short ca;								// numero de la case sur l'echiquier

	while (bb) {							// Tant que bb contient au moins un bit �gal � 1 c'est qu'il y a encore des cavaliers sur l'�chiquier
		ca = getFirstBit(bb);				// Retourne l'emplacement du premier 1 dans le bitboard. ( ce qui correspond au num�ro de la case sur l'�chiquier )
		printf("Cavalier sur la case %d\n", ca);
		videBit(bb, ca);					// Positionne � 0 le bit correspondant � la case "ca" dans le bitboard 
	}

	
	short nbPionBlancs = getNbBits(p->bbPion[BLANC]);   // Nombre de 1 dans le BitBoard p->bbPion[BLANC]
	printf("\n\nIl y a %d pions blancs sur l'�chiquier\n" , nbPionBlancs);

	printf("\n\nIl y a %d pieces blanches sur les 2 premi�res lignes\n", getNbBits_ligne12( p->bbAllPiece[ BLANC ]  ) );

	printf("\n\nIl y a %d pieces sur les 2 premi�res lignes\n", getNbBits_ligne12( p->bbAllPiece[BLANC] | p->bbAllPiece[NOIR]));

	 
	printf("\n\nBitBoard : La case  C2 \n");
	BitBoard c2 = getCaseToBitBoard( C2 );  // c2 � comme num�ro de case 10 ( C2 est un entier qui vaut 10 )
	printBitBoard( c2 );

	printf("\n\nBitBoard : Toutes les cases sauf C2\n");
	BitBoard complementC2 = getComplementCaseToBitBoard( C2 );
	printBitBoard( complementC2 );

	printf("\n\nBitBoard : deplacement d'une tour en c2\n");
	printBitBoard( getDeplacementTour( C2 ) );

	printf("\n\nBitBoard des pieces presentes sur la ligne de la case c2\n");
	printBitBoardLigne(plateau_getLigne(p, C2));  // Les 8 premiers bits contiennent l'emplacement des pieces sur les lignes de la case ca.
												  // les autres bits de 9 � 63 sont � 0.
												  // Le resultat est un entier cod� sur 8 bits donc compris entre 0 et 255
												  // Cet entier permet d'indexer des tableaux. 

	// Exemple d'utilisation de l'entier cod� sur 8 bits pour indexer des tableaux 
	// D�terminer les d�placements possibles de la  tour a8 sur une ligne 8 et la colonne 1
	
	// plateau_getLigne(p,A8) est un entier en 0 et 255 correspondant � la ligne 8 de l'�chiquier
	// A partir de cet entier et de la case A8 on r�cupere un BitBoard contenant les d�placements possibles
	// de la tour A8. Le bitboard inclu la case de capture m�me si la pi�ce est de m�me couleur ( Ici la dame noire d8 et le pion noir h7)
	BitBoard deplacementTourA8Ligne = getDeplacementTourLigne(A8, plateau_getLigne(p,A8) );  
	printf("\n\nDeplacement de la tour A8 sur la ligne 8 ( inclu la case d8 de la dame noire ) \n");
	printBitBoard(deplacementTourA8Ligne);

	BitBoard deplacementTourA8Colonne = getDeplacementTourColonne(A8, plateau_getColonne(p, A8));
	printf("\n\nDeplacement de la tour A8 sur la colonne A ( inclu la case h7 du pion noir ) \n");
	printBitBoard(deplacementTourA8Colonne);

	// Pour obtenir le d�placement r�el de la tour A8, il suffit de faire un ou logique entre ces 2 bitboards et un et logique avec le bitboard de toutes
	// les cases ne contenant pas de pieces noires ( ce que retourne plateau_getCaseVideEtPiecesAdverses )
	printf("\n\nDeplacement de la tour A8 \n");
	BitBoard deplacementTour = ( deplacementTourA8Ligne | deplacementTourA8Colonne ) & plateau_getCaseVideEtPiecesAdverses(p, NOIR);
	printBitBoard( deplacementTour );


	// Deplacement du fou G4 sur les 2 diagonales
	BitBoard deplacementFouG4Diagonale1 = getDeplacementFouA8H1( G4, plateau_getDiagonaleA8H1(p, G4));
	printf("\n\nDeplacement du fou G4 sur la diagonale c8-h3\n");
	printBitBoard(deplacementFouG4Diagonale1);

	BitBoard deplacementFouG4Diagonale2 = getDeplacementFouA1H8(G4, plateau_getDiagonaleA1H8(p, G4));
	printf("\n\nDeplacement du fou G4 sur la diagonale d1-h5\n");
	printBitBoard(deplacementFouG4Diagonale2);

	printf("\n\nDeplacement du fou G4 \n");
	BitBoard deplacementFouG4 = (deplacementFouG4Diagonale1 | deplacementFouG4Diagonale2) & plateau_getCaseVideEtPiecesAdverses(p, NOIR);
	printBitBoard(deplacementFouG4);

	// Test pour savoir si le fou G4 peut capturer une piece
	BitBoard captureDuFouG4 = (deplacementFouG4Diagonale1 | deplacementFouG4Diagonale2) & p->bbAllPiece[BLANC];

	if ( captureDuFouG4 ) {
		printf("Le fou g4 peut capturer une piece. Les pieces qui peuvent etre capture sont :");
		printBitBoard(captureDuFouG4);
	}

	//R�cup�rer les cases que le pion d4 attaque
	printf("\n\nLes cases que le pion blanc d4 attaque\n");
	printBitBoard(getDeplacementPionCapture(D4, BLANC));

	//R�cup�rer de mani�re dynamique le tableau de bitboard des cavaliers noirs et blancs 
	// getPieceToBitBoard(CAVALIER); => �quivalent � p->bbCavalier
	printf("\n\nEmplacement des cavaliers blancs r�cup�r�s de mani�re dynamique\n");
	printBitBoard( getPieceToBitBoard(CAVALIER)[BLANC] );

	// Bitboard des cases situ�es entre 2 cases donn�es
	printf("\n\nCases entre b4 et f4 \n");
	printBitBoard(getCasesEntreDeuxCases( B4 , F4 ));
	printf("\n\nCases entre b4 et f7 \n");
	printBitBoard(getCasesEntreDeuxCases( B4, E7));
	printf("\n\nCases entre b4 et c6 ( bitboard vide car les cases sont ni sur la m�me ligne ni sur la m�me colonne )\n");
	printBitBoard(getCasesEntreDeuxCases(B4, C6));

	// Ligne , colonne , diagonale passant par 2 cases donn�es
	printf("\n\nligne passant par B3 et G3\n");
	printBitBoard(getDroiteContenantCases(B3, G3));

	printf("\n\ndiagonale passant par C3 et G7\n");
	printBitBoard(getDroiteContenantCases(G7, C3));

	// Cases devant un pion blanc
	/* exemple : ( o est un poin sur la ligne 5, x les cases contenu dans le BitBoard ) 
		     x x x
	         x x x
	         x x x
		       o
	*/
	printf("\n\nCases devant le pion D4.\n");
	printBitBoard( getPionPasseBlanc(D4) );

	//Le pion D4 est-il un pion pass� ?
	if ( (getPionPasseBlanc(D4) & p->bbPion[NOIR]) !=0 ) {
		printf("\n\nLe pion D4 n'est pas un pion pass�\n");
	}

	/* getPionAcolyteBlanc : 
	   exemple : o un pion en d3, X est le BitBoard retourn� par getPionAcolyteBlanc( D3 )

	        x o x
	        x   x
	        x   x
	*/

	/* getPionAcolyteNoir :
	   exemple : o un pion en d3, X est le BitBoard retourn� par getPionAcolyteNoir( D3 )

			X	X
			X	X
			X	X
			x   x
			x   x
			x o x
	*/

	// Savoir si un pion est arriere , isol�
	if ((getPionAcolyteBlanc(E3) & p->bbPion[BLANC]) == 0) {			/* pion arriere */

		if ((getPionAcolyteNoir(E3) & p->bbPion[BLANC]) == 0) {			/* pion isole */
		}
	}

	// R�cup�rer toutes les cases vides de l'�chiquier
	printf("\n\nCases vide de l'echiquier.\n");
	printBitBoard( plateau_getCaseVide(p) );

	// R�cup�rer toute les pi�ces NOIR qui attaque la case D4
	printf("\n\nLe fou g7 et le cavalier c6 atatque le pion D4\n");
	printBitBoard(plateau_pieceAttaqueCase(p, D4, NOIR));

	// Est-ce le d�but , milieu ou la fin de la partie
	printf("\n\nEst-ce le debut de partie : %d \n" , plateau_isOpening(p) );
	printf("\n\nEst-ce le milieu de partie : %d \n", plateau_isMiddleGame(p));
	printf("\n\nEst-ce la fin de partie : %d \n", plateau_isEndGame(p));

	// Cases o� se trouvent les rois
	printf("\n\nLe roi blanc est sur la case numero %d\n" , plateau_getEmplacementRoi(p, BLANC ));
	printf("\n\nLe roi noir est sur la case numero %d\n", plateau_getEmplacementRoi(p, NOIR));
	
}
