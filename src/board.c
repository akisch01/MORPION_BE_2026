/*
 * Fichier : board.c
 * Description : Gestion du plateau de jeu (affichage et logique basique)
 */

#include "board.h"
#include <stdio.h>

void initialiser_plateau(Plateau *p) {
    // TODO : remplir toutes les cases avec ' '
}

void afficher_plateau(const Plateau *p) {
    // TODO : afficher le plateau avec les symboles
}

int jouer_coup(Plateau *p, int ligne, int colonne, char symbole) {
    // TODO : vérifier et jouer le coup
    return 1;
}

int verifier_victoire(const Plateau *p, char symbole) {
    // TODO : vérifier victoire
    return 0;
}

int verifier_match_nul(const Plateau *p) {
    // TODO : vérifier match nul
    return 0;
}
