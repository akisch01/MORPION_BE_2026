#include <stdio.h>
#include "ui.h"
#include "help.h"
#include "utils.h"

void afficher_regles() {
    effacer_ecran();
    printf("\n=== RÈGLES DU JEU ===\n\n");
    printf("Le Morpion est un jeu pour deux joueurs, X et O, qui placent à tour de rôle leur marque dans une grille de 3x3.\n");
    printf("Le premier joueur à aligner trois de ses marques horizontalement, verticalement ou en diagonale gagne la partie.\n");
    printf("Si la grille est remplie et qu'aucun joueur n'a gagné, la partie est déclarée nulle.\n\n");
    printf("Appuyez sur Entrée pour revenir au menu d'aide.\n");
    attendre_entree();
}

void afficher_commandes() {
    effacer_ecran();
    printf("\n=== COMMANDES DISPONIBLES ===\n\n");
    printf("Pendant une partie, pour jouer un coup, entrez les coordonnées de la case (ligne et colonne), séparées par un espace.\n");
    printf("Par exemple, '1 3' pour jouer dans la première ligne, troisième colonne.\n");
    printf("Pour quitter une partie en cours, entrez 'Q' ou 'q'.\n\n");
    printf("Appuyez sur Entrée pour revenir au menu d'aide.\n");
    attendre_entree();
}

void afficher_a_propos() {
    effacer_ecran();
    printf("\n=== À PROPOS ===\n\n");
    printf("Projet MORPION_BE_2026\n");
    printf("Réalisé par : Akpo Akisch, Arsène, Jean-Yves\n");
    printf("Dans le cadre du Bureau d'Études 2025/2026 de l'EILCO.\n\n");
    printf("Appuyez sur Entrée pour revenir au menu d'aide.\n");
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
                afficher_regles();
                break;
            case 2:
                afficher_commandes();
                break;
            case 3:
                afficher_a_propos();
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
