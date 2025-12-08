#ifndef UTILS_H
#define UTILS_H
#include <stddef.h>
void obtenir_date_actuelle(char *buffer, int taille);
void obtenir_chemin_saves(char *path, size_t size);
int lire_entree_utilisateur(char *buffer, size_t taille);
#endif
