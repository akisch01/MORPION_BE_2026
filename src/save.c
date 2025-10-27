/*
 * Fichier : save.c
 * Auteur  : Akpo Akisch
 * Date    : Octobre 2025
 * Description : Gestion de la lecture, écriture et sélection des fichiers de sauvegarde.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "ui.h"
#include "save.h"
#include "utils.h"


#define DOSSIER_SAVES "../data/saves/"

void afficher_resume_partie(const Partie *p);

// CHARGEMENT RÉEL D’UNE SAUVEGARDE EXISTANTE
void charger_partie() {
    DIR *dir = opendir(DOSSIER_SAVES);
    if (!dir) {
        printf("⚠️  Impossible d'accéder au dossier des sauvegardes (%s)\n", DOSSIER_SAVES);
        attendre_entree();
        return;
    }

    struct dirent *entry;
    char fichiers[50][256];
    int nb_saves = 0;

    effacer_ecran();
    printf("\n=== CHARGER UNE PARTIE ===\n\n");

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (strstr(entry->d_name, ".txt")) {
            strcpy(fichiers[nb_saves], entry->d_name);
            nb_saves++;
        }
    }
    closedir(dir);

    if (nb_saves == 0) {
        printf("😕 Aucune sauvegarde disponible pour le moment.\n");
        attendre_entree();
        return;
    }

    printf("Sauvegardes disponibles :\n\n");
    for (int i = 0; i < nb_saves; ++i)
        printf(" %d. %s\n", i + 1, fichiers[i]);

    int choix = 0;
    printf("\nEntrez le numéro de la sauvegarde à charger (0 pour annuler) : ");
    scanf("%d", &choix);
    while (getchar() != '\n');

    if (choix == 0) {
        printf("\nRetour au menu principal...\n");
        pause_courte(600);
        return;
    }

    if (choix < 1 || choix > nb_saves) {
        printf("\nEntrée invalide.\n");
        attendre_entree();
        return;
    }

    char chemin_complet[512];
    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", DOSSIER_SAVES, fichiers[choix - 1]);
    FILE *f = fopen(chemin_complet, "r");
    if (!f) {
        printf("⚠️  Erreur d'ouverture du fichier : %s\n", chemin_complet);
        attendre_entree();
        return;
    }

    Partie partie;
    memset(&partie, 0, sizeof(Partie));

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), f)) {
        if (strstr(ligne, "# Joueur 1 :")) {
            sscanf(ligne, "# Joueur 1 : %[^ (] (%c)", partie.joueur1, &partie.symboleJ1);
        } else if (strstr(ligne, "# Joueur 2 :")) {
            sscanf(ligne, "# Joueur 2 : %[^ (] (%c)", partie.joueur2, &partie.symboleJ2);
        } else if (strstr(ligne, "# Tour actuel :")) {
            sscanf(ligne, "# Tour actuel : %d", &partie.tour);
        } else if (strncmp(ligne, "Plateau :", 9) == 0) {
            for (int i = 0; i < 3; ++i) {
                fgets(ligne, sizeof(ligne), f);
                sscanf(ligne, "%c %c %c", &partie.plateau.cases[i][0],
                                          &partie.plateau.cases[i][1],
                                          &partie.plateau.cases[i][2]);
            }
        }
    }

    fclose(f);

    effacer_ecran();
    printf("\n✅ Partie chargée depuis : %s\n", fichiers[choix - 1]);
    afficher_resume_partie(&partie);
    attendre_entree();
}


// AFFICHAGE DU RÉSUMÉ DE LA PARTIE CHARGÉE
void afficher_resume_partie(const Partie *p) {
    printf("\nRésumé de la partie :\n");
    printf("----------------------------\n");
    printf("Joueur 1 : %s (%c)\n", p->joueur1, p->symboleJ1);
    printf("Joueur 2 : %s (%c)\n", p->joueur2, p->symboleJ2);
    printf("Tour actuel : %d\n", p->tour);
    printf("\nPlateau actuel :\n");

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j)
            printf(" %c ", p->plateau.cases[i][j]);
        printf("\n");
    }
}


// SAUVEGARDE RÉELLE D'UNE PARTIE
void sauvegarder_partie(const char *nom_fichier) {
    Partie partie;

    // Génération du chemin complet
    char chemin_complet[512];
    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", DOSSIER_SAVES, nom_fichier);

    // Récupération de la date actuelle
    char date[64];
    obtenir_date_actuelle(date, sizeof(date));

    FILE *f = fopen(chemin_complet, "w");
    if (!f) {
        printf("⚠️  Erreur : impossible de créer le fichier %s\n", chemin_complet);
        attendre_entree();
        return;
    }

    // Écriture dans le fichier
    fprintf(f, "#############################################\n");
    fprintf(f, "# Sauvegarde du : %s\n", date);
    fprintf(f, "# Joueur 1 : %s (%c)\n", partie.joueur1, partie.symboleJ1);
    fprintf(f, "# Joueur 2 : %s (%c)\n", partie.joueur2, partie.symboleJ2);
    fprintf(f, "# Tour actuel : %d\n", partie.tour);
    fprintf(f, "#############################################\n");
    fprintf(f, "Plateau :\n");

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j)
            fprintf(f, "%c ", partie.plateau.cases[i][j]);
        fprintf(f, "\n");
    }

    fprintf(f, "#############################################\n");
    fclose(f);

    printf("\n💾 Partie sauvegardée avec succès dans : %s\n", chemin_complet);
    attendre_entree();
}


// SUPPRESSION D'UNE SAUVEGARDE
void supprimer_sauvegarde(const char *nom_fichier) {
    char chemin_complet[512];
    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", DOSSIER_SAVES, nom_fichier);

    if (remove(chemin_complet) == 0)
        printf("🗑️  Sauvegarde '%s' supprimée avec succès.\n", nom_fichier);
    else
        printf("⚠️  Impossible de supprimer '%s'.\n", nom_fichier);
}
