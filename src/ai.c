// Fichier : ai.c
// Auteur  : Akpo Akisch
// Date    : Novembre 2025
// Description : Implémentation des différents niveaux d'intelligence artificielle pour le jeu de Morpion.
//
//               Niveau 1: L'IA joue un coup complètement aléatoire sur une case libre.
//               Niveau 2: L'IA suit une heuristique simple :
//                         1. Gagner si un coup gagnant existe.
//                         2. Bloquer l'adversaire s'il est sur le point de gagner.
//                         3. Prendre la case du centre.
//                         4. Prendre une case d'angle aléatoire.
//                         5. Jouer un coup aléatoire.
//               Niveau 3: L'IA utilise l'algorithme Minimax pour trouver le coup optimal,
//                         la rendant imbattable.
//
//*************************************************************************************************

#include "ai.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// Déduit le symbole du joueur courant en comptant les 'X' et les 'O' sur le plateau.
// Le joueur qui a joué le moins de coups est le joueur courant.
static char deduire_symbole_courant(const Plateau *p) {
    int countX = 0, countO = 0;
    for (int i = 0; i < TAILLE; ++i) {
        for (int j = 0; j < TAILLE; ++j) {
            if (p->cases[i][j] == 'X') countX++;
            else if (p->cases[i][j] == 'O') countO++;
        }
    }
    return (countX <= countO) ? 'X' : 'O';
}

// Implémentation de l'algorithme Minimax pour évaluer le meilleur coup.
// - 'depth' représente la profondeur de la récursion.
// - 'isMax' est un booléen qui indique si on cherche à maximiser ou minimiser le score.
// - 'ai_sym' et 'human_sym' sont les symboles de l'IA et de l'humain.
//
// La fonction retourne un score :
// - +10 si l'IA gagne (moins la profondeur pour favoriser les victoires rapides).
// - -10 si l'humain gagne (plus la profondeur pour retarder les défaites).
// - 0 en cas de match nul.
static int minimax(Plateau *p, int depth, int isMax, char ai_sym, char human_sym) {
    if (verifier_victoire(p, ai_sym)) return 10 - depth;
    if (verifier_victoire(p, human_sym)) return depth - 10;
    if (verifier_match_nul(p)) return 0;

    if (isMax) { // Tour de l'IA (maximisation)
        int best = INT_MIN;
        for (int i = 0; i < TAILLE; ++i) {
            for (int j = 0; j < TAILLE; ++j) {
                if (p->cases[i][j] == ' ') {
                    p->cases[i][j] = ai_sym;
                    int val = minimax(p, depth + 1, 0, ai_sym, human_sym);
                    p->cases[i][j] = ' ';
                    if (val > best) best = val;
                }
            }
        }
        return best;
    } else { // Tour de l'humain (minimisation)
        int best = INT_MAX;
        for (int i = 0; i < TAILLE; ++i) {
            for (int j = 0; j < TAILLE; ++j) {
                if (p->cases[i][j] == ' ') {
                    p->cases[i][j] = human_sym;
                    int val = minimax(p, depth + 1, 1, ai_sym, human_sym);
                    p->cases[i][j] = ' ';
                    if (val < best) best = val;
                }
            }
        }
        return best;
    }
}

// Fonction principale de l'IA.
// Prend le niveau de difficulté et le plateau, et retourne le coup à jouer via les pointeurs 'ligne' et 'colonne'.
void ia_jouer_coup(int niveau, const Plateau *p, int *ligne, int *colonne) {
    (void)niveau;  // utilisé dans les cas de switch ci-dessous
    if (!p || !ligne || !colonne) return;
    *ligne = -1; *colonne = -1;

    // Récupérer la liste des cases libres
    int free_count = 0;
    int frees[TAILLE * TAILLE][2];
    for (int i = 0; i < TAILLE; ++i) {
        for (int j = 0; j < TAILLE; ++j) {
            if (p->cases[i][j] == ' ') {
                frees[free_count][0] = i;
                frees[free_count][1] = j;
                free_count++;
            }
        }
    }
    if (free_count == 0) return;

    // Initialiser le générateur de nombres aléatoires une seule fois
    static int seeded = 0;
    if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }

    char ai_sym = deduire_symbole_courant(p);
    char human_sym = (ai_sym == 'X') ? 'O' : 'X';

    // ==================
    // === NIVEAU 1 : Aléatoire
    // ==================
    if (niveau <= 1) {
        // Choisit une case libre au hasard.
        int idx = rand() % free_count;
        *ligne = frees[idx][0];
        *colonne = frees[idx][1];
        return;
    }

    // ==================
    // === NIVEAU 2 : Heuristique simple
    // ==================
    if (niveau == 2) {
        // 1. Cherche un coup gagnant.
        for (int k = 0; k < free_count; ++k) {
            int r = frees[k][0], c = frees[k][1];
            Plateau tmp = *p;
            tmp.cases[r][c] = ai_sym;
            if (verifier_victoire(&tmp, ai_sym)) { *ligne = r; *colonne = c; return; }
        }
        // 2. Cherche à bloquer l'adversaire s'il est sur le point de gagner.
        for (int k = 0; k < free_count; ++k) {
            int r = frees[k][0], c = frees[k][1];
            Plateau tmp = *p;
            tmp.cases[r][c] = human_sym;
            if (verifier_victoire(&tmp, human_sym)) { *ligne = r; *colonne = c; return; }
        }
        // 3. Prend la case du centre si elle est libre.
        if (p->cases[1][1] == ' ') { *ligne = 1; *colonne = 1; return; }
        
        // 4. Prend un coin aléatoire s'il est libre.
        int corners[4][2] = {{0,0},{0,2},{2,0},{2,2}};
        int corner_free[4], cf = 0;
        for (int i = 0; i < 4; ++i) {
            int r = corners[i][0], c = corners[i][1];
            if (p->cases[r][c] == ' ') { corner_free[cf++] = i; }
        }
        if (cf > 0) {
            int idx = corner_free[rand() % cf];
            *ligne = corners[idx][0]; *colonne = corners[idx][1]; return;
        }
        // 5. En dernier recours, joue un coup aléatoire.
        int idx = rand() % free_count;
        *ligne = frees[idx][0]; *colonne = frees[idx][1];
        return;
    }

    // ==================
    // === NIVEAU 3 : Minimax (imbattable)
    // ==================
    if (niveau >= 3) {
        int bestVal = INT_MIN;
        int bestR = frees[0][0], bestC = frees[0][1];
        Plateau tmp = *p;
        // Itère sur tous les coups possibles et choisit celui avec le score minimax le plus élevé.
        for (int k = 0; k < free_count; ++k) {
            int r = frees[k][0], c = frees[k][1];
            tmp = *p;
            tmp.cases[r][c] = ai_sym;
            int moveVal = minimax(&tmp, 0, 0, ai_sym, human_sym);
            if (moveVal > bestVal) { bestVal = moveVal; bestR = r; bestC = c; }
        }
        *ligne = bestR; *colonne = bestC; return;
    }
}
