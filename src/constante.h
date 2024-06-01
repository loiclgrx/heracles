

#ifndef CONSTANTE_H
#define CONSTANTE_H

#define DEBUT 	0
#define MILIEU 	1
#define FIN		 	2

#define MAX_COUPS 	150
#define MAX_PROFONDEUR 	40
#define NB_KILLER	3     /* 3 killer move */

#define LIGNE1 0x00000000000000FFLL
#define LIGNE2 0x000000000000FF00LL
#define LIGNE3 0x0000000000FF0000LL
#define LIGNE4 0x00000000FF000000LL
#define LIGNE5 0x000000FF00000000LL
#define LIGNE6 0x0000FF0000000000LL
#define LIGNE7 0x00FF000000000000LL
#define LIGNE8 0xFF00000000000000LL

/* Utile pour par exemple nbBits qui a besion d'un entier sur 2 octets */
#define LIGNE1ET2 0xFFFF

#define COLONNE1 0x0101010101010101LL
#define COLONNE2 0x0202020202020202LL
#define COLONNE3 0x0404040404040404LL
#define COLONNE4 0x0808080808080808LL
#define COLONNE5 0x1010101010101010LL
#define COLONNE6 0x2020202020202020LL
#define COLONNE7 0x4040404040404040LL
#define COLONNE8 0x8080808080808080LL

#define COLONNE1A7 0x7F7F7F7F7F7F7F7FLL
#define COLONNE2A8 0xFEFEFEFEFEFEFEFELL

/* Case pour vérifier de la possibilite de roque */
#define CASE_F1G1 	0x60
#define CASE_B1C1D1 0xE
#define CASE_B8C8D8 0x0E00000000000000LL
#define CASE_F8G8 	0x6000000000000000LL

/* Pion pour la protection du roque */
#define CASE_A2B2C2 	0x700
#define CASE_F2G2H2 	0xE000
#define CASE_A7B7C7 	0x07000000000000LL
#define CASE_F7G7H7 	0xE0000000000000LL

/* emplacement du roi pour un grand roque */
#define CASE_A1B1C1	0x7 
#define CASE_A8B8C8     0x0700000000000000L	

/* emplacement du roi pour un peit roque */
#define CASE_F1G1H1	0xE0
#define CASE_F8G8H8 	0xE000000000000000LL

/* emplacement de la tour pour un petit roque */
#define CASE_E1F1	0x30
#define CASE_E8F8	0x3000000000000000LL

/* emplacement de la tour pour un grand roque */
#define CASE_C1D1E1	0x1C
#define CASE_C8D8E8	0x1C00000000000000LL

#define CASE_INIT_CAVALIER_BLANC	0x42
#define CASE_INIT_FOU_BLANC		0x24
#define CASE_INIT_PION_CENTRE_BLANC	0x1800
#define CASE_INIT_CAVALIER_NOIR		0x4200000000000000LL
#define CASE_INIT_FOU_NOIR		0x2400000000000000LL
#define CASE_INIT_PION_CENTRE_NOIR	0x18000000000000LL

#define CASE_CENTRALE			0x1818000000LL	

/* Ensemble des cases de l'echiquier en BitBoard */
#define CASE_A1		0x01
#define CASE_B1		0x02
#define CASE_C1		0x04
#define CASE_D1		0x08
#define CASE_E1		0x10	
#define CASE_F1		0x20
#define CASE_G1		0x40
#define CASE_H1		0x80

#define CASE_A2		0x100
#define CASE_B2		0x200
#define CASE_C2		0x400
#define CASE_D2		0x800
#define CASE_E2		0x1000	
#define CASE_F2		0x2000
#define CASE_G2		0x4000
#define CASE_H2		0x8000

#define CASE_A3		0x10000
#define CASE_B3		0x20000
#define CASE_C3		0x40000
#define CASE_D3		0x80000
#define CASE_E3		0x100000	
#define CASE_F3		0x200000
#define CASE_G3		0x400000
#define CASE_H3		0x800000

#define CASE_A4		0x1000000LL
#define CASE_B4		0x2000000LL
#define CASE_C4		0x4000000LL
#define CASE_D4		0x8000000LL
#define CASE_E4		0x10000000LL	
#define CASE_F4		0x20000000LL
#define CASE_G4		0x40000000LL
#define CASE_H4		0x80000000LL

#define CASE_A5		0x100000000LL
#define CASE_B5		0x200000000LL
#define CASE_C5		0x400000000LL
#define CASE_D5		0x800000000LL
#define CASE_E5		0x1000000000LL	
#define CASE_F5		0x2000000000LL
#define CASE_G5		0x4000000000LL
#define CASE_H5		0x8000000000LL

#define CASE_A6		0x10000000000LL
#define CASE_B6		0x20000000000LL
#define CASE_C6		0x40000000000LL
#define CASE_D6		0x80000000000LL
#define CASE_E6		0x100000000000LL	
#define CASE_F6		0x200000000000LL
#define CASE_G6		0x400000000000LL
#define CASE_H6		0x800000000000LL

#define CASE_A7		0x1000000000000LL
#define CASE_B7		0x2000000000000LL
#define CASE_C7		0x4000000000000LL
#define CASE_D7		0x8000000000000LL
#define CASE_E7		0x10000000000000LL	
#define CASE_F7		0x20000000000000LL
#define CASE_G7		0x40000000000000LL
#define CASE_H7		0x80000000000000LL

#define CASE_A8		0x100000000000000LL
#define CASE_B8		0x200000000000000LL
#define CASE_C8		0x400000000000000LL
#define CASE_D8		0x800000000000000LL
#define CASE_E8		0x1000000000000000LL	
#define CASE_F8		0x2000000000000000LL
#define CASE_G8		0x4000000000000000LL
#define CASE_H8		0x8000000000000000LL

#endif /* CONSTANTE_H */

