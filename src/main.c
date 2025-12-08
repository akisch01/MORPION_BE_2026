/*
 * Fichier : main.c
 * Auteur  : Akpo Akisch
 * Date    : Octobre 2025
 * Description : Point d'entrée du programme Morpion BE 2026
 */

#include <stdio.h>
#include "ui.h"
#include "game.h"
#include "utils.h"

#include <stdio.h>
#include "ui.h"
#include "game.h"
#include "utils.h"

int main(void) {
    initialiser_ui();
    afficher_chargement();
    afficher_logo();
    afficher_bienvenue();

    int quitter = 0;

    while (!quitter) {
        afficher_menu_principal();
        int choix = choisir_option_menu();

        switch (choix) {
            case 1:
                nouvelle_partie();
                break;
            case 2:
                charger_partie();
                break;
            case 3:
                revisualiser_partie();
                break;
            case 4:
                lancer_tournoi();
                break;
            case 5:
                afficher_statistiques();
                break;
            case 6:
                afficher_aide();
                break;
            case 7: {
                char confirmation = ' ';
                printf("\nÊtes-vous sûr de vouloir quitter ? (o/n) : ");
                char buffer[16];
                if (lire_entree_utilisateur(buffer, sizeof(buffer)) && buffer[0] != '\0') {
                    confirmation = buffer[0];
                }

                if (confirmation == 'o' || confirmation == 'O') {
                    effacer_ecran();
                    printf("Merci d'avoir joué à MORPION BE 2026 ! À bientôt !\n");
                    quitter = 1;
                } else {
                    printf("\nRetour au menu principal...\n");
                    pause_courte(600);
                }
                break;
            }
            default:
                printf("\nOption invalide. Veuillez choisir une option entre 1 et 7.\n");
                attendre_entree();
        }
    }

    return 0;
}
