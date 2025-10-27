#include <stdio.h>
#include <time.h>
#include "utils.h"

void obtenir_date_actuelle(char *buffer, int taille) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, taille, "%d/%m/%Y %H:%M", tm_info);
}
