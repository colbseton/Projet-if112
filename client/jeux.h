#ifndef _JEUX_H
#define _JEUX_H

#include "tools.h"

#define NB_MAX_PLAYERS 5

enum game_state { OVER, STARTED };

struct player {
    int score;
    char name[MAX_LEN];
};

struct game {
    struct player players[NB_MAX_PLAYERS];
    enum game_state state;
};

void menu(const struct board_t *board);

void init_game(struct game *game, int nb_players);
/* initialise la strucutre avec le nbr de joueurs, les scores à 0
   et leurs noms à "Joueurs i" */

#endif



