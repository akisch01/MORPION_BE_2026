/*
 * Fichier : game.c
 * Auteur  : Akpo Akisch
 * Description : Orchestration du jeu et liens entre modules
 */

#include "game.h"
#include "ui.h"
#include "ai.h"
#include "save.h"
#include "stats.h"
#include "help.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

// Par Jean-Yves
void nouvelle_partie(void) {
    Plateau plateau;
    Joueur j1, j2;
    char buffer[64];
    char rep;

    effacer_ecran();
    printf("=== NOUVELLE PARTIE ===\n\n");

    // Joueur 1
    printf("Nom du Joueur 1 : ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
    buffer[strcspn(buffer, "\n")] = '\0';
    if (buffer[0] == '\0') strncpy(buffer, "Joueur1", sizeof(buffer));

    // Choix du symbole
    char symbole_choisi = 'X';
    char tmpc;
    printf("Choisissez votre symbole (X/O) [X par défaut] : ");
    if (scanf(" %c", &tmpc) == 1) {
        while (getchar() != '\n');
        tmpc = toupper((unsigned char)tmpc);
        if (tmpc == 'X' || tmpc == 'O') symbole_choisi = tmpc;
    } else {
        while (getchar() != '\n');
    }

    initialiser_joueur(&j1, buffer, symbole_choisi, 0);

    // Joueur 2 (IA ou humain)
    int ia_level = 1;
    printf("Joueur 2 est-il une IA ? (o/n) : ");
    if (scanf(" %c", &rep) != 1) { while (getchar() != '\n'); rep = 'n'; }
    while (getchar() != '\n');

    char symbole_j2 = (symbole_choisi == 'X') ? 'O' : 'X';

    if (rep == 'o' || rep == 'O') {
        // Choix du niveau d'IA
        printf("Choisissez le niveau de l'IA (1-3) [1 par défaut] : ");
        if (scanf("%d", &ia_level) != 1) { ia_level = 1; }
        while (getchar() != '\n');
        if (ia_level < 1) ia_level = 1;
        if (ia_level > 3) ia_level = 3;
        initialiser_joueur(&j2, "IA", symbole_j2, 1);
    } else {
        printf("Nom du Joueur 2 : ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
        buffer[strcspn(buffer, "\n")] = '\0';
        if (buffer[0] == '\0') strncpy(buffer, "Joueur2", sizeof(buffer));
        initialiser_joueur(&j2, buffer, symbole_j2, 0);
    }

    // Initialiser plateau
    initialiser_plateau(&plateau);

    Joueur *courant = &j1;
    int tour = 1;
    int ligne, colonne;
    int coup_count = 0;
    Coup coups[MAX_COUPS];
    memset(coups, 0, sizeof(coups));

    while (1) {
        effacer_ecran();
        printf("Tour %d\n", tour);
        afficher_joueur(courant);
        afficher_plateau(&plateau);

        if (courant->est_IA) {
            int il = -1, ic = -1;
            ia_jouer_coup(ia_level, &plateau, &il, &ic);
            if (il >= 0 && ic >= 0) {
                jouer_coup(&plateau, il, ic, courant->symbole);
                // Enregistrer le coup
                if (coup_count < MAX_COUPS) {
                    coups[coup_count].ligne = il;
                    coups[coup_count].colonne = ic;
                    coups[coup_count].joueur = courant->symbole;
                    coup_count++;
                }
            }
            printf("IA joue...\n");
            pause_courte(600);
        } else {
            // Joueur humain
            int ok = 0;
            do {
                printf("Entrez la ligne (1-3) et la colonne (1-3), séparées par un espace : ");
                if (scanf("%d %d", &ligne, &colonne) != 2) {
                    while (getchar() != '\n');
                    printf("Entrée invalide. Réessayez.\n");
                    continue;
                }
                while (getchar() != '\n');
                if (jouer_coup(&plateau, ligne - 1, colonne - 1, courant->symbole)) {
                    ok = 1;
                    // Enregistrer le coup
                    if (coup_count < MAX_COUPS) {
                        coups[coup_count].ligne = ligne - 1;
                        coups[coup_count].colonne = colonne - 1;
                        coups[coup_count].joueur = courant->symbole;
                        coup_count++;
                    }
                } else {
                    printf("Coup invalide. Case occupée ou hors limite.\n");
                }
            } while (!ok);
        }

        // Vérifier victoire
        if (verifier_victoire(&plateau, courant->symbole)) {
            effacer_ecran();
            afficher_plateau(&plateau);
            printf("\n%s (%c) a gagné !\n", courant->nom, courant->symbole);
            mettre_a_jour_statistiques(courant->nom, 0);
            sauvegarder_partie(j1.nom, j2.nom, j1.symbole, j2.symbole, coup_count, coups, &plateau);
            break;
        }

        // Vérifier match nul
        if (verifier_match_nul(&plateau)) {
            effacer_ecran();
            afficher_plateau(&plateau);
            printf("\nMatch nul !\n");
            mettre_a_jour_statistiques("Match nul", 0);
            sauvegarder_partie(j1.nom, j2.nom, j1.symbole, j2.symbole, coup_count, coups, &plateau);
            break;
        }

        // Changer de joueur
        courant = (courant == &j1) ? &j2 : &j1;
        tour++;
    }

    printf("\nFin de la partie. Appuyez sur Entrée pour revenir au menu.\n");
    attendre_entree();
}

// Par Jean-Yves
void revisualiser_partie(void) {
    Partie partie;
    memset(&partie, 0, sizeof(Partie));

    DIR *dir = opendir("../data/saves/");
    if (!dir) {
        printf("Impossible d'accéder au dossier des sauvegardes.\n");
        attendre_entree();
        return;
    }

    struct dirent *entry;
    char fichiers[50][256];
    int nb_saves = 0;

    effacer_ecran();
    printf("\n=== REVISUALISER UNE PARTIE ===\n\n");

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (strstr(entry->d_name, ".txt")) {
            strcpy(fichiers[nb_saves], entry->d_name);
            nb_saves++;
        }
    }
    closedir(dir);

    if (nb_saves == 0) {
        printf("Aucune sauvegarde disponible.\n");
        attendre_entree();
        return;
    }

    printf("Parties sauvegardées :\n\n");
    for (int i = 0; i < nb_saves; ++i)
        printf(" %d. %s\n", i + 1, fichiers[i]);

    int choix = 0;
    printf("\nEntrez le numéro de la partie à revisualiser (0 pour annuler) : ");
    scanf("%d", &choix);
    while (getchar() != '\n');

    if (choix == 0 || choix < 1 || choix > nb_saves) {
        printf("Annulation.\n");
        attendre_entree();
        return;
    }

    char chemin_complet[512];
    snprintf(chemin_complet, sizeof(chemin_complet), "../data/saves/%s", fichiers[choix - 1]);
    FILE *f = fopen(chemin_complet, "r");
    if (!f) {
        printf("Erreur d'ouverture du fichier.\n");
        attendre_entree();
        return;
    }

    char ligne[256];
    char joueur1[50] = "", joueur2[50] = "";
    char symboleJ1 = ' ', symboleJ2 = ' ';
    int expected_coups = 0;    /* nombre indiqué dans l'en-tête */
    int parsed_coups = 0;      /* nombre réellement parsés */
    Coup coups[MAX_COUPS];
    Plateau plateau_replay;
    initialiser_plateau(&plateau_replay);
    
    /* Lecture du fichier */
    int in_historique = 0, in_plateau = 0;
    int plateau_row = 0;
    while (fgets(ligne, sizeof(ligne), f)) {
        /* Parse header */
        if (strstr(ligne, "# Joueur 1 :")) {
            sscanf(ligne, "# Joueur 1 : %[^ (] (%c)", joueur1, &symboleJ1);
            continue;
        } else if (strstr(ligne, "# Joueur 2 :")) {
            sscanf(ligne, "# Joueur 2 : %[^ (] (%c)", joueur2, &symboleJ2);
            continue;
        } else if (strstr(ligne, "# Nombre de coups :")) {
            sscanf(ligne, "# Nombre de coups : %d", &expected_coups);
            continue;
        } else if (strstr(ligne, "=== HISTORIQUE DES COUPS ===")) {
            in_historique = 1;
            in_plateau = 0;
            continue;
        } else if (strstr(ligne, "=== PLATEAU FINAL ===")) {
            in_historique = 0;
            in_plateau = 1;
            plateau_row = 0;
            continue;
        }
        
        /* Parse coups historique */
        if (in_historique && strstr(ligne, "Coup")) {
            int coup_num;
            char joueur;
            int lig, col;
            if (sscanf(ligne, "Coup %d : %c joue (%d,%d)", &coup_num, &joueur, &lig, &col) == 4) {
                if (parsed_coups < MAX_COUPS) {
                    coups[parsed_coups].ligne = lig - 1;  /* Convert 1-indexed to 0-indexed */
                    coups[parsed_coups].colonne = col - 1;
                    coups[parsed_coups].joueur = joueur;
                    parsed_coups++;
                }
            }
            continue;
        }
        
        /* Parse plateau final */
        if (in_plateau && ligne[0] != '#' && ligne[0] != '\n' && ligne[0] != '=') {
            int col_count = 0;
            for (int i = 0; i < (int)strlen(ligne) && col_count < 3; i++) {
                if (ligne[i] != ' ' && ligne[i] != '|' && ligne[i] != '-' && ligne[i] != '\n') {
                    plateau_replay.cases[plateau_row][col_count++] = ligne[i];
                }
            }
            if (col_count > 0) plateau_row++;
            if (plateau_row >= 3) plateau_row = 0;
            continue;
        }
    }
    fclose(f);

    int nb_coups = parsed_coups; /* nombre réel de coups à rejouer */

    // Replay avec pauses
    effacer_ecran();
    printf("=== REPLAY : %s ===\n\n", fichiers[choix - 1]);
    printf("Joueur 1 : %s (%c)\n", joueur1, symboleJ1);
    printf("Joueur 2 : %s (%c)\n\n", joueur2, symboleJ2);
    
    Plateau replay_board;
    initialiser_plateau(&replay_board);
    afficher_plateau(&replay_board);
    
    printf("\nAppuyez sur Entrée pour voir les coups étape par étape...\n");
    attendre_entree();
    
    for (int i = 0; i < nb_coups; i++) {
        effacer_ecran();
        printf("=== REPLAY : %s ===\n\n", fichiers[choix - 1]);
        printf("Joueur 1 : %s (%c)\n", joueur1, symboleJ1);
        printf("Joueur 2 : %s (%c)\n", joueur2, symboleJ2);
        printf("\nCoup %d : %c joue (%d,%d)\n\n", i + 1, coups[i].joueur, 
               coups[i].ligne + 1, coups[i].colonne + 1);
        
        replay_board.cases[coups[i].ligne][coups[i].colonne] = coups[i].joueur;
        afficher_plateau(&replay_board);
        
        printf("\nAppuyez sur Entrée pour le prochain coup (0 pour arrêter) : ");
        char input[10];
        fgets(input, sizeof(input), stdin);
        if (input[0] == '0') break;
    }
    
    effacer_ecran();
    printf("=== PLATEAU FINAL ===\n\n");
    printf("Joueur 1 : %s (%c)\n", joueur1, symboleJ1);
    printf("Joueur 2 : %s (%c)\n\n", joueur2, symboleJ2);
    afficher_plateau(&replay_board);
    printf("\nAppuyez sur Entrée pour revenir au menu.\n");
    attendre_entree();
}

void lancer_tournoi(void) {
    printf("[TODO] Mode tournoi (best of N) entre joueurs/IA.\n");
    attendre_entree();
}


void action_charger_partie() {
    charger_partie();   
    attendre_entree();
}

void action_afficher_aide() {
    afficher_aide();    
    attendre_entree();
}

void action_statistiques() {
    afficher_statistiques();
    attendre_entree();
}