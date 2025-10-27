#ifndef BOARD_H
#define BOARD_H

#define TAILLE 3

typedef struct {
    char cases[TAILLE][TAILLE];
} Plateau;

void initialiser_plateau(Plateau *p);
void afficher_plateau(const Plateau *p);
int jouer_coup(Plateau *p, int ligne, int colonne, char symbole);
int verifier_victoire(const Plateau *p, char symbole);
int verifier_match_nul(const Plateau *p);

#endif
