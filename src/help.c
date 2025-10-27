/*
 * Fichier : help.c
 * Auteur  : Akpo Akisch
 * Date    : Octobre 2025
 * Description : Gestion de l’aide interactive (lecture du fichier help.txt)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "help.h"

void afficher_aide() {
    FILE *fichier = fopen("../data/help.txt", "r");
    if (!fichier) {
        printf("⚠️  Impossible de charger le fichier d’aide (help.txt).\n");
        attendre_entree();
        return;
    }

    effacer_ecran();
    printf("\n=== AIDE INTERACTIVE ===\n\n");

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        // Colorer les sections principales
        if (strstr(ligne, "1.") || strstr(ligne, "2.") || strstr(ligne, "3.")) {
            printf("\033[1;33m%s\033[0m", ligne); // Jaune
        } else if (ligne[0] == '#') {
            // Ignorer les lignes de commentaires #
            continue;
        } else {
            printf("%s", ligne);
        }
    }

    fclose(fichier);
    printf("\n\033[36mFin de l’aide. Appuyez sur Entrée pour revenir au menu.\033[0m\n");
    attendre_entree();
}
