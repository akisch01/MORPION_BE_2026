#include <stdio.h>
#include <string.h>
#include "ui.h"
#include "help.h"
#include "utils.h"

// Affiche une section spécifique du fichier d'aide
void afficher_section_aide(const char *section_tag) {
    FILE *fichier = fopen("data/help.txt", "r");
    if (!fichier) {
        printf("⚠️  Impossible de charger le fichier d’aide (help.txt).\n");
        attendre_entree();
        return;
    }

    effacer_ecran();
    printf("\n\033[1;33m=== %s ===\033[0m\n\n", section_tag + 1); // Affiche le titre de la section

    char ligne[256];
    int in_section = 0;
    while (fgets(ligne, sizeof(ligne), fichier)) {
        // Si on trouve le tag de la section
        if (strstr(ligne, section_tag) == ligne) {
            in_section = 1;
            continue;
        }
        // Si on trouve un autre tag, on arrête la lecture de la section
        if (in_section && ligne[0] == '[') {
            break;
        }
        // Si on est dans la bonne section, on affiche la ligne
        if (in_section) {
            printf("%s", ligne);
        }
    }

    fclose(fichier);
    printf("\n\033[36mAppuyez sur Entrée pour revenir au menu d'aide.\033[0m\n");
    attendre_entree();
}

void afficher_aide() {
    int quitter = 0;
    while (!quitter) {
        effacer_ecran();
        printf("\n=== AIDE INTERACTIVE ===\n\n");
        printf("1. Règles du jeu\n");
        printf("2. Commandes disponibles\n");
        printf("3. À propos\n");
        printf("4. Retour\n\n");
        printf("Votre choix : ");

        int choix = 0;
        char buffer[16];
        lire_entree_utilisateur(buffer, sizeof(buffer));
        if (sscanf(buffer, "%d", &choix) != 1) {
            choix = -1; // Invalid input
        }

        switch (choix) {
            case 1:
                afficher_section_aide("[REGLES]");
                break;
            case 2:
                afficher_section_aide("[COMMANDES]");
                break;
            case 3:
                afficher_section_aide("[APROPOS]");
                break;
            case 4:
                quitter = 1;
                break;
            default:
                printf("Choix invalide. Réessayez.\n");
                pause_courte(600);
        }
    }
}
