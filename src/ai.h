#ifndef AI_H
#define AI_H
#include "board.h"

/* IA simple : choisit un coup selon le niveau.
 * Pour le niveau 1, choisit une case libre aléatoire.
 * Retourne la position choisie via les pointeurs `ligne` et `colonne`.
 */
void ia_jouer_coup(int niveau, const Plateau *p, int *ligne, int *colonne);

#endif
