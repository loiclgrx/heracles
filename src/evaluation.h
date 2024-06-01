#ifndef EVALUATION_H
#define EVALUATION_H

/* Fourni un ensemble d'informations pour effectuer l'evaluation d'une position
 * Valeur du materiel present sur l'echiquier
 * Mobilite des pieces
 * Securite du roi ....
 * */

#include "plateau.h"
#include "couleur.h"

/* TODO : Modifier le type du score mettre int16 au lieu de int */

/* Retourne la difference de mobilite entre les pieces noires et blanches. cp est un coefficient de pondération qui peut
 * varier en cours de partie (debut , milieu et la fin de partie) */
int eval_mobilite( Plateau *p , short cp );

/* Malus si piece mineur non deplace , malus si dame bouge de trop ... */
int eval_developpement( Plateau *p ); 

/* Bonus pour tous pions situes au centre
 * Bonus pour chaque piece attaquant le centre */

int eval_controleCentre(Plateau *p); 

/* Difference entre le materiel BLANC et le materiel NOIR */
#define eval_materiel( p )   plateau_getMateriel(p,BLANC)-plateau_getMateriel(p,NOIR)

int evaluation( Plateau *p , Couleur c , int alpha , int beta );

#endif /* EVALUATION_H */

