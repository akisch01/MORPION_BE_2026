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
} PlayerStat;

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

    PlayerStat player_stats[MAX_PLAYERS];
    int num_players = 0;
    int match_nuls = 0;

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        char *gagnant = strstr(ligne, " a gagné le ");
        if (gagnant) {
            *gagnant = '\0'; // Terminate the string at " a gagné le " to get the player name
            char *nom_joueur = ligne;

            if (strcmp(nom_joueur, "Match nul") == 0) {
                match_nuls++;
            } else {
                int found = 0;
                for (int i = 0; i < num_players; i++) {
                    if (strcmp(player_stats[i].nom, nom_joueur) == 0) {
                        player_stats[i].victoires++;
                        found = 1;
                        break;
                    }
                }
                if (!found && num_players < MAX_PLAYERS) {
                    strncpy(player_stats[num_players].nom, nom_joueur, sizeof(player_stats[num_players].nom) - 1);
                    player_stats[num_players].nom[sizeof(player_stats[num_players].nom) - 1] = '\0';
                    player_stats[num_players].victoires = 1;
                    num_players++;
                }
            }
        }
    }
    fclose(fichier);

    for (int i = 0; i < num_players; i++) {
        printf("%s: %d victoire(s)\n", player_stats[i].nom, player_stats[i].victoires);
    }
    printf("Matchs nuls: %d\n", match_nuls);

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

    fprintf(f, "%s a gagné le %s\n", joueur_gagnant, date_str);
    fclose(f);
}
