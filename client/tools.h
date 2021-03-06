#ifndef _TOOLS_H
#define _TOOLS_H

#define MAX_LEN     5000
#define ENTREE      13 // pour '\n', 13 et pas 10 visiblement
#define EFFACER     127 // pour effacer un caractère
#define LIMIT       3 // tolérance pour les fautes d'orthographe

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

#include "jeux.h"

struct deck {
    int nb_qst;
    int nb_lignes;
    char **questions;
};

struct double_char {
    char **s;
    int n_i; // nombre de lignes du double tableau
};

void sort(struct player p[], int nb_players);

void __free(struct double_char *d_str); // size est le nombre de lignes

void sort_and_print(const struct board_t *board, struct game m_game);
/* classe les joueurs et affiche le classement */


int print_question(const struct board_t *board, struct deck m_deck, int flag, char *c);
/* sélectionne, met en forme et affiche la question
   si flag = -1 -> cash par défaut, retourne le numéro de la question */

int levenshtein(char *s1, char *s2);

void print_screen(const struct board_t *board, char **text);

void print_answer(const struct board_t *board, struct deck m_deck, int num_q, char **qst_formed, char *c);
/* demande à l'utilisateur duo carré ou cash, affiche les réponses en fonction et 
   retourne par effet de bord son choix */

void clear_screen(const struct board_t *board);


void countdown(const struct board_t *board, int line);


void init_deck(struct deck *my_deck); 
/* on calcule le nombre de questions et on charge le fichier dans un 
   tableau de chaîne */

void get_input(const struct board_t *board, char *str, int nb_line); 
/* récupère une chaîne au board */
#endif

