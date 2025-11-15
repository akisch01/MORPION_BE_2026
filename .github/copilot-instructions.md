# MORPION_BE_2026 AI Coding Instructions

## Project Overview
This is a **Tic-Tac-Toe (Morpion) game** in C with a console UI, AI opponents (3 levels), game persistence, statistics, and tournament mode. It's a bureau d'études (design project) for EILCO engineering school (2025/2026).

## Architecture & Components

### Core Modules (each has `.c` and `.h` pair)
| Module | Purpose | Key Structures | Notes |
|--------|---------|-----------------|-------|
| `board` | 3×3 game board state & logic | `Plateau` (3×3 char array) | Handles move validation, win/draw detection |
| `player` | Player metadata | `Joueur` (name, symbol, is_ia flag) | Tracks AI vs human distinction |
| `game` | Game flow orchestration | — | Entry points: `nouvelle_partie()`, `charger_partie()`, `lancer_tournoi()` |
| `ai` | AI decision-making | — | Levels 1-3: random→heuristic→minimax |
| `save` | Game persistence | `Partie` struct (players, board, turn) | Saves to `saves/` directory as text files |
| `stats` | Match statistics | — | Reads/writes to `data/stats.txt` |
| `ui` | Console display & menus | — | Handles colors, animations, formatted output |
| `help` | Interactive help system | — | Displays rules, commands, about screen |
| `utils` | General utilities | — | File ops, date formatting, common helpers |

### Data Flow
```
main.c → afficher_menu_principal() → switch(choix) calls:
  ├─ nouvelle_partie() → game.c → board + player logic
  ├─ charger_partie() → save.c loads Partie → resume game
  ├─ lancer_tournoi() → game.c loop N rounds (best of N)
  ├─ afficher_statistiques() → stats.c reads data/stats.txt
  └─ afficher_aide() → help.c displays help.txt 
```

## Critical Patterns & Conventions

### Header Pattern (Important!)
Each module **must** follow this structure:
```c
/* Fichier : name.c
 * Auteur  : [Team member name]
 * Date    : [Month Year]
 * Description : [What it does]
 */
#include <required_headers>
#include "header_dependencies"
```
All files use **French naming** (e.g., `afficher_plateau`, `initialiser_joueur`).

### Constants
- `TAILLE = 3` (board size, defined in `board.h`)
- Board symbols: `'X'` and `'O'` (stored in `Joueur.symbole`)
- Empty cell: `' '` (space character)

### Struct Conventions
```c
// Board state (from board.h)
typedef struct { char cases[TAILLE][TAILLE]; } Plateau;

// Player (from player.h)
typedef struct {
    char nom[50];      // Player name
    char symbole;      // 'X' or 'O'
    int est_IA;        // 1 if AI, 0 if human
} Joueur;

// Game state (from save.h)
typedef struct {
    char joueur1[50], joueur2[50];
    char symboleJ1, symboleJ2;
    int tour;          // Turn counter
    Plateau plateau;
} Partie;
```

### Control Flow for Moves
1. `nouvelle_partie()` initializes `Plateau` with `initialiser_plateau()`
2. During turn: call `jouer_coup(plateau, ligne, colonne, symbole)` → returns 1 (valid) or 0 (invalid)
3. After move: check `verifier_victoire(plateau, symbole)` and `verifier_match_nul(plateau)`
4. AI move: `ia_jouer_coup(niveau)` returns row/col based on level (1=random, 2=heuristic, 3=minimax)
5. Save state: `sauvegarder_partie(nom_fichier)` persists `Partie` struct

## Build & Run

### Linux/macOS
```bash
cd src
gcc *.c -o ../build/morpion -Wall -Wextra -std=c11
../build/morpion
```

### Windows (PowerShell/CMD)
```bash
cd src
gcc *.c -o ../build/morpion.exe -Wall -Wextra -std=c11
../build/morpion.exe
```

## File Organization
- `src/` — All source files (headers + implementations)
- `build/` — Compiled binary (morpion or morpion.exe)
- `data/` — Runtime data files (help.txt, stats.txt)
- `docs/` — Documentation (Readme.txt, REPORT.pdf)
- `saves/` — Game save files (auto-created, plain text format)

## Team Responsibility Mapping
- **Akpo Akisch** (Lead): `ai.c/h`, `stats.c/h`, integration
- **Arsène**: `ui.c/h`, `save.c/h`, `help.c/h`
- **Jean-Yves**: `board.c/h`, `player.c/h`, `game.c/h`, testing

When modifying a module, check README.md team table before major changes.

## Common Tasks

### Adding a New Menu Option
1. Add case to `switch` in `main.c` (after `choisir_option_menu()`)
2. Create new game flow function in `game.c` (e.g., `void mon_option(void)`)
3. Update UI menu string in `ui.c:afficher_menu_principal()`

### Persisting Game State
- Use `Partie` struct from `save.h` — populate all fields (joueur1, joueur2, symboles, tour, plateau)
- Call `sauvegarder_partie("filename")` to write
- Load with `charger_partie()` which lists saves/ directory

### AI Integration
- AI works independently — `ia_jouer_coup(niveau)` returns a legal move
- Levels: 1=random, 2=heuristic, 3=minimax
- **Don't modify AI return type or signature** without updating all call sites in `game.c`

## Input/Output Notes
- User input uses `scanf()` with buffer flush: `while (getchar() != '\n');`
- Avoid `gets()` — not safe
- File I/O: plain text format for saves (one save per file in saves/)
- UI: `printf()` for output, `effacer_ecran()` for screen clears

## Known Constraints
- Fixed 3×3 board (TAILLE constant cannot change without breaking logic)
- No external libraries (pure C, standard library only)
- Console-only UI (no graphics, no SDL)
- Tournament mode limited to "best of N" (N must be odd)

## Documentation Artifacts
- **README.md** — Project overview, team roles, build instructions (this file)
- **docs/Readme.txt** — Technical architecture details
- **docs/REPORT.pdf** — Final engineering report (15+ pages)
- **Inline comments** — All files have headers with author, date, description

---

**Last Updated:** November 2025 | **For:** EILCO Bureau d'Études 2025/2026
