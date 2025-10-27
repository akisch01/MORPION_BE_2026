#ifndef UI_H
#define UI_H

void initialiser_ui(void);
void afficher_chargement(void);
void afficher_logo(void);
void afficher_bienvenue(void);
void effacer_ecran(void);
void pause_courte(int ms);
void attendre_entree(void);
void afficher_menu_principal(void);
int choisir_option_menu(void);
void afficher_message_centre(const char *message);

#endif
