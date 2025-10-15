
Structure du projet

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
