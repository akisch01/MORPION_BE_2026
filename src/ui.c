/*
 * Fichier : ui.c
 * Auteur  : Akpo Akisch
 * Description : Gestion de l'affichage console, couleurs et animations
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif
#include "ui.h"


#define RESET "\x1b[0m"
#define CYAN "\x1b[36m"
#define JAUNE "\x1b[33m"
#define VERT  "\x1b[32m"

void initialiser_ui(void) {}

void effacer_ecran(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause_courte(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
    usleep(ms * 1000);
    #pragma GCC diagnostic pop
#endif
}

void attendre_entree(void) {
    printf("\nAppuyez sur Entrée pour continuer...");
    while (getchar() != '\n');
}

void afficher_message_centre(const char *msg) {
    printf("%s\n", msg);
}

void afficher_chargement(void) {
    effacer_ecran();
    printf("%sChargement de MORPION BE 2026...%s\n", CYAN, RESET);
    char anim[] = {'|','/','-','\\'};
    for (int i = 0; i < 15; ++i) {
        printf(" [%c] ", anim[i % 4]);
        fflush(stdout);
        pause_courte(120);
        printf("\r");
    }
    effacer_ecran();
}

void afficher_logo(void) {
    printf("%s", JAUNE);
    printf("╔══════════════════════════════════╗\n");
    printf("║          M O R P I O N           ║\n");
    printf("║           B E  2 0 2 6           ║\n");
    printf("╚══════════════════════════════════╝\n");
    printf("%s", RESET);
}

void afficher_bienvenue(void) {
    printf("%sBienvenue dans le jeu du Morpion !%s\n\n", VERT, RESET);
    pause_courte(400);
}

void afficher_menu_principal(void) {
    effacer_ecran();
    afficher_logo();
    printf("\n");
    printf("1. Nouvelle partie\n");
    printf("2. Charger une partie\n");
    printf("3. Revisualiser une partie\n");
    printf("4. Tournoi (Best of N)\n");
    printf("5. Statistiques\n");
    printf("6. Aide\n");
    printf("7. Quitter\n\n");
    printf("Choisissez une option : ");
}

int choisir_option_menu(void) {
    int choix;
    if (scanf("%d", &choix) != 1) {
        while (getchar() != '\n');
        return -1;
    }
    while (getchar() != '\n');
    return choix;
}


