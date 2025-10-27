/*
 * Fichier : stats.h
 * Auteur  : Akpo Akisch
 * Date    : Octobre 2025
 * Description : Gestion et affichage des statistiques de jeu.
 */

#ifndef STATS_H
#define STATS_H

// Affiche les statistiques à partir du fichier stats.txt
void afficher_statistiques();

// Met à jour les statistiques après une partie (à implémenter plus tard)
void mettre_a_jour_statistiques(const char *joueur_gagnant, int duree);

#endif
