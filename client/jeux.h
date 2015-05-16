#ifndef _JEUX_H
#define _JEUX_H

#define NB_MAX_PLAYERS  5
#define MAX_LEN         5000

#define DUO             '1'
#define CARRE           '2'
#define CASH            '3'

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

void jeux_cinq_pour_tous(const struct board_t *board, struct game *m_game);

void jeu_max_de_questions(const struct board_t *board, struct game *m_game);

void init_game(struct game *game, int nb_players);
/* initialise la strucutre avec le nbr de joueurs, les scores à 0
   et leurs noms à "Joueurs i" */

#endif



