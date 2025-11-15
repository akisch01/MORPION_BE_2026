/*
 * Fichier : board.c
 * Description : Gestion du plateau de jeu (affichage et logique basique)
 */

#include "board.h"
#include <stdio.h>

// Par Jean-Yves
void initialiser_plateau(Plateau *p) {
    for (int i = 0; i < TAILLE; ++i) {
        for (int j = 0; j < TAILLE; ++j) {
            p->cases[i][j] = ' ';
        }
    }
}

// Par Jean-Yves
void afficher_plateau(const Plateau *p) {
    printf("\n");
    for (int i = 0; i < TAILLE; ++i) {
        printf(" ");
        for (int j = 0; j < TAILLE; ++j) {
            printf(" %c ", p->cases[i][j]);
                if (j < TAILLE - 1) printf("|");
        }
        printf("\n");
        if (i < TAILLE - 1) {
            printf("---+---+---\n");
        }
    }
}

// Par Jean-Yves
int jouer_coup(Plateau *p, int ligne, int colonne, char symbole) {
    if (ligne < 0 || ligne >= TAILLE || colonne < 0 || colonne >= TAILLE)
        return 0;

    if (p->cases[ligne][colonne] != ' ')
        return 0;

    p->cases[ligne][colonne] = symbole;
    return 1;
}

// Par Jean-Yves
int verifier_victoire(const Plateau *p, char symbole) {
    // Lignes
    for (int i = 0; i < TAILLE; ++i) {
        int ok = 1;
        for (int j = 0; j < TAILLE; ++j)
            if (p->cases[i][j] != symbole) { ok = 0; break; }
        if (ok) return 1;
    }

    // Colonnes
    for (int j = 0; j < TAILLE; ++j) {
        int ok = 1;
        for (int i = 0; i < TAILLE; ++i)
            if (p->cases[i][j] != symbole) { ok = 0; break; }
        if (ok) return 1;
    }

    // Diagonales
    int ok = 1;
    for (int i = 0; i < TAILLE; ++i)
        if (p->cases[i][i] != symbole) { ok = 0; break; }
    if (ok) return 1;

    ok = 1;
    for (int i = 0; i < TAILLE; ++i)
        if (p->cases[i][TAILLE - 1 - i] != symbole) { ok = 0; break; }
    if (ok) return 1;

    return 0;
}

// Par Jean-Yves
int verifier_match_nul(const Plateau *p) {
    for (int i = 0; i < TAILLE; ++i)
        for (int j = 0; j < TAILLE; ++j)
            if (p->cases[i][j] == ' ') return 0;
    return 1;
}
