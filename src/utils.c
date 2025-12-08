#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#include "utils.h"

void obtenir_date_actuelle(char *buffer, int taille) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, taille, "%d/%m/%Y %H:%M", tm_info);
}

void obtenir_chemin_saves(char *path, size_t size) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        // Si on est dans src/, remonter d'un niveau
        if (strstr(cwd, "/src") != NULL) {
            snprintf(path, size, "%s/../data/saves/", cwd);
        } else {
            snprintf(path, size, "%s/data/saves/", cwd);
        }
    } else {
        // Fallback sur chemin relatif
        strncpy(path, "../data/saves/", size);
    }
}
