#ifndef _JEUX_H
#define _JEUX_H

#define LIMIT 5
#define NB_MAX_PLAYERS 5
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

#define MAX_LEN 5000

enum game_state { OVER, STARTED };

struct player {
    int score;
    char name[MAX_LEN];
};

struct game {
    struct player players[NB_MAX_PLAYERS];
    enum game_state state;
    int nb_players;
};

void menu(const struct board_t *board);

int levenshtein(char *s1, char *s2);

void init_game(struct game *game, int nb_players);
/* initialise la strucutre avec le nbr de joueurs, les scores à 0
   et leurs noms à "Joueurs i" */

#endif



