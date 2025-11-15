#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    char nom[50]; // Pseudo
    char symbole; // Le pion préféré qu'il veut
    int est_IA; // Pour savoir s'il va jouer contre une IA ou pas 
} Joueur;

void initialiser_joueur(Joueur *j, const char *nom, char symbole, int ia);
void afficher_joueur(const Joueur *j);

#endif

