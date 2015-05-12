#ifndef _JEU1_H
#define _JEU1_H

#define MAX_LEN 5000
#define NB_MAX_PLAYERS 5

#define ENTREE 13 // pour '\n', 13 et pas 10 visiblement
#define EFFACER 127 // pour effacer un caractère

struct deck {
    int nb_qst;
    int nb_lignes;
    char **questions;
};

struct double_char {
    char **s;
    int n_i; // nombre de lignes du double tableau
};

enum game_state { OVER, STARTED };

struct player {
    int score;
    char *name;
};

struct game {
    struct player players[NB_MAX_PLAYERS];
    enum game_state state;
};


void __free(struct double_char *d_str); // size est le nombre de lignes


void wait_for_quit(const struct board_t *board); 
/* attendre l'appuie d'une touche pour quitter */


void print_question(const struct board_t *board, struct deck m_deck, int flag);


void countdown(const struct board_t *board);


void init_deck(struct deck *my_deck); 
/* on calcule le nombre de questions et on charge le fichier dans un 
   tableau de chaîne */

void get_input(const struct board_t *board, char *str); // récupère une chaîne au board
#endif