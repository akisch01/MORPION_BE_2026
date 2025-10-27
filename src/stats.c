/*
 * Fichier : stats.c
 * Auteur  : Akpo Akisch
 * Date    : Octobre 2025
 * Description : Lecture, affichage et mise à jour des statistiques de jeu.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "stats.h"

// 📊 Affiche les statistiques actuelles depuis stats.txt
void afficher_statistiques() {
    FILE *fichier = fopen("../data/stats.txt", "r");
    if (!fichier) {
        printf("⚠️  Impossible de charger le fichier de statistiques (stats.txt).\n");
        attendre_entree();
        return;
    }

    effacer_ecran();
    printf("\n=== STATISTIQUES DU JEU ===\n\n");

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        // Colorer les titres
        if (strstr(ligne, "STATISTIQUES") || strstr(ligne, "---------------------------------------------")) {
            printf("\033[1;34m%s\033[0m", ligne); // Bleu clair
        } else if (strstr(ligne, ":")) {
            printf("\033[32m%s\033[0m", ligne); // Vert pour les données
        } else if (ligne[0] == '#') {
            // Ignorer les lignes de commentaires
            continue;
        } else {
            printf("%s", ligne);
        }
    }

    fclose(fichier);
    printf("\n\033[36mAppuyez sur Entrée pour revenir au menu principal.\033[0m\n");
    attendre_entree();
}

// Fonction à implémenter plus tard : mise à jour automatique après chaque partie
void mettre_a_jour_statistiques(const char *joueur_gagnant, int duree) {
    // TODO : Lire stats.txt, modifier les valeurs correspondantes, et sauvegarder à nouveau
    // Exemple : incrémenter les parties, mettre à jour le gagnant, recalculer la durée moyenne
}
