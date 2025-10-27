#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    char nom[50];
    char symbole;
    int est_IA;
} Joueur;

void initialiser_joueur(Joueur *j, const char *nom, char symbole, int ia);
void afficher_joueur(const Joueur *j);

#endif
