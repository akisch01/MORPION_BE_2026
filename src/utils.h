#ifndef UTILS_H
#define UTILS_H
#include <stddef.h>

#define MAX_SAVE_FILENAME_LEN 256 // Max filename length + null terminator

void obtenir_date_actuelle(char *buffer, int taille);
const char *obtenir_chemin_saves();
int lire_entree_utilisateur(char *buffer, size_t taille);
#endif
