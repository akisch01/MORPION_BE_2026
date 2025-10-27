/*
 * Fichier : save.h
 * Auteur  : Akpo Akisch
 * Date    : Octobre 2025
 * Description : Gestion des sauvegardes de parties (lecture, sélection, sauvegarde, suppression).
 */

#ifndef SAVE_H
#define SAVE_H

#include "board.h"  // ✅ Import de la structure Plateau

// Structure complète d'une partie
typedef struct {
    char joueur1[50];
    char joueur2[50];
    char symboleJ1;
    char symboleJ2;
    int tour;
    Plateau plateau;
} Partie;

// Fonctions principales
void charger_partie();
void sauvegarder_partie(const char *nom_fichier);
void supprimer_sauvegarde(const char *nom_fichier);

#endif
