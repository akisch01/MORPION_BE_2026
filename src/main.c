/*
 * Fichier : main.c
 * Auteur  : Akpo Akisch (Chef de projet)
 * Date    : Octobre 2025
 * Description : Point d'entrée du programme Morpion BE 2026
 */

#include <stdio.h>
#include "ui.h"
#include "game.h"

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
                char confirmation;
                do {
                    printf("\nVoulez-vous vraiment quitter ? (o/n) : ");
                    scanf(" %c", &confirmation);
                    while (getchar() != '\n'); // vider le buffer

                    if (confirmation == 'o' || confirmation == 'O') {
                        effacer_ecran();
                        printf("Merci d'avoir joué à MORPION BE 2026 !\n");
                        quitter = 1;
                        break;
                    } else if (confirmation == 'n' || confirmation == 'N') {
                        printf("\nRetour au menu principal...\n");
                        pause_courte(600);
                        break;
                    } else {
                        printf("Entrée invalide. Veuillez taper 'o' ou 'n'.\n");
                    }
                } while (confirmation != 'o' && confirmation != 'O' &&
                         confirmation != 'n' && confirmation != 'N');
                break;
            }
            default:
                printf("Choix invalide. Réessayez.\n");
                attendre_entree();
        }
    }

    return 0;
}
