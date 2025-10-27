/*
 * Fichier : game.c
 * Auteur  : Akpo Akisch
 * Description : Orchestration du jeu et liens entre modules
 */

#include "game.h"
#include "ui.h"
#include "save.h"
#include "stats.h"
#include "help.h"

#include <stdio.h>

void nouvelle_partie(void) {
    printf("[TODO] Créer joueurs, initialiser plateau, lancer la boucle de jeu.\n");
    attendre_entree();
}


void revisualiser_partie(void) {
    printf("[TODO] Lire et rejouer une partie (replay).\n");
    attendre_entree();
}

void lancer_tournoi(void) {
    printf("[TODO] Mode tournoi (best of N) entre joueurs/IA.\n");
    attendre_entree();
}


void action_charger_partie() {
    charger_partie();   
    attendre_entree();
}

void action_afficher_aide() {
    afficher_aide();    
    attendre_entree();
}

void action_statistiques() {
    afficher_statistiques();
    attendre_entree();
}