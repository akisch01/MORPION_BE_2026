/*
 * Fichier : save.h
 * Auteur  : Akpo Akisch
 * Date    : Octobre 2025
 * Description : Gestion des sauvegardes de parties (lecture, sélection, sauvegarde, suppression).
 */

#ifndef SAVE_H
#define SAVE_H

#include "board.h"

#define MAX_COUPS 9

/* Structure pour un coup joué */
typedef struct {
    int ligne;       /* 0-2 */
    int colonne;     /* 0-2 */
    char joueur;     /* 'X' ou 'O' */
} Coup;

/* Structure pour une partie sauvegardée */
typedef struct {
    char joueur1[50];
    char joueur2[50];
    char symboleJ1;
    char symboleJ2;
    int nb_coups;                /* Nombre de coups joués */
    Coup coups[MAX_COUPS];       /* Historique des coups */
    Plateau plateau;              /* État final du plateau */
    char etat[20];               /* État de la partie : EN_COURS, VICTOIRE, MATCH_NUL */
} Partie;

void charger_partie();
/*
 * Sauvegarde une partie.
 * - si `nom_fichier_cible` == NULL : crée un fichier horodaté dans ../data/saves/
 * - si `nom_fichier_cible` != NULL : écrase/crée le fichier cible (basename attendu)
 * Retourne 0 en cas de succès, -1 sinon.
 */
int sauvegarder_partie(const char *joueur1, const char *joueur2, char symJ1, char symJ2, 
                        int nb_coups, const Coup *coups, const Plateau *plateau_final, const char *etat,
                        const char *nom_fichier_cible);
void supprimer_sauvegarde(const char *nom_fichier);

#endif