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
#include <time.h>
#include "ui.h"
#include "save.h"
#include "utils.h"


#define DOSSIER_SAVES "../data/saves/"

void afficher_resume_partie(const Partie *p);

// CHARGEMENT RÉEL D’UNE SAUVEGARDE EXISTANTE
// Par Jean-Yves
void charger_partie() {
    DIR *dir = opendir(DOSSIER_SAVES);
    if (!dir) {
        printf("Impossible d'accéder au dossier des sauvegardes (%s)\n", DOSSIER_SAVES);
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
        printf("Aucune sauvegarde disponible pour le moment.\n");
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
        printf("Erreur d'ouverture du fichier : %s\n", chemin_complet);
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
        } else if (strstr(ligne, "# Nombre de coups :")) {
            sscanf(ligne, "# Nombre de coups : %d", &partie.nb_coups);
        }
    }

    fclose(f);

    effacer_ecran();
    printf("\nPartie chargée depuis : %s\n", fichiers[choix - 1]);
    afficher_resume_partie(&partie);
    attendre_entree();
}


// AFFICHAGE DU RÉSUMÉ DE LA PARTIE CHARGÉE
// Par Jean-Yves
void afficher_resume_partie(const Partie *p) {
    printf("\nRésumé de la partie :\n");
    printf("----------------------------\n");
    printf("Joueur 1 : %s (%c)\n", p->joueur1, p->symboleJ1);
    printf("Joueur 2 : %s (%c)\n", p->joueur2, p->symboleJ2);
    printf("Nombre de coups : %d\n", p->nb_coups);
}

// SAUVEGARDE D'UNE PARTIE AVEC HISTORIQUE
// Par Jean-Yves
void sauvegarder_partie(const char *joueur1, const char *joueur2, char symJ1, char symJ2, 
                        int nb_coups, const Coup *coups, const Plateau *plateau_final) {
    if (!joueur1 || !joueur2 || !coups || !plateau_final) return;

    // Créer le répertoire s'il n'existe pas
    #ifdef _WIN32
        system("if not exist \"../data/saves\" mkdir \"../data/saves\"");
    #else
        system("mkdir -p ../data/saves");
    #endif

    // Générer un nom de fichier avec timestamp
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char nom_fichier[256];
    strftime(nom_fichier, sizeof(nom_fichier), "partie_%Y-%m-%d_%H-%M-%S.txt", tm_info);

    char chemin_complet[512];
    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", DOSSIER_SAVES, nom_fichier);

    FILE *f = fopen(chemin_complet, "w");
    if (!f) {
        printf("Impossible de sauvegarder la partie.\n");
        return;
    }

    // En-tête de sauvegarde
    char date_str[64];
    strftime(date_str, sizeof(date_str), "%d/%m/%Y %H:%M:%S", tm_info);
    
    fprintf(f, "#############################################\n");
    fprintf(f, "# Sauvegarde automatique du : %s\n", date_str);
    fprintf(f, "# Joueur 1 : %s (%c)\n", joueur1, symJ1);
    fprintf(f, "# Joueur 2 : %s (%c)\n", joueur2, symJ2);
    fprintf(f, "# Nombre de coups : %d\n", nb_coups);
    fprintf(f, "#############################################\n\n");

    // Historique des coups
    fprintf(f, "=== HISTORIQUE DES COUPS ===\n");
    for (int i = 0; i < nb_coups; ++i) {
        fprintf(f, "Coup %d : %c joue (%d,%d)\n", i + 1, coups[i].joueur, coups[i].ligne + 1, coups[i].colonne + 1);
    }
    fprintf(f, "\n");

    // Plateau final
    fprintf(f, "=== PLATEAU FINAL ===\n");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j)
            fprintf(f, "%c ", plateau_final->cases[i][j]);
        fprintf(f, "\n");
    }

    fprintf(f, "#############################################\n");
    fclose(f);

    printf("Partie automatiquement sauvegardée : %s\n", nom_fichier);
}

// SUPPRESSION D'UNE SAUVEGARDE
void supprimer_sauvegarde(const char *nom_fichier) {
    char chemin_complet[512];
    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", DOSSIER_SAVES, nom_fichier);

    if (remove(chemin_complet) == 0)
        printf("Sauvegarde '%s' supprimée avec succès.\n", nom_fichier);
    else
        printf("Impossible de supprimer '%s'.\n", nom_fichier);
}
