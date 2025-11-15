/* Fichier : player.c
 * Auteur  : Jean-Yves
 * Date    : Novembre 2025
 * Description : Initialisation et affichage des joueurs
 */

#include "player.h"
#include <stdio.h>
#include <string.h>

// Par Jean-Yves
void initialiser_joueur(Joueur *j, const char *nom, char symbole, int ia) {
    if (!j || !nom) return;
    strncpy(j->nom, nom, sizeof(j->nom) - 1);
    j->nom[sizeof(j->nom) - 1] = '\0';
    j->symbole = symbole;
    j->est_IA = ia ? 1 : 0;
}

// Par Jean-Yves
void afficher_joueur(const Joueur *j) {
    if (!j) return;
    printf("Joueur : %s (%c) - %s\n", j->nom, j->symbole, j->est_IA ? "IA" : "Humain");
}
