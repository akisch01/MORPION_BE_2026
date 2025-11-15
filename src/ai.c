/*
 * Fichier : ai.c
 * Auteur  : Akpo Akisch
 * Date    : Novembre 2025
 * Description : IA simple (niveau 1 = random free cell)
 */

#include "ai.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>

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

// minimax evaluation
static int minimax(Plateau *p, int depth, int isMax, char ai_sym, char human_sym) {
    if (verifier_victoire(p, ai_sym)) return 10 - depth;
    if (verifier_victoire(p, human_sym)) return depth - 10;
    if (verifier_match_nul(p)) return 0;

    if (isMax) {
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
    } else {
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

void ia_jouer_coup(int niveau, const Plateau *p, int *ligne, int *colonne) {
    (void)niveau;  // utilisé dans les cas de switch ci-dessous
    if (!p || !ligne || !colonne) return;
    *ligne = -1; *colonne = -1;

    // Collect free cells
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

    static int seeded = 0;
    if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }

    char ai_sym = deduire_symbole_courant(p);
    char human_sym = (ai_sym == 'X') ? 'O' : 'X';

    if (niveau <= 1) {
        // random
        int idx = rand() % free_count;
        *ligne = frees[idx][0];
        *colonne = frees[idx][1];
        return;
    }

    // Niveau 2: heuristique simple
    if (niveau == 2) {
        // 1) gagner si possible
        for (int k = 0; k < free_count; ++k) {
            int r = frees[k][0], c = frees[k][1];
            Plateau tmp = *p;
            tmp.cases[r][c] = ai_sym;
            if (verifier_victoire(&tmp, ai_sym)) { *ligne = r; *colonne = c; return; }
        }
        // 2) bloquer adversaire
        for (int k = 0; k < free_count; ++k) {
            int r = frees[k][0], c = frees[k][1];
            Plateau tmp = *p;
            tmp.cases[r][c] = human_sym;
            if (verifier_victoire(&tmp, human_sym)) { *ligne = r; *colonne = c; return; }
        }
        // 3) centre
        if (p->cases[1][1] == ' ') { *ligne = 1; *colonne = 1; return; }
        // 4) coin
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
        // 5) random fallback
        int idx = rand() % free_count;
        *ligne = frees[idx][0]; *colonne = frees[idx][1];
        return;
    }

    // Niveau 3: Minimax (optimal)
    if (niveau >= 3) {
        int bestVal = INT_MIN;
        int bestR = frees[0][0], bestC = frees[0][1];
        Plateau tmp = *p;
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
