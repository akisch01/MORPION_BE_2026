/*
 * Fichier : stats.h
 * Auteur  : Akpo Akisch
 * Date    : Decembre 2025
 * Description : Gestion et affichage des statistiques de jeu.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ui.h"
#include "stats.h"

#define MAX_PLAYERS 100

typedef struct {
    char nom[50];
    int victoires;
    int tournois;
} PlayerStat;

// Affiche les statistiques actuelles depuis stats.txt
void afficher_statistiques() {
    FILE *fichier = fopen("data/stats.txt", "r");
    if (!fichier) {
        printf("Impossible de charger le fichier de statistiques (stats.txt).\n");
        attendre_entree();
        return;
    }

    effacer_ecran();
    printf("\n\033[1;34m=== STATISTIQUES DU JEU ===\033[0m\n\n");

    PlayerStat player_stats[MAX_PLAYERS];
    int num_players = 0;
    int match_nuls = 0;

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        char *gagnant = strstr(ligne, " a gagné le ");
        if (gagnant) {
            *gagnant = '\0'; 
            char *nom_joueur = ligne;
            
            int is_tournoi = (strstr(nom_joueur, "(Tournoi)") != NULL);
            if (is_tournoi) {
                char *tournoi_marker = strstr(nom_joueur, " (Tournoi)");
                if (tournoi_marker) {
                    *tournoi_marker = '\0';
                }
            }
            
            if (strcmp(nom_joueur, "Match nul") == 0) {
                match_nuls++;
            } else {
                int found = 0;
                for (int i = 0; i < num_players; i++) {
                    if (strcmp(player_stats[i].nom, nom_joueur) == 0) {
                        if (is_tournoi) {
                            player_stats[i].tournois++;
                        } else {
                            player_stats[i].victoires++;
                        }
                        found = 1;
                        break;
                    }
                }
                if (!found && num_players < MAX_PLAYERS) {
                    strncpy(player_stats[num_players].nom, nom_joueur, sizeof(player_stats[num_players].nom) - 1);
                    player_stats[num_players].nom[sizeof(player_stats[num_players].nom) - 1] = '\0';
                    player_stats[num_players].victoires = is_tournoi ? 0 : 1;
                    player_stats[num_players].tournois = is_tournoi ? 1 : 0;
                    num_players++;
                }
            }
        }
    }
    fclose(fichier);

    if (num_players == 0 && match_nuls == 0) {
        printf("Aucune statistique à afficher pour le moment.\n");
    } else {
        printf("\033[1;36m┌───────────────────────────┬────────────┬───────────┐\033[0m\n");
        printf("\033[1;36m│          JOUEUR           │ VICTOIRES  │ TOURNOIS  │\033[0m\n");
        printf("\033[1;36m├───────────────────────────┼────────────┼───────────┤\033[0m\n");

        for (int i = 0; i < num_players; i++) {
            printf("\033[32m│ %-25s │ %-10d │ %-9d │\033[0m\n", player_stats[i].nom, player_stats[i].victoires, player_stats[i].tournois);
        }
        
        printf("\033[1;36m└───────────────────────────┴────────────┴───────────┘\033[0m\n\n");
        
        if (match_nuls > 0) {
            printf("\033[33mMatchs nuls : %d\033[0m\n", match_nuls);
        }
    }

    printf("\n\033[36mAppuyez sur Entrée pour revenir au menu principal.\033[0m\n");
    attendre_entree();
}

// Mise à jour automatique après chaque partie
// duree = 0 pour une partie normale, 1 pour une victoire de tournoi
void mettre_a_jour_statistiques(const char *joueur_gagnant, int is_tournoi) {
    if (!joueur_gagnant) return;

    FILE *f = fopen("data/stats.txt", "a");
    if (!f) {
        printf("Impossible d'ouvrir stats.txt pour écriture.\n");
        return;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date_str[64];
    strftime(date_str, sizeof(date_str), "%d/%m/%Y %H:%M", tm_info);

    if (is_tournoi) {
        fprintf(f, "%s (Tournoi) a gagné le %s\n", joueur_gagnant, date_str);
    } else {
        fprintf(f, "%s a gagné le %s\n", joueur_gagnant, date_str);
    }
    
    fclose(f);
}
