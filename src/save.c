/*
 * Fichier : save.c
 * Auteur  : Jean-Yves
 * Date    : Octobre 2025
 * Description : Gestion de la lecture, écriture et sélection des fichiers de sauvegarde.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#ifdef _WIN32
#include <direct.h>
#endif
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#include "ui.h"
#include "save.h"
#include "utils.h"

void afficher_resume_partie(const Partie *p);

// CHARGEMENT RÉEL D'UNE SAUVEGARDE EXISTANTE - PARTIES EN COURS UNIQUEMENT
// Par Jean-Yves
void charger_partie() {
    const char *saves_path = obtenir_chemin_saves();
    DIR *dir = opendir(saves_path);
    if (!dir) {
        printf("Impossible d'accéder au dossier des sauvegardes (%s)\n", saves_path);
        attendre_entree();
        return;
    }

    struct dirent *entry;
    char fichiers_en_cours[50][MAX_SAVE_FILENAME_LEN];
    int nb_en_cours = 0;

    effacer_ecran();
    printf("\n  CHARGER UNE PARTIE EN COURS  \n\n");

    // Parcourir le répertoire et filtrer les parties EN_COURS
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (!strstr(entry->d_name, ".txt"))
            continue;

        // Vérifier si la partie est EN_COURS
        char chemin_complet[PATH_MAX];
        snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", saves_path, entry->d_name);
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
    char buffer_choix[16];
    lire_entree_utilisateur(buffer_choix, sizeof(buffer_choix));
    if (sscanf(buffer_choix, "%d", &choix) != 1) {
        choix = -1; // Invalid input
    }

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
    char chemin_complet[PATH_MAX];
    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", saves_path, fichiers_en_cours[choix - 1]);
    Partie partie;
    memset(&partie, 0, sizeof(Partie));

    if (load_partie_from_file(chemin_complet, &partie) != 0) {
        printf("Erreur lors du chargement de la sauvegarde.\n");
        attendre_entree();
        return;
    }
    // Store basename
    strncpy(partie.nom_fichier, fichiers_en_cours[choix - 1], sizeof(partie.nom_fichier)-1);
    partie.nom_fichier[sizeof(partie.nom_fichier)-1] = '\0';

    // Afficher le résumé
    effacer_ecran();
    printf("\nPartie chargée depuis : %s\n", partie.nom_fichier);
    afficher_resume_partie(&partie);
    
    // Menu : Continuer ou Supprimer
    while (1) {
        printf("1. Reprendre cette partie\n");
        printf("2. Supprimer cette partie\n");
        printf("\nChoisissez une option : ");
        
        int action = 0;
        char buffer_action[16];
        lire_entree_utilisateur(buffer_action, sizeof(buffer_action));
        if (sscanf(buffer_action, "%d", &action) != 1) {
            action = -1; // Invalid input
        }

        if (action == 1) {
            // Continuer la partie : appeler la reprise dans game.c
            extern void nouvelle_partie_reprise(const Partie *p);
            nouvelle_partie_reprise(&partie);
            break;
        } else if (action == 2) {
            // Supprimer la partie
            printf("\nConfirmez-vous la suppression ? (O/N) : ");
            char confirm = ' ';
            char buffer_confirm[16];
            if (lire_entree_utilisateur(buffer_confirm, sizeof(buffer_confirm)) && buffer_confirm[0] != '\0') {
                confirm = buffer_confirm[0];
            }

            if (confirm == 'O' || confirm == 'o') {
                supprimer_sauvegarde(fichiers_en_cours[choix - 1]);
                attendre_entree();
                break;
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

// Lit une sauvegarde complète et remplit la struct Partie (coups + plateau)
int load_partie_from_file(const char *chemin, Partie *out) {
    if (!chemin || !out) return -1;
    FILE *f = fopen(chemin, "r");
    if (!f) return -1;

    memset(out, 0, sizeof(Partie));
    char ligne[256];
    int in_historique = 0, in_plateau = 0;
    int parsed_coups = 0;
    int plateau_row = 0;

    while (fgets(ligne, sizeof(ligne), f)) {
        if (strstr(ligne, "# Joueur 1 :")) {
            sscanf(ligne, "# Joueur 1 : %[^ (] (%c)", out->joueur1, &out->symboleJ1);
            continue;
        } else if (strstr(ligne, "# Joueur 2 :")) {
            sscanf(ligne, "# Joueur 2 : %[^ (] (%c)", out->joueur2, &out->symboleJ2);
            continue;
        } else if (strstr(ligne, "# Nombre de coups :")) {
            sscanf(ligne, "# Nombre de coups : %d", &out->nb_coups);
            continue;
        } else if (strstr(ligne, "# État :")) {
            char *p = strchr(ligne, ':');
            if (p) {
                p++; while (*p == ' ') p++;
                char *nl = strchr(p, '\n'); if (nl) *nl = '\0';
                strncpy(out->etat, p, sizeof(out->etat)-1);
            }
            continue;
        } else if (strstr(ligne, "  HISTORIQUE DES COUPS  ")) {
            in_historique = 1; in_plateau = 0; continue;
        } else if (strstr(ligne, "  PLATEAU FINAL  ")) {
            in_historique = 0; in_plateau = 1; plateau_row = 0; continue;
        }

        if (in_historique && strstr(ligne, "Coup")) {
            int coup_num, lig, col; char joueur;
            if (sscanf(ligne, "Coup %d : %c joue (%d,%d)", &coup_num, &joueur, &lig, &col) == 4) {
                if (parsed_coups < MAX_COUPS) {
                    out->coups[parsed_coups].ligne = lig - 1;
                    out->coups[parsed_coups].colonne = col - 1;
                    out->coups[parsed_coups].joueur = joueur;
                    parsed_coups++;
                }
            }
            continue;
        }

        if (in_plateau && ligne[0] != '#' && ligne[0] != '\n' && ligne[0] != '=') {
            // Le format sauvegardé est "X O   \n" (caractère, espace, caractère, espace, caractère, \n)
            // Les cases sont aux positions 0, 2, 4 de chaque ligne
            // On lit directement les caractères aux positions paires (0, 2, 4)
            int col_count = 0;
            for (int pos = 0; pos < (int)strlen(ligne) && col_count < 3 && plateau_row < 3; pos += 2) {
                if (ligne[pos] != '\n' && ligne[pos] != '\r' && ligne[pos] != '\0') {
                    out->plateau.cases[plateau_row][col_count] = ligne[pos];
                    col_count++;
                }
            }
            if (col_count > 0) plateau_row++;
            continue;
        }
        
        
    }

    fclose(f);
    // finalise le nombre de coups parsés si l'en-tête était absent ou incohérent
    if (out->nb_coups <= 0) out->nb_coups = parsed_coups;
    return 0;
}

// SAUVEGARDE D'UNE PARTIE AVEC HISTORIQUE
// Par Jean-Yves
int sauvegarder_partie(const char *joueur1, const char *joueur2, char symJ1, char symJ2, 
                        int nb_coups, const Coup *coups, const Plateau *plateau_final, const char *etat,
                        const char *nom_fichier_cible) {
    const char *saves_path = obtenir_chemin_saves();
    
    // Créer le répertoire s'il n'existe pas
    // Retirer le slash final pour mkdir
    char saves_dir[PATH_MAX];
    strncpy(saves_dir, saves_path, sizeof(saves_dir) - 1);
    saves_dir[sizeof(saves_dir) - 1] = '\0';
    size_t len = strlen(saves_dir);
    if (len > 0 && saves_dir[len - 1] == '/') {
        saves_dir[len - 1] = '\0';
    }
    
    // Créer récursivement les répertoires manuellement
    char *p = saves_dir;
    if (*p == '/') p++; // Skip leading / on Unix
    
    while (*p) {
        if (*p == '/' || *p == '\\') {
            char save_char = *p;
            *p = '\0';
            #ifdef _WIN32
                _mkdir(saves_dir);
            #else
                mkdir(saves_dir, 0755); // Ignore error if exists
            #endif
            *p = save_char;
        }
        p++;
    }
    // Créer le dernier niveau
    #ifdef _WIN32
        _mkdir(saves_dir);
    #else
        mkdir(saves_dir, 0755);
    #endif

    char nom_fichier[MAX_SAVE_FILENAME_LEN];
    char chemin_complet[PATH_MAX];
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

    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", saves_path, nom_fichier);

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
    fprintf(f, "  HISTORIQUE DES COUPS  \n");
    for (int i = 0; i < nb_coups; ++i) {
        fprintf(f, "Coup %d : %c joue (%d,%d)\n", i + 1, coups[i].joueur, coups[i].ligne + 1, coups[i].colonne + 1);
    }
    fprintf(f, "\n");

    // Plateau final
    fprintf(f, "  PLATEAU FINAL  \n");
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
    const char *saves_path = obtenir_chemin_saves();
    char chemin_complet[PATH_MAX];
    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", saves_path, nom_fichier);

    if (remove(chemin_complet) == 0)
        printf("Sauvegarde '%s' supprimée avec succès.\n", nom_fichier);
    else
        printf("Impossible de supprimer '%s'.\n", nom_fichier);
}
