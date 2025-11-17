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

// CHARGEMENT RÉEL D'UNE SAUVEGARDE EXISTANTE - PARTIES EN COURS UNIQUEMENT
// Par Jean-Yves
void charger_partie() {
    DIR *dir = opendir(DOSSIER_SAVES);
    if (!dir) {
        printf("Impossible d'accéder au dossier des sauvegardes (%s)\n", DOSSIER_SAVES);
        attendre_entree();
        return;
    }

    struct dirent *entry;
    char fichiers_en_cours[50][256];
    int nb_en_cours = 0;

    effacer_ecran();
    printf("\n=== CHARGER UNE PARTIE EN COURS ===\n\n");

    // Parcourir le répertoire et filtrer les parties EN_COURS
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (!strstr(entry->d_name, ".txt"))
            continue;

        // Vérifier si la partie est EN_COURS
        char chemin_complet[512];
        snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", DOSSIER_SAVES, entry->d_name);
        FILE *f = fopen(chemin_complet, "r");
        if (!f) continue;

        char ligne[256];
        int is_en_cours = 0;
        while (fgets(ligne, sizeof(ligne), f)) {
            if (strstr(ligne, "# État :")) {
                if (strstr(ligne, "EN_COURS")) {
                    is_en_cours = 1;
                }
                break;
            }
        }
        fclose(f);

        if (is_en_cours) {
            strcpy(fichiers_en_cours[nb_en_cours], entry->d_name);
            nb_en_cours++;
        }
    }
    closedir(dir);

    if (nb_en_cours == 0) {
        printf("Aucune partie en cours disponible.\n");
        attendre_entree();
        return;
    }

    printf("Parties en cours :\n\n");
    for (int i = 0; i < nb_en_cours; ++i)
        printf(" %d. %s\n", i + 1, fichiers_en_cours[i]);

    int choix = 0;
    printf("\nEntrez le numéro de la partie (0 pour annuler) : ");
    scanf("%d", &choix);
    while (getchar() != '\n');

    if (choix == 0) {
        printf("\nRetour au menu principal...\n");
        pause_courte(600);
        return;
    }

    if (choix < 1 || choix > nb_en_cours) {
        printf("\nEntrée invalide.\n");
        attendre_entree();
        return;
    }

    // Charger la partie sélectionnée
    char chemin_complet[512];
    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", DOSSIER_SAVES, fichiers_en_cours[choix - 1]);
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
        } else if (strstr(ligne, "# État :")) {
            sscanf(ligne, "# État : %s", partie.etat);
        }
    }
    fclose(f);

    // Afficher le résumé
    effacer_ecran();
    printf("\nPartie chargée depuis : %s\n", fichiers_en_cours[choix - 1]);
    afficher_resume_partie(&partie);
    
    // Menu : Continuer ou Supprimer
    while (1) {
        printf("1. Reprendre cette partie\n");
        printf("2. Supprimer cette partie\n");
        printf("\nChoisissez une option : ");
        
        int action = 0;
        if (scanf("%d", &action) != 1) {
            while (getchar() != '\n');
            printf("Entrée invalide.\n");
            continue;
        }
        while (getchar() != '\n');

        if (action == 1) {
            // TODO : Continuer la partie
            printf("\n[TODO] Reprendre la partie en cours...\n");
            attendre_entree();
            break;
        } else if (action == 2) {
            // Supprimer la partie
            printf("\nConfirmez-vous la suppression ? (O/N) : ");
            char confirm;
            if (scanf("%c", &confirm) == 1) {
                while (getchar() != '\n');
                if (confirm == 'O' || confirm == 'o') {
                    supprimer_sauvegarde(fichiers_en_cours[choix - 1]);
                    attendre_entree();
                    break;
                }
            } else {
                while (getchar() != '\n');
            }
            printf("Suppression annulée.\n");
        } else {
            printf("Choix invalide. Réessayez.\n");
        }
    }
}

// AFFICHAGE DU RÉSUMÉ DE LA PARTIE CHARGÉE
// Par Jean-Yves
void afficher_resume_partie(const Partie *p) {
    printf("\nRésumé de la partie :\n");
    printf("----------------------------\n");
    printf("Joueur 1 : %s (%c)\n", p->joueur1, p->symboleJ1);
    printf("Joueur 2 : %s (%c)\n", p->joueur2, p->symboleJ2);
    printf("Nombre de coups : %d\n", p->nb_coups);
    if (p->etat[0] != '\0') {
        printf("État : %s\n", p->etat);
    }
    printf("\nAppuyez sur Entrée pour continuer...\n");
}

// SAUVEGARDE D'UNE PARTIE AVEC HISTORIQUE
// Par Jean-Yves
int sauvegarder_partie(const char *joueur1, const char *joueur2, char symJ1, char symJ2, 
                        int nb_coups, const Coup *coups, const Plateau *plateau_final, const char *etat,
                        const char *nom_fichier_cible) {
    if (!joueur1 || !joueur2 || !coups || !plateau_final) return -1;

    // Créer le répertoire s'il n'existe pas
    #ifdef _WIN32
        system("if not exist \"../data/saves\" mkdir \"../data/saves\"");
    #else
        system("mkdir -p ../data/saves");
    #endif

    char nom_fichier[256];
    char chemin_complet[512];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    if (nom_fichier_cible && nom_fichier_cible[0] != '\0') {
        // Utiliser le nom passé (on attend un basename, p.ex. partie_....txt)
        strncpy(nom_fichier, nom_fichier_cible, sizeof(nom_fichier));
        nom_fichier[sizeof(nom_fichier)-1] = '\0';
    } else {
        // Générer un nom horodaté
        strftime(nom_fichier, sizeof(nom_fichier), "partie_%Y-%m-%d_%H-%M-%S.txt", tm_info);
    }

    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", DOSSIER_SAVES, nom_fichier);

    FILE *f = fopen(chemin_complet, "w");
    if (!f) {
        printf("Impossible de sauvegarder la partie (chemin: %s).\n", chemin_complet);
        return -1;
    }

    // En-tête de sauvegarde
    char date_str[64];
    strftime(date_str, sizeof(date_str), "%d/%m/%Y %H:%M:%S", tm_info);
    
    fprintf(f, "#############################################\n");
    fprintf(f, "# Sauvegarde automatique du : %s\n", date_str);
    fprintf(f, "# Joueur 1 : %s (%c)\n", joueur1, symJ1);
    fprintf(f, "# Joueur 2 : %s (%c)\n", joueur2, symJ2);
    fprintf(f, "# Nombre de coups : %d\n", nb_coups);
    fprintf(f, "# État : %s\n", etat ? etat : "");
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

    printf("Partie sauvegardée : %s\n", nom_fichier);
    return 0;
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

// REPRISE D'UNE PARTIE EN COURS
// Par Jean-Yves
void reprendre_partie(void) {
    printf("[TODO] Reprendre une partie en cours à partir du dernier coup sauvegardé.\n");
    attendre_entree();
}
