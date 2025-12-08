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
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Par Jean-Yves
// Fonction interne pour jouer une seule partie
// Retourne 1 si j1 gagne, 2 si j2 gagne, 0 pour match nul
int jouer_une_partie_interne(Joueur *j1, Joueur *j2, int ia_level_j1, int ia_level_j2, int is_tournament) {
    Plateau plateau;
    initialiser_plateau(&plateau);

    Joueur *courant = j1;
    int tour = 1;
    int ligne, colonne;
    int coup_count = 0;
    int quitter = 0;
    Coup coups[MAX_COUPS];
    memset(coups, 0, sizeof(coups));

    // Déterminer le niveau d'IA pour chaque joueur
    int current_ia_level_j1 = (j1->est_IA) ? ia_level_j1 : -1;
    int current_ia_level_j2 = (j2->est_IA) ? ia_level_j2 : -1;


    while (1) {
        effacer_ecran();
        printf("Tour %d\n", tour);
        afficher_joueur(courant);
        afficher_plateau(&plateau);

        int current_ia_level = (courant == j1) ? current_ia_level_j1 : current_ia_level_j2;

        if (courant->est_IA) {
            int il = -1, ic = -1;
            ia_jouer_coup(current_ia_level, &plateau, &il, &ic);
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
            char input[64];
            do {
                printf("Entrez la ligne (1-3) et la colonne (1-3), séparées par un espace (ou Q pour quitter) : ");
                if (!fgets(input, sizeof(input), stdin)) {
                    printf("Entrée invalide. Réessayez.\n");
                    continue;
                }
                if (input[0] == 'Q' || input[0] == 'q') {
                    if (!is_tournament) { // Only offer to save if not in a tournament
                        printf("Voulez-vous enregistrer la partie en cours avant de quitter ? (O/N) : ");
                        if (!fgets(input, sizeof(input), stdin)) break;
                        if (input[0] == 'O' || input[0] == 'o') {
                            sauvegarder_partie(j1->nom, j2->nom, j1->symbole, j2->symbole,
                                            coup_count, coups, &plateau, "EN_COURS", NULL);
                        }
                    }
                    quitter = 1;
                    break;
                }

                if (sscanf(input, "%d %d", &ligne, &colonne) != 2) {
                    printf("Entrée invalide. Réessayez.\n");
                    continue;
                }

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
            } while (!ok && !quitter);

            if (quitter) break;
        }

        // Vérifier victoire
        if (verifier_victoire(&plateau, courant->symbole)) {
            effacer_ecran();
            afficher_plateau(&plateau);
            printf("\n%s (%c) a gagné !\n", courant->nom, courant->symbole);
            mettre_a_jour_statistiques(courant->nom, 0);
            if (!is_tournament) {
                const char *etat_victoire = (courant == j1) ? "VICTOIRE_J1" : "VICTOIRE_J2";
                sauvegarder_partie(j1->nom, j2->nom, j1->symbole, j2->symbole, coup_count, coups, &plateau, etat_victoire, NULL);
            }
            return (courant == j1) ? 1 : 2;
        }

        // Vérifier match nul
        if (verifier_match_nul(&plateau)) {
            effacer_ecran();
            afficher_plateau(&plateau);
            printf("\nMatch nul !\n");
            mettre_a_jour_statistiques("Match nul", 0);
            if (!is_tournament) {
                sauvegarder_partie(j1->nom, j2->nom, j1->symbole, j2->symbole, coup_count, coups, &plateau, "MATCH_NUL", NULL);
            }
            return 0;
        }

        // Changer de joueur
        courant = (courant == j1) ? j2 : j1;
        tour++;
    }
    return 0; // Should not be reached if game ends
}

void nouvelle_partie(void) {
    Joueur j1, j2;
    char buffer[64];
    char rep;
    int ia_level_j1 = -1; // Not used for human player
    int ia_level_j2 = -1;

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
    char buffer_symbole[16];
    if (lire_entree_utilisateur(buffer_symbole, sizeof(buffer_symbole)) && buffer_symbole[0] != '\0') {
        tmpc = toupper((unsigned char)buffer_symbole[0]);
        if (tmpc == 'X' || tmpc == 'O') symbole_choisi = tmpc;
    }

    initialiser_joueur(&j1, buffer, symbole_choisi, 0);

    // Joueur 2 (IA ou humain)
    printf("Joueur 2 est-il une IA ? (o/n) : ");
    char buffer_ia[16];
    lire_entree_utilisateur(buffer_ia, sizeof(buffer_ia));
    rep = (buffer_ia[0] == 'o' || buffer_ia[0] == 'O') ? 'o' : 'n';

    char symbole_j2 = (symbole_choisi == 'X') ? 'O' : 'X';

    if (rep == 'o' || rep == 'O') {
        // Choix du niveau d'IA
        printf("Choisissez le niveau de l'IA (1-3) [1 par défaut] : ");
        char buffer_level[16];
        lire_entree_utilisateur(buffer_level, sizeof(buffer_level));
        if (sscanf(buffer_level, "%d", &ia_level_j2) != 1) {
            ia_level_j2 = 1;
        }
        if (ia_level_j2 < 1) ia_level_j2 = 1;
        if (ia_level_j2 > 3) ia_level_j2 = 3;
        initialiser_joueur(&j2, "IA", symbole_j2, 1);
    } else {
        printf("Nom du Joueur 2 : ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
        buffer[strcspn(buffer, "\n")] = '\0';
        if (buffer[0] == '\0') strncpy(buffer, "Joueur2", sizeof(buffer));
        initialiser_joueur(&j2, buffer, symbole_j2, 0);
    }

    jouer_une_partie_interne(&j1, &j2, ia_level_j1, ia_level_j2, 0); // 0 for not in tournament

    printf("\nFin de la partie. Appuyez sur Entrée pour revenir au menu.\n");
    attendre_entree();
}

// Par Jean-Yves
void revisualiser_partie(void) {
    Partie partie;
    memset(&partie, 0, sizeof(Partie));

    char saves_path[PATH_MAX];
    obtenir_chemin_saves(saves_path, sizeof(saves_path));
    DIR *dir = opendir(saves_path);
    if (!dir) {
        printf("Impossible d'accéder au dossier des sauvegardes (%s).\n", saves_path);
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
    char buffer_choix[16];
    lire_entree_utilisateur(buffer_choix, sizeof(buffer_choix));
    if (sscanf(buffer_choix, "%d", &choix) != 1) {
        choix = -1; // Invalid input
    }

    if (choix == 0 || choix < 1 || choix > nb_saves) {
        printf("Annulation.\n");
        attendre_entree();
        return;
    }

    char chemin_complet[PATH_MAX];
    if (strlen(saves_path) + strlen(fichiers[choix - 1]) + 1 > PATH_MAX) {
        printf("Erreur: Le chemin du fichier de sauvegarde est trop long.\n");
        attendre_entree();
        return;
    }
    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", saves_path, fichiers[choix - 1]);
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
    
    printf("\nChoisissez le mode de replay :\n");
    printf("1. Manuel (vous appuyez sur Entrée à chaque étape)\n");
    printf("2. Automatique (1 seconde par coup)\n");
    printf("Votre choix [1] : ");
    char buffer_mode[16];
    lire_entree_utilisateur(buffer_mode, sizeof(buffer_mode));
    int mode_auto = 0;
    if (sscanf(buffer_mode, "%d", &mode_auto) == 1 && mode_auto == 2) {
        mode_auto = 1;
    } else {
        mode_auto = 0;
    }

    if (!mode_auto) {
        printf("\nAppuyez sur Entrée pour voir les coups étape par étape (ou 'q' pour quitter)...\n");
        attendre_entree();
    }
    
    for (int i = 0; i < nb_coups; i++) {
        effacer_ecran();
        printf("=== REPLAY : %s ===\n\n", fichiers[choix - 1]);
        printf("Joueur 1 : %s (%c)\n", joueur1, symboleJ1);
        printf("Joueur 2 : %s (%c)\n", joueur2, symboleJ2);
        printf("\nCoup %d : %c joue (%d,%d)\n\n", i + 1, coups[i].joueur, 
               coups[i].ligne + 1, coups[i].colonne + 1);
        
        replay_board.cases[coups[i].ligne][coups[i].colonne] = coups[i].joueur;
        afficher_plateau(&replay_board);
        
        if (mode_auto) {
            pause_courte(1000);
        } else {
            printf("\nAppuyez sur Entrée pour le prochain coup ('q' pour arrêter) : ");
            char input[10];
            lire_entree_utilisateur(input, sizeof(input));
            if (input[0] == 'q' || input[0] == 'Q') break;
        }
    }
    
    effacer_ecran();
    printf("=== PLATEAU FINAL ===\n\n");
    printf("Joueur 1 : %s (%c)\n", joueur1, symboleJ1);
    printf("Joueur 2 : %s (%c)\n\n", joueur2, symboleJ2);
    afficher_plateau(&replay_board);
    printf("\nAppuyez sur Entrée pour revenir au menu.\n");
    attendre_entree();
}

// (reprise function removed)

void lancer_tournoi(void) {
    effacer_ecran();
    printf("=== MODE TOURNOI (BEST OF N) ===\n\n");

    int nb_parties = 0;
    char buffer_nb_parties[16];
    do {
        printf("Entrez le nombre de parties pour le tournoi (N impair, ex: 3, 5) : ");
        lire_entree_utilisateur(buffer_nb_parties, sizeof(buffer_nb_parties));
        if (sscanf(buffer_nb_parties, "%d", &nb_parties) != 1 || nb_parties < 1 || nb_parties % 2 == 0) {
            printf("Nombre de parties invalide. Veuillez entrer un nombre impair et positif.\n");
        } else {
            break;
        }
    } while (1);

    Joueur j1, j2;
    char buffer[64];
    char rep;
    int ia_level_j1 = -1;
    int ia_level_j2 = -1;
    int mode_ia_j1 = 0; // 0 for human, 1 for IA
    int mode_ia_j2 = 0; // 0 for human, 1 for IA

    // Configuration Joueur 1
    printf("\nConfiguration Joueur 1 :\n");
    printf("Joueur 1 est-il une IA ? (o/n) : ");
    char buffer_ia1[16];
    lire_entree_utilisateur(buffer_ia1, sizeof(buffer_ia1));
    rep = (buffer_ia1[0] == 'o' || buffer_ia1[0] == 'O') ? 'o' : 'n';

    if (rep == 'o' || rep == 'O') {
        mode_ia_j1 = 1;
        printf("Choisissez le niveau de l'IA pour Joueur 1 (1-3) [1 par défaut] : ");
        char buffer_level1[16];
        lire_entree_utilisateur(buffer_level1, sizeof(buffer_level1));
        if (sscanf(buffer_level1, "%d", &ia_level_j1) != 1) {
            ia_level_j1 = 1;
        }
        if (ia_level_j1 < 1) ia_level_j1 = 1;
        if (ia_level_j1 > 3) ia_level_j1 = 3;
        initialiser_joueur(&j1, "IA_J1", 'X', 1); // Default symbol for IA_J1
    } else {
        printf("Nom du Joueur 1 : ");
        if (lire_entree_utilisateur(buffer, sizeof(buffer)) == 0) return;
        if (buffer[0] == '\0') strncpy(buffer, "Joueur1", sizeof(buffer));
        initialiser_joueur(&j1, buffer, 'X', 0); // Default symbol for human J1
    }

    // Configuration Joueur 2
    printf("\nConfiguration Joueur 2 :\n");
    printf("Joueur 2 est-il une IA ? (o/n) : ");
    char buffer_ia2[16];
    lire_entree_utilisateur(buffer_ia2, sizeof(buffer_ia2));
    rep = (buffer_ia2[0] == 'o' || buffer_ia2[0] == 'O') ? 'o' : 'n';

    if (rep == 'o' || rep == 'O') {
        mode_ia_j2 = 1;
        printf("Choisissez le niveau de l'IA pour Joueur 2 (1-3) [1 par défaut] : ");
        char buffer_level2[16];
        lire_entree_utilisateur(buffer_level2, sizeof(buffer_level2));
        if (sscanf(buffer_level2, "%d", &ia_level_j2) != 1) {
            ia_level_j2 = 1;
        }
        if (ia_level_j2 < 1) ia_level_j2 = 1;
        if (ia_level_j2 > 3) ia_level_j2 = 3;
        initialiser_joueur(&j2, "IA_J2", 'O', 1); // Default symbol for IA_J2
    } else {
        printf("Nom du Joueur 2 : ");
        if (lire_entree_utilisateur(buffer, sizeof(buffer)) == 0) return;
        if (buffer[0] == '\0') strncpy(buffer, "Joueur2", sizeof(buffer));
        initialiser_joueur(&j2, buffer, 'O', 0); // Default symbol for human J2
    }

    int score_j1 = 0;
    int score_j2 = 0;

    printf("\n=== DÉBUT DU TOURNOI ===\n");
    for (int i = 1; i <= nb_parties; ++i) {
        printf("\n--- PARTIE %d/%d ---\n", i, nb_parties);
        pause_courte(1000); // Pause before each game

        // To ensure symbols alternate each game for fairness, if players are human.
        // If AI, they can keep their symbols.
        // For simplicity, let's keep symbols fixed for now or randomly assign in the future.
        if (i % 2 == 1) { // J1 starts as X
            j1.symbole = 'X';
            j2.symbole = 'O';
        } else { // J2 starts as X
            j1.symbole = 'O';
            j2.symbole = 'X';
        }
        
        // Re-initialize players to reset internal state if any, but keep names/IA status
        if (mode_ia_j1) initialiser_joueur(&j1, j1.nom, j1.symbole, 1);
        else initialiser_joueur(&j1, j1.nom, j1.symbole, 0);

        if (mode_ia_j2) initialiser_joueur(&j2, j2.nom, j2.symbole, 1);
        else initialiser_joueur(&j2, j2.nom, j2.symbole, 0);


        int result = jouer_une_partie_interne(&j1, &j2, ia_level_j1, ia_level_j2, 1); // 1 for in tournament

        if (result == 1) {
            score_j1++;
            printf("%s gagne la partie %d!\n", j1.nom, i);
        } else if (result == 2) {
            score_j2++;
            printf("%s gagne la partie %d!\n", j2.nom, i);
        } else {
            printf("Match nul pour la partie %d!\n", i);
        }
        printf("Score actuel : %s %d - %s %d\n", j1.nom, score_j1, j2.nom, score_j2);
        attendre_entree(); // Pause after each game result
    }

    effacer_ecran();
    printf("=== RÉSULTATS DU TOURNOI ===\n");
    printf("Score final : %s %d - %s %d\n\n", j1.nom, score_j1, j2.nom, score_j2);

    if (score_j1 > score_j2) {
        printf("Félicitations, %s remporte le tournoi !\n", j1.nom);
        mettre_a_jour_statistiques(j1.nom, 0); // Update stats for tournament win
        printf("\n");
        printf("       /\\\n");
        printf("      /  \\\n");
        printf("     /____\\\n");
        printf("    |      |\n");
        printf("    |      |\n");
        printf("    |______| \n");
        printf("    /______\\\n");
        printf("\n");
        printf("    TROPHÉE !\n");
    } else if (score_j2 > score_j1) {
        printf("Félicitations, %s remporte le tournoi !\n", j2.nom);
        mettre_a_jour_statistiques(j2.nom, 0); // Update stats for tournament win
        printf("\n");
        printf("       /\\\n");
        printf("      /  \\\n");
        printf("     /____\\\n");
        printf("    |      |\n");
        printf("    |      |\n");
        printf("    |______| \n");
        printf("    /______\\\n");
        printf("\n");
        printf("    TROPHÉE !\n");
    } else {
        printf("Le tournoi se termine sur une égalité !\n");
    }

    printf("\nAppuyez sur Entrée pour revenir au menu principal.\n");
    attendre_entree();
}


// Reprendre une partie existante (Partie remplie par load_partie_from_file)
// Par Jean-Yves
void nouvelle_partie_reprise(const Partie *p) {
    if (!p) return;
    Plateau plateau = p->plateau;
    Joueur j1, j2;
    initialiser_joueur(&j1, p->joueur1, p->symboleJ1, 0);
    // Détecte si joueur2 est une IA par nom 'IA'
    int j2_is_ia = (strcmp(p->joueur2, "IA") == 0) ? 1 : 0;
    initialiser_joueur(&j2, p->joueur2, p->symboleJ2, j2_is_ia);

    // AI levels are not saved, so default to 1 if it's an IA for resumed game
    int ia_level_j1 = (j1.est_IA) ? 1 : -1;
    int ia_level_j2 = (j2.est_IA) ? 1 : -1;

    Coup coups[MAX_COUPS];
    int coup_count = 0;
    for (int i = 0; i < p->nb_coups && i < MAX_COUPS; ++i) coups[coup_count++] = p->coups[i];

    Joueur *courant = (coup_count % 2 == 0) ? &j1 : &j2;
    int tour = coup_count + 1;
    int quitter = 0;

    while (1) {
        effacer_ecran();
        printf("Tour %d\n", tour);
        afficher_joueur(courant);
        afficher_plateau(&plateau);

        int current_ia_level = (courant == &j1) ? ia_level_j1 : ia_level_j2;

        if (courant->est_IA) {
            int il = -1, ic = -1;
            ia_jouer_coup(current_ia_level, &plateau, &il, &ic);
            if (il >= 0 && ic >= 0) {
                jouer_coup(&plateau, il, ic, courant->symbole);
                if (coup_count < MAX_COUPS) { coups[coup_count].ligne = il; coups[coup_count].colonne = ic; coups[coup_count].joueur = courant->symbole; coup_count++; }
            }
            printf("IA joue...\n"); pause_courte(600);
        } else {
            char input[64]; int ligne, colonne; int ok = 0;
            do {
                printf("Entrez la ligne (1-3) et la colonne (1-3), séparées par un espace (ou Q pour quitter) : ");
                if (!fgets(input, sizeof(input), stdin)) { printf("Entrée invalide. Réessayez.\n"); continue; }
                if (input[0] == 'Q' || input[0] == 'q') {
                    printf("Voulez-vous enregistrer la partie en cours avant de quitter ? (O/N) : ");
                    if (!fgets(input, sizeof(input), stdin)) break;
                    if (input[0] == 'O' || input[0] == 'o') {
                        // écrase la sauvegarde existante
                        sauvegarder_partie(j1.nom, j2.nom, j1.symbole, j2.symbole, coup_count, coups, &plateau, "EN_COURS", p->nom_fichier);
                    }
                    quitter = 1; break;
                }
                if (sscanf(input, "%d %d", &ligne, &colonne) != 2) { printf("Entrée invalide. Réessayez.\n"); continue; }
                if (jouer_coup(&plateau, ligne - 1, colonne - 1, courant->symbole)) {
                    ok = 1; if (coup_count < MAX_COUPS) { coups[coup_count].ligne = ligne-1; coups[coup_count].colonne = colonne-1; coups[coup_count].joueur = courant->symbole; coup_count++; }
                } else { printf("Coup invalide. Case occupée ou hors limite.\n"); }
            } while (!ok && !quitter);
            if (quitter) break;
        }

        // Vérifier victoire
        if (verifier_victoire(&plateau, courant->symbole)) {
            effacer_ecran(); afficher_plateau(&plateau);
            printf("\n%s (%c) a gagné !\n", courant->nom, courant->symbole);
            mettre_a_jour_statistiques(courant->nom, 0);
            const char *etat_victoire = (courant == &j1) ? "VICTOIRE_J1" : "VICTOIRE_J2";
            sauvegarder_partie(j1.nom, j2.nom, j1.symbole, j2.symbole, coup_count, coups, &plateau, etat_victoire, p->nom_fichier);
            break;
        }

        if (verifier_match_nul(&plateau)) {
            effacer_ecran(); afficher_plateau(&plateau);
            printf("\nMatch nul !\n"); mettre_a_jour_statistiques("Match nul", 0);
            sauvegarder_partie(j1.nom, j2.nom, j1.symbole, j2.symbole, coup_count, coups, &plateau, "MATCH_NUL", p->nom_fichier);
            break;
        }

        courant = (courant == &j1) ? &j2 : &j1;
        tour++;
    }

    printf("\nFin de la partie reprise. Appuyez sur Entrée pour revenir au menu.\n");
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