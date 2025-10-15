# Projet MORPION_BE_2026 (EILCO - Bureau d’Études 2025/2026)

##  Sujet
Développement complet du jeu MORPION (Tic-Tac-Toe) en langage C, dans un environnement console enrichi (interface ASCII, IA, sauvegardes et statistiques).

##  Équipe
- **Akpo Akisch** — Chef de projet & IA / Statistiques / Intégration
- **Arsène** — Interface console & Sauvegardes / Aide interactive
- **Jean-Yves** — Logique du jeu & Tests / Mode tournoi

##  Structure du projet
 Dans docs/Readme.txt

## Modules principaux
- main.c → menu principal et orchestration
- board.c / player.c → plateau et gestion des joueurs
- game.c → logique de partie et tournoi
- ui.c → interface console stylée et animations
- ai.c → intelligence artificielle (3 niveaux)
- save.c → sauvegarde et chargement de parties
- stats.c → statistiques avancées
- help.c → menu d’aide interactif
- utils.c → outils généraux (dates, fichiers, etc.)

## MENU PRINCIPAL (main.c)
### Options principales :
1. Nouvelle partie
2. Charger une partie (choisir parmi saves/)
3. Re-visualiser une partie (replay)
4. Tournoi (best of N)(Menu tournoi : “Best of N” (N odd, ex 3 ou 5).
Paramètres : joueur vs joueur, joueur vs IA (niveau choisi).
5. Stocker résultats de chaque manche, afficher score courant.
À la fin, afficher trophée ASCII, update stats globales.)
Statistiques
6. Aide (interactive)(AIDE :
-  Règles du jeu
- Commandes disponibles
-  À propos
-  Retour
)
7. Quitter
Comportement :
Boucle jusqu’à Quitter.
Après chaque action, retour au menu.

## RAPPORT (docs/REPORT.pdf) — structure et contenu à fournir
### 15 pages hors codes : suggestions précises pour sections.
1. Page de garde (titre, noms, tuteur)
2. Résumé (1/2 page)
3. Introduction (objectif, contraintes BE)
4. Analyse fonctionnelle (cas d’usage : joueur vs joueur, joueur vs IA, sauvegarde, replay, stats, tournoi)
5. Architecture logicielle (diagramme modules, responsabilités)
6. Détails modules (board, ai, save, stats, ui) avec pseudo-codes clés
7. Algorithme IA (détails minimax, heuristiques, complexité)
8. Formats de fichiers (exemples de save.txt, stats.txt)
9. Tests réalisés & résultats (tableau des tests)
10. Difficultés rencontrées & solutions
11. Améliorations possibles (ex : GUI SDL, réseau, matchmaking)
12. Conclusion
13. Annexes (extraits de codes commentés, instructions compilation, README)

## Répartition technique par dossier (alignée sur la structure officielle)
Dans docs/Readme.txt

## Répartition des rôles et responsabilités — Projet MORPION_BE_2026
| Membre                                                     | Rôle principal                                                | Missions techniques principales                                                                                                                                                                                                      | Livrables associés                                   |
| ---------------------------------------------------------- | ------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ---------------------------------------------------- |
| **Akpo Akisch** *(Chef de projet / Responsable technique)* | Coordination générale, intégration finale, IA et statistiques | - Organisation du dépôt et du code <br> - Implémentation IA niveaux 1–3 (ai.c / ai.h) <br> - Calcul des statistiques et affichage (stats.c / stats.h) <br> - Documentation technique et rapport final                                | `ai.c/h`, `stats.c/h`, `REPORT.pdf`, `README.txt`    |
| **Arsène** *(Responsable Interface & Sauvegarde)*          | Interface console stylée, narration, et gestion des fichiers  | - Design et effets visuels (ui.c / ui.h) <br> - Implémentation de la sauvegarde intelligente (save.c / save.h) <br> - Animation de victoire, chargement, trophée ASCII <br> - Aide interactive (help.c / help.h)                     | `ui.c/h`, `save.c/h`, `help.c/h`, `data/help.txt`    |
| **Jean-Yves** *(Responsable Logique de jeu & Tests)*       | Mécanique du plateau, gestion des joueurs et tests unitaires  | - Logique de partie, vérification des coups et vainqueur (board.c / game.c) <br> - Gestion des joueurs (player.c / player.h) <br> - Création du mode tournoi (best of N) <br> - Validation et tests finaux (unitaires + intégration) | `board.c/h`, `player.c/h`, `game.c/h`, jeux de tests |

## Plan de travail semaine par semaine (avec répartition par membre)
| Semaine          | Étape                                                   | Tâches principales                                                           | Responsable(s)              |
| ---------------- | ------------------------------------------------------- | ---------------------------------------------------------------------------- | --------------------------- |
| **42**           |  Présentation du sujet, plan & diagrammes             | Création du dépôt, README initial, diagramme des modules, partage des rôles  | Akisch, Arsène, Jean-Yves   |
| **45**           |  Menu principal + jeu 2 joueurs                       | `main.c`, `board.c`, `player.c`, `ui.c` (affichage basique)                  | Jean-Yves + Arsène          |
| **48**           |  IA niveau 1 & 2 + Sauvegarde                         | `ai.c`, `save.c`, `utils.c`, ajout au menu principal                         | Akisch + Arsène             |
| **51**           |  Statistiques + Aide interactive + Replay             | `stats.c`, `help.c`, `game.c` (historique), `ui.c` (améliorations visuelles) | Akisch + Arsène + Jean-Yves |
| **02 (Janvier)** |  IA niveau 3 (minimax) + Mode tournoi + Rapport final | `ai.c`, `game.c`, `docs/REPORT.pdf`, tests complets                          | Akisch + Jean-Yves          |
