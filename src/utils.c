/*
 * Fichier : utils.c
 * Auteur  : Équipe du projet
 * Date    : Décembre 2025
 * Description : Fonctions utilitaires générales pour le projet, telles que 
 *               la gestion des dates, des chemins de fichiers et des entrées 
 *               utilisateur sécurisées.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#include "utils.h"

void obtenir_date_actuelle(char *buffer, int taille) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, taille, "%d/%m/%Y %H:%M", tm_info);
}

void obtenir_chemin_saves(char *path, size_t size) {
    snprintf(path, size, "data/saves/");
}

int lire_entree_utilisateur(char *buffer, size_t taille) {
    if (fgets(buffer, taille, stdin) != NULL) {
        // Enlever le newline
        buffer[strcspn(buffer, "\n")] = 0;
        // Si l'entrée était trop longue, vider le reste du buffer stdin
        if (strlen(buffer) == taille - 1 && buffer[taille - 2] != '\n') {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        return 1;
    }
    return 0;
}
