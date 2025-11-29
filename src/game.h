#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "board.h"
#include "save.h"

void nouvelle_partie(void);
void charger_partie(void);
void revisualiser_partie(void);
void lancer_tournoi(void);
void afficher_statistiques(void);
void afficher_aide(void);
void nouvelle_partie_reprise(const Partie *p);

#endif
