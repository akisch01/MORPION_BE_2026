# Projet MORPION_BE_2026 (EILCO - Bureau d’Études 2025/2026)

##  Sujet
Développement complet du jeu MORPION (Tic-Tac-Toe) en langage C, dans un environnement console enrichi (interface ASCII, IA, sauvegardes et statistiques).

##  Équipe
- **Akpo Akisch** — Chef de projet & IA / Statistiques / Intégration
- **Arsène** — Interface console & Sauvegardes / Aide interactive
- **Jean-Yves** — Logique du jeu & Tests / Mode tournoi

##  Structure du projet
/MORPION_BE_2026/
│
├── src/
│   ├── main.c           // menu & orchestration
│   ├── game.c / game.h  // logique de parties, history, tournoi
│   ├── board.c / board.h// affichage & manip plateau
│   ├── player.c / player.h // gestion joueurs (noms, symboles)
│   ├── ai.c / ai.h      // IA niveaux 1-3
│   ├── save.c / save.h  // sauvegarde multiple / reprise / replay
│   ├── stats.c / stats.h// lecture/ecriture + calculs avancés
│   ├── help.c / help.h  // menu d'aide interactif
│   ├── ui.c / ui.h      // affichage stylé, animations, messages narratifs
│   ├── utils.c / utils.h// utilitaires (date/time, file helpers)
│
├── data/
│   ├── saves/           // contient partie1.txt, partie2.txt, ...
│   ├── replays/         // history files for replay (json-like or simple)
│   ├── stats.txt
│   ├── help.txt
│
├── build/
│   └── morpion.exe (ou ./morpion)
│
├── docs/
│   ├── REPORT.pdf
│   └── README.txt

Modules principaux
main.c → menu principal et orchestration
board.c / player.c → plateau et gestion des joueurs
game.c → logique de partie et tournoi
ui.c → interface console stylée et animations
ai.c → intelligence artificielle (3 niveaux)
save.c → sauvegarde et chargement de parties
stats.c → statistiques avancées
help.c → menu d’aide interactif
utils.c → outils généraux (dates, fichiers, etc.)

MENU PRINCIPAL (main.c)
Options principales :
Nouvelle partie
Charger une partie (choisir parmi saves/)
Re-visualiser une partie (replay)
Tournoi (best of N)(Menu tournoi : “Best of N” (N odd, ex 3 ou 5).
Paramètres : joueur vs joueur, joueur vs IA (niveau choisi).
Stocker résultats de chaque manche, afficher score courant.
À la fin, afficher trophée ASCII, update stats globales.)
Statistiques
Aide (interactive)(AIDE :
1. Règles du jeu
2. Commandes disponibles
3. À propos
4. Retour
)
Quitter
Comportement :
Boucle jusqu’à Quitter.
Après chaque action, retour au menu.

RAPPORT (docs/REPORT.pdf) — structure et contenu à fournir
15 pages hors codes : suggestions précises pour sections.
Page de garde (titre, noms, tuteur)
Résumé (1/2 page)
Introduction (objectif, contraintes BE)
Analyse fonctionnelle (cas d’usage : joueur vs joueur, joueur vs IA, sauvegarde, replay, stats, tournoi)
Architecture logicielle (diagramme modules, responsabilités)
Détails modules (board, ai, save, stats, ui) avec pseudo-codes clés
Algorithme IA (détails minimax, heuristiques, complexité)
Formats de fichiers (exemples de save.txt, stats.txt)
Tests réalisés & résultats (tableau des tests)
Difficultés rencontrées & solutions
Améliorations possibles (ex : GUI SDL, réseau, matchmaking)
Conclusion
Annexes (extraits de codes commentés, instructions compilation, README)

Répartition technique par dossier (alignée sur la structure officielle)
/MORPION_BE_2026/
│
├── src/
│   ├── main.c               → (Akisch) orchestration, intégration
│   ├── game.c / game.h      → (Jean-Yves)
│   ├── board.c / board.h    → (Jean-Yves)
│   ├── player.c / player.h  → (Jean-Yves)
│   ├── ai.c / ai.h          → (Akisch)
│   ├── save.c / save.h      → (Arsène)
│   ├── stats.c / stats.h    → (Akisch)
│   ├── help.c / help.h      → (Arsène)
│   ├── ui.c / ui.h          → (Arsène)
│   ├── utils.c / utils.h    → (partagé entre tous pour fonctions génériques)
│
├── data/
│   ├── saves/               → (Arsène)
│   ├── replays/             → (Jean-Yves)
│   ├── stats.txt            → (Akisch)
│   ├── help.txt             → (Arsène)
│
├── build/
│   └── morpion.exe          → (Akisch – intégration et build final)
│
├── docs/
│   ├── REPORT.pdf           → (Akisch – finalisation et mise en forme)
│   └── README.txt           → (Akisch)


Répartition des rôles et responsabilités — Projet MORPION_BE_2026
| Membre                                                     | Rôle principal                                                | Missions techniques principales                                                                                                                                                                                                      | Livrables associés                                   |
| ---------------------------------------------------------- | ------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ---------------------------------------------------- |
| **Akpo Akisch** *(Chef de projet / Responsable technique)* | Coordination générale, intégration finale, IA et statistiques | - Organisation du dépôt et du code <br> - Implémentation IA niveaux 1–3 (ai.c / ai.h) <br> - Calcul des statistiques et affichage (stats.c / stats.h) <br> - Documentation technique et rapport final                                | `ai.c/h`, `stats.c/h`, `REPORT.pdf`, `README.txt`    |
| **Arsène** *(Responsable Interface & Sauvegarde)*          | Interface console stylée, narration, et gestion des fichiers  | - Design et effets visuels (ui.c / ui.h) <br> - Implémentation de la sauvegarde intelligente (save.c / save.h) <br> - Animation de victoire, chargement, trophée ASCII <br> - Aide interactive (help.c / help.h)                     | `ui.c/h`, `save.c/h`, `help.c/h`, `data/help.txt`    |
| **Jean-Yves** *(Responsable Logique de jeu & Tests)*       | Mécanique du plateau, gestion des joueurs et tests unitaires  | - Logique de partie, vérification des coups et vainqueur (board.c / game.c) <br> - Gestion des joueurs (player.c / player.h) <br> - Création du mode tournoi (best of N) <br> - Validation et tests finaux (unitaires + intégration) | `board.c/h`, `player.c/h`, `game.c/h`, jeux de tests |

Plan de travail semaine par semaine (avec répartition par membre)
| Semaine          | Étape                                                   | Tâches principales                                                           | Responsable(s)              |
| ---------------- | ------------------------------------------------------- | ---------------------------------------------------------------------------- | --------------------------- |
| **42**           |  Présentation du sujet, plan & diagrammes             | Création du dépôt, README initial, diagramme des modules, partage des rôles  | Akisch, Arsène, Jean-Yves   |
| **45**           |  Menu principal + jeu 2 joueurs                       | `main.c`, `board.c`, `player.c`, `ui.c` (affichage basique)                  | Jean-Yves + Arsène          |
| **48**           |  IA niveau 1 & 2 + Sauvegarde                         | `ai.c`, `save.c`, `utils.c`, ajout au menu principal                         | Akisch + Arsène             |
| **51**           |  Statistiques + Aide interactive + Replay             | `stats.c`, `help.c`, `game.c` (historique), `ui.c` (améliorations visuelles) | Akisch + Arsène + Jean-Yves |
| **02 (Janvier)** |  IA niveau 3 (minimax) + Mode tournoi + Rapport final | `ai.c`, `game.c`, `docs/REPORT.pdf`, tests complets                          | Akisch + Jean-Yves          |
