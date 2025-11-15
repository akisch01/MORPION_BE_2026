/*
 * Fichier : stats.c
 * Auteur  : Akpo Akisch
 * Date    : Octobre 2025
 * Description : Lecture, affichage et mise à jour des statistiques de jeu.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ui.h"
#include "stats.h"

// Affiche les statistiques actuelles depuis stats.txt
void afficher_statistiques() {
    FILE *fichier = fopen("../data/stats.txt", "r");
    if (!fichier) {
        printf("Impossible de charger le fichier de statistiques (stats.txt).\n");
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

// Mise à jour automatique après chaque partie
// Par Jean-Yves
void mettre_a_jour_statistiques(const char *joueur_gagnant, int duree) {
    (void)duree;
    if (!joueur_gagnant) return;

    FILE *f = fopen("../data/stats.txt", "a");
    if (!f) {
        printf("Impossible d'ouvrir stats.txt pour écriture.\n");
        return;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date_str[64];
    strftime(date_str, sizeof(date_str), "%d/%m/%Y %H:%M", tm_info);

    fprintf(f, "\n%s a gagné le %s", joueur_gagnant, date_str);
    fclose(f);
}
