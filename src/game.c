/*
 * Fichier : game.c
 * Auteur  : Akpo Akisch / Jean-Yves
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
                // Demander la ligne
                printf("Entrez la ligne (1-3) ou 'q' pour quitter : ");
                if (lire_entree_utilisateur(input, sizeof(input)) == 0) {
                    quitter = 1; // Gérer la fin de l'entrée (Ctrl+D)
                    break;
                }

                if (input[0] == 'Q' || input[0] == 'q') {
                    if (!is_tournament) {
                        printf("Voulez-vous enregistrer la partie en cours avant de quitter ? (o/n) : ");
                        if (lire_entree_utilisateur(input, sizeof(input)) && (input[0] == 'O' || input[0] == 'o')) {
                            sauvegarder_partie(j1->nom, j2->nom, j1->symbole, j2->symbole,
                                            coup_count, coups, &plateau, "EN_COURS", NULL);
                        }
                    }
                    quitter = 1;
                    break;
                }

                if (sscanf(input, "%d", &ligne) != 1) {
                    printf("Entrée invalide. Veuillez entrer un nombre.\n");
                    continue;
                }

                // Demander la colonne
                printf("Entrez la colonne (1-3) : ");
                if (lire_entree_utilisateur(input, sizeof(input)) == 0) {
                    quitter = 1;
                    break;
                }

                if (sscanf(input, "%d", &colonne) != 1) {
                    printf("Entrée invalide. Veuillez entrer un nombre.\n");
                    continue;
                }

                if (jouer_coup(&plateau, ligne - 1, colonne - 1, courant->symbole)) {
                    ok = 1;
                    if (coup_count < MAX_COUPS) {
                        coups[coup_count].ligne = ligne - 1;
                        coups[coup_count].colonne = colonne - 1;
                        coups[coup_count].joueur = courant->symbole;
                        coup_count++;
                    }
                } else {
                    printf("Coup invalide. Case occupée ou hors limite. Réessayez.\n");
                }
            } while (!ok && !quitter);

            if (quitter) break;
        }

        // Vérifier victoire
        if (verifier_victoire(&plateau, courant->symbole)) {
            effacer_ecran();
            afficher_plateau(&plateau);
            printf("\nFélicitations, %s (%c) a gagné la partie !\n", courant->nom, courant->symbole);
            if (!is_tournament) {
                mettre_a_jour_statistiques(courant->nom, 0);
                const char *etat_victoire = (courant == j1) ? "VICTOIRE_J1" : "VICTOIRE_J2";
                sauvegarder_partie(j1->nom, j2->nom, j1->symbole, j2->symbole, coup_count, coups, &plateau, etat_victoire, NULL);
            }
            return (courant == j1) ? 1 : 2;
        }

        // Vérifier match nul
        if (verifier_match_nul(&plateau)) {
            effacer_ecran();
            afficher_plateau(&plateau);
            printf("\nMatch nul ! La grille est pleine.\n");
            if (!is_tournament) {
                mettre_a_jour_statistiques("Match nul", 0);
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
    int ia_level_j1 = -1; // Non utilisé pour un joueur humain
    int ia_level_j2 = -1;

    effacer_ecran();
    printf("  NOUVELLE PARTIE  \n\n");

    // Joueur 1
    printf("Entrez le nom du Joueur 1 (laissez vide pour 'Joueur1') : ");
    if (lire_entree_utilisateur(buffer, sizeof(buffer)) == 0) return;
    if (buffer[0] == '\0') strncpy(buffer, "Joueur1", sizeof(buffer));

    // Choix du symbole
    char symbole_choisi = 'X';
    printf("Joueur 1, choisissez votre symbole (X/O) [X par défaut] : ");
    char buffer_symbole[16];
    if (lire_entree_utilisateur(buffer_symbole, sizeof(buffer_symbole)) && buffer_symbole[0] != '\0') {
        char tmpc = toupper((unsigned char)buffer_symbole[0]);
        if (tmpc == 'X' || tmpc == 'O') symbole_choisi = tmpc;
    }

    initialiser_joueur(&j1, buffer, symbole_choisi, 0);
    printf("Joueur 1 initialisé : %s (%c)\n\n", j1.nom, j1.symbole);
    pause_courte(1000);

    // Joueur 2 (IA ou humain)
    printf("Le Joueur 2 est-il une IA ? (o/n) : ");
    char buffer_ia[16];
    lire_entree_utilisateur(buffer_ia, sizeof(buffer_ia));
    char rep = (buffer_ia[0] == 'o' || buffer_ia[0] == 'O') ? 'o' : 'n';

    char symbole_j2 = (symbole_choisi == 'X') ? 'O' : 'X';

    if (rep == 'o') {
        // Choix du niveau d'IA
        printf("Choisissez le niveau de l'IA \n1-Facile \n2-Normal \n3-Difficile \n[1] : ");
        char buffer_level[16];
        lire_entree_utilisateur(buffer_level, sizeof(buffer_level));
        if (sscanf(buffer_level, "%d", &ia_level_j2) != 1 || ia_level_j2 < 1 || ia_level_j2 > 3) {
            ia_level_j2 = 1;
        }
        initialiser_joueur(&j2, "IA", symbole_j2, 1);
        printf("Joueur 2 est une IA de niveau %d.\n\n", ia_level_j2);
    } else {
        printf("Entrez le nom du Joueur 2 (laissez vide pour 'Joueur2') : ");
        if (lire_entree_utilisateur(buffer, sizeof(buffer)) == 0) return;
        if (buffer[0] == '\0') strncpy(buffer, "Joueur2", sizeof(buffer));
        initialiser_joueur(&j2, buffer, symbole_j2, 0);
        printf("Joueur 2 initialisé : %s (%c)\n\n", j2.nom, j2.symbole);
    }
    
    pause_courte(1500);
    jouer_une_partie_interne(&j1, &j2, ia_level_j1, ia_level_j2, 0); // 0 = pas en tournoi

    printf("\nFin de la partie. Appuyez sur Entrée pour revenir au menu.\n");
    attendre_entree();
}

// Par Jean-Yves
void revisualiser_partie(void) {
    Partie partie;
    memset(&partie, 0, sizeof(Partie));

    const char *saves_path = obtenir_chemin_saves();
    DIR *dir = opendir(saves_path);
    if (!dir) {
        printf("Impossible d'accéder au dossier des sauvegardes (%s).\n", saves_path);
        attendre_entree();
        return;
    }

    struct dirent *entry;
    char fichiers[50][MAX_SAVE_FILENAME_LEN];
    int nb_saves = 0;

    effacer_ecran();
    printf("\n  REVOIR UNE PARTIE ARCHIVÉE  \n\n");

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
        printf("Aucune sauvegarde disponible pour le moment.\n");
        attendre_entree();
        return;
    }

    printf("Parties sauvegardées :\n\n");
    for (int i = 0; i < nb_saves; ++i)
        printf(" %d. %s\n", i + 1, fichiers[i]);

    int choix = 0;
    printf("\nEntrez le numéro de la partie à re-visualiser (0 pour annuler) : ");
    char buffer_choix[16];
    lire_entree_utilisateur(buffer_choix, sizeof(buffer_choix));
    if (sscanf(buffer_choix, "%d", &choix) != 1) {
        choix = -1; // Entrée invalide
    }

    if (choix <= 0 || choix > nb_saves) {
        printf("Annulation.\n");
        attendre_entree();
        return;
    }

    char chemin_complet[PATH_MAX];
    snprintf(chemin_complet, sizeof(chemin_complet), "%s%s", saves_path, fichiers[choix - 1]);
    FILE *f = fopen(chemin_complet, "r");
    if (!f) {
        printf("Erreur d'ouverture du fichier de sauvegarde.\n");
        attendre_entree();
        return;
    }

    char joueur1[50] = "", joueur2[50] = "";
    char symboleJ1 = ' ', symboleJ2 = ' ';
    int parsed_coups = 0;      /* nombre réellement parsés */
    Coup coups[MAX_COUPS];
    Plateau plateau_replay;
    initialiser_plateau(&plateau_replay);
    
    char ligne[256];
    int in_historique = 0;
    while (fgets(ligne, sizeof(ligne), f)) {
        if (strstr(ligne, "# Joueur 1 :")) {
            sscanf(ligne, "# Joueur 1 : %[^ (] (%c)", joueur1, &symboleJ1);
        } else if (strstr(ligne, "# Joueur 2 :")) {
            sscanf(ligne, "# Joueur 2 : %[^ (] (%c)", joueur2, &symboleJ2);
        } else if (strstr(ligne, "  HISTORIQUE DES COUPS  ")) {
            in_historique = 1;
        } else if (in_historique && strstr(ligne, "Coup")) {
            int coup_num, lig, col;
            char joueur_sym;
            if (sscanf(ligne, "Coup %d : %c joue (%d,%d)", &coup_num, &joueur_sym, &lig, &col) == 4) {
                if (parsed_coups < MAX_COUPS) {
                    coups[parsed_coups].ligne = lig - 1;
                    coups[parsed_coups].colonne = col - 1;
                    coups[parsed_coups].joueur = joueur_sym;
                    parsed_coups++;
                }
            }
        } else if (in_historique && strlen(ligne) < 2) { // Fin de la section historique
            in_historique = 0;
        }
    }
    
    fclose(f);

    int nb_coups = parsed_coups;

    effacer_ecran();
    printf("  REPLAY : %s  \n\n", fichiers[choix - 1]);
    printf("Joueur 1 : %s (%c)  |  Joueur 2 : %s (%c)\n", joueur1, symboleJ1, joueur2, symboleJ2);
    
    Plateau replay_board;
    initialiser_plateau(&replay_board);
    afficher_plateau(&replay_board);
    
    printf("\nChoisissez le mode de Replay :\n");
    printf("1. Manuel (vous appuyez sur Entrée à chaque étape)\n");
    printf("2. Automatique (1 seconde par coup)\n");
    printf("Votre choix [1] : ");
    char buffer_mode[16];
    lire_entree_utilisateur(buffer_mode, sizeof(buffer_mode));
    int mode_auto = (sscanf(buffer_mode, "%d", &mode_auto) == 1 && mode_auto == 2);

    if (!mode_auto) {
        printf("\nAppuyez sur Entrée pour avancer, ou 'q' pour quitter le replay...\n");
    }
    
    for (int i = 0; i < nb_coups; i++) {
        effacer_ecran();
        printf("  REPLAY : %s  \n\n", fichiers[choix - 1]);
        printf("Joueur 1 : %s (%c)  |  Joueur 2 : %s (%c)\n", joueur1, symboleJ1, joueur2, symboleJ2);
        printf("\nCoup %d/%d : Le joueur '%c' joue en (%d,%d)\n\n", i + 1, nb_coups, coups[i].joueur, 
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
    printf("  REPLAY TERMINÉ  \n\n");
    printf("Plateau final de la partie :\n");
    afficher_plateau(&replay_board);
    printf("\nAppuyez sur Entrée pour revenir au menu.\n");
    attendre_entree();
}

// (reprise function removed)
// Par akisch

void lancer_tournoi(void) {
    effacer_ecran();
    printf("  MODE TOURNOI (AU MEILLEUR DES N MANCHES)  \n\n");

    int nb_parties = 0;
    char buffer_nb_parties[16];
    do {
        printf("Entrez le nombre de manches (doit être impair, ex: 3, 5) : ");
        lire_entree_utilisateur(buffer_nb_parties, sizeof(buffer_nb_parties));
        if (sscanf(buffer_nb_parties, "%d", &nb_parties) != 1 || nb_parties < 1 || nb_parties % 2 == 0) {
            printf("Nombre invalide. Veuillez entrer un nombre impair et positif.\n");
        } else {
            break;
        }
    } while (1);

    Joueur j1, j2;
    char buffer[64];
    int ia_level_j1 = -1;
    int ia_level_j2 = -1;
    int mode_ia_j1 = 0;
    int mode_ia_j2 = 0;

    // Configuration Joueur 1
    printf("\n--- Configuration Joueur 1 ---\n");
    printf("Le Joueur 1 est-il une IA ? (o/n) : ");
    char buffer_ia1[16];
    lire_entree_utilisateur(buffer_ia1, sizeof(buffer_ia1));
    if (buffer_ia1[0] == 'o' || buffer_ia1[0] == 'O') {
        mode_ia_j1 = 1;
        printf("Niveau de l'IA pour Joueur 1 \n1-Facile \n2-Normal \n3-Difficile \n[1] : ");
        char buffer_level1[16];
        lire_entree_utilisateur(buffer_level1, sizeof(buffer_level1));
        if (sscanf(buffer_level1, "%d", &ia_level_j1) != 1 || ia_level_j1 < 1 || ia_level_j1 > 3) {
            ia_level_j1 = 1;
        }
        initialiser_joueur(&j1, "IA (J1)", 'X', 1);
    } else {
        printf("Nom du Joueur 1 (laissez vide pour 'Joueur1') : ");
        if (lire_entree_utilisateur(buffer, sizeof(buffer)) == 0) return;
        if (buffer[0] == '\0') strncpy(buffer, "Joueur1", sizeof(buffer));
        initialiser_joueur(&j1, buffer, 'X', 0);
    }

    // Configuration Joueur 2
    printf("\n--- Configuration Joueur 2 ---\n");
    printf("Le Joueur 2 est-il une IA ? (o/n) : ");
    char buffer_ia2[16];
    lire_entree_utilisateur(buffer_ia2, sizeof(buffer_ia2));
    if (buffer_ia2[0] == 'o' || buffer_ia2[0] == 'O') {
        mode_ia_j2 = 1;
        printf("Niveau de l'IA pour Joueur 2 \n1-Facile \n2-Normal \n3-Difficile \n[1] : ");
        char buffer_level2[16];
        lire_entree_utilisateur(buffer_level2, sizeof(buffer_level2));
        if (sscanf(buffer_level2, "%d", &ia_level_j2) != 1 || ia_level_j2 < 1 || ia_level_j2 > 3) {
            ia_level_j2 = 1;
        }
        initialiser_joueur(&j2, "IA (J2)", 'O', 1);
    } else {
        printf("Nom du Joueur 2 (laissez vide pour 'Joueur2') : ");
        if (lire_entree_utilisateur(buffer, sizeof(buffer)) == 0) return;
        if (buffer[0] == '\0') strncpy(buffer, "Joueur2", sizeof(buffer));
        initialiser_joueur(&j2, buffer, 'O', 0);
    }

    int score_j1 = 0;
    int score_j2 = 0;

    printf("\n\n  LE TOURNOI COMMENCE !  \n");
    pause_courte(1500);

    for (int i = 1; i <= nb_parties; ++i) {
        printf("\n--- MANCHE %d/%d ---\n", i, nb_parties);
        pause_courte(1000);

        if (i % 2 == 1) { j1.symbole = 'X'; j2.symbole = 'O'; } 
        else { j1.symbole = 'O'; j2.symbole = 'X'; }
        
        initialiser_joueur(&j1, j1.nom, j1.symbole, mode_ia_j1);
        initialiser_joueur(&j2, j2.nom, j2.symbole, mode_ia_j2);

        int result = jouer_une_partie_interne(&j1, &j2, ia_level_j1, ia_level_j2, 1);

        if (result == 1) {
            score_j1++;
            printf("\n%s remporte la manche !\n", j1.nom);
        } else if (result == 2) {
            score_j2++;
            printf("\n%s remporte la manche !\n", j2.nom);
        } else {
            printf("\nMatch nul pour cette manche !\n");
        }
        printf("Score : %s %d - %d %s\n", j1.nom, score_j1, score_j2, j2.nom);
        
        if (score_j1 > nb_parties / 2 || score_j2 > nb_parties / 2) {
            break; 
        }
        
        attendre_entree();
    }

    effacer_ecran();
    printf("  RÉSULTATS FINAUX DU TOURNOI  \n");
    printf("Score final : %s %d - %d %s\n\n", j1.nom, score_j1, score_j2, j2.nom);

    char gagnant_tournoi[50] = "";
    if (score_j1 > score_j2) {
        printf("Félicitations, %s remporte le tournoi !\n", j1.nom);
        strncpy(gagnant_tournoi, j1.nom, sizeof(gagnant_tournoi) - 1);
    } else if (score_j2 > score_j1) {
        printf("Félicitations, %s remporte le tournoi !\n", j2.nom);
        strncpy(gagnant_tournoi, j2.nom, sizeof(gagnant_tournoi) - 1);
    } else {
        printf("Le tournoi se termine sur une égalité !\n");
        strncpy(gagnant_tournoi, "Tournoi Nul", sizeof(gagnant_tournoi) - 1);
    }
    
    if (strlen(gagnant_tournoi) > 0) {
        mettre_a_jour_statistiques(gagnant_tournoi, 1); // 1 pour indiquer une victoire en tournoi
    }
    
    printf("\n");
    printf("      \\_/\n");
    printf("     (o.o)\n");
    printf("    (  _  )\n");
    printf("   /`>---<`\\\n");
    printf("   \\|     |/\n");
    printf("    `-----`\n\n");

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
    int j2_is_ia = (strcmp(p->joueur2, "IA") == 0);
    initialiser_joueur(&j2, p->joueur2, p->symboleJ2, j2_is_ia);

    int ia_level_j1 = (j1.est_IA) ? 1 : -1;
    int ia_level_j2 = (j2_is_ia) ? 1 : -1;

    Coup coups[MAX_COUPS];
    int coup_count = 0;
    for (int i = 0; i < p->nb_coups && i < MAX_COUPS; ++i) coups[coup_count++] = p->coups[i];

    Joueur *courant = (coup_count % 2 == 0) ? &j1 : &j2;
    int tour = coup_count + 1;
    int quitter = 0;

    while (1) {
        effacer_ecran();
        printf("--- Reprise du Tour %d ---\n", tour);
        afficher_joueur(courant);
        afficher_plateau(&plateau);

        int current_ia_level = (courant == &j1) ? ia_level_j1 : ia_level_j2;

        if (courant->est_IA) {
            printf("\nL'IA (%s) réfléchit...\n", courant->nom);
            pause_courte(800);
            int il = -1, ic = -1;
            ia_jouer_coup(current_ia_level, &plateau, &il, &ic);
            if (il >= 0 && ic >= 0) {
                jouer_coup(&plateau, il, ic, courant->symbole);
                if (coup_count < MAX_COUPS) { 
                    coups[coup_count] = (Coup){il, ic, courant->symbole};
                    coup_count++;
                }
            }
        } else {
            // Logique de jeu pour humain
            int ok = 0;
            char input[64];
            int ligne, colonne;
            do {
                printf("\nÀ votre tour, %s.\n", courant->nom);
                printf("Entrez la ligne (1-3) ou 'q' pour quitter : ");
                if (lire_entree_utilisateur(input, sizeof(input)) == 0) {
                    quitter = 1;
                    break;
                }

                if (input[0] == 'Q' || input[0] == 'q') {
                    printf("Voulez-vous enregistrer la partie avant de quitter ? (o/n) : ");
                    if (lire_entree_utilisateur(input, sizeof(input)) && (input[0] == 'O' || input[0] == 'o')) {
                        sauvegarder_partie(j1.nom, j2.nom, j1.symbole, j2.symbole, coup_count, coups, &plateau, "EN_COURS", p->nom_fichier);
                    }
                    quitter = 1;
                    break;
                }

                if (sscanf(input, "%d", &ligne) != 1) {
                    printf("Entrée invalide. Veuillez entrer un nombre entre 1 et 3.\n");
                    pause_courte(1000);
                    continue;
                }

                printf("Entrez la colonne (1-3) : ");
                if (lire_entree_utilisateur(input, sizeof(input)) == 0) {
                    quitter = 1;
                    break;
                }

                if (sscanf(input, "%d", &colonne) != 1) {
                    printf("Entrée invalide. Veuillez entrer un nombre entre 1 et 3.\n");
                    pause_courte(1000);
                    continue;
                }

                if (jouer_coup(&plateau, ligne - 1, colonne - 1, courant->symbole)) {
                    ok = 1;
                    if (coup_count < MAX_COUPS) {
                        coups[coup_count] = (Coup){ligne - 1, colonne - 1, courant->symbole};
                        coup_count++;
                    }
                } else {
                    printf("Coup invalide. La case est déjà occupée ou hors des limites.\n");
                    pause_courte(1000);
                }
            } while (!ok && !quitter);
            if (quitter) break;
        }

        if (verifier_victoire(&plateau, courant->symbole)) {
            effacer_ecran();
            afficher_plateau(&plateau);
            printf("\nFélicitations, %s (%c) a gagné la partie !\n", courant->nom, courant->symbole);
            mettre_a_jour_statistiques(courant->nom, 0);
            const char *etat_victoire = (courant == &j1) ? "VICTOIRE_J1" : "VICTOIRE_J2";
            sauvegarder_partie(j1.nom, j2.nom, j1.symbole, j2.symbole, coup_count, coups, &plateau, etat_victoire, p->nom_fichier);
            break;
        }

        if (verifier_match_nul(&plateau)) {
            effacer_ecran();
            afficher_plateau(&plateau);
            printf("\nMatch nul ! La grille est pleine.\n");
            mettre_a_jour_statistiques("Match nul", 0);
            sauvegarder_partie(j1.nom, j2.nom, j1.symbole, j2.symbole, coup_count, coups, &plateau, "MATCH_NUL", p->nom_fichier);
            break;
        }

        courant = (courant == &j1) ? &j2 : &j1;
        tour++;
    }

    printf("\nFin de la partie. Appuyez sur Entrée pour revenir au menu.\n");
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