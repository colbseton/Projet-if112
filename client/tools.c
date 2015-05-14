#include <stdio.h> // printf
#include <stdlib.h> // perror/EXIT_SUCCESS/...
#include <unistd.h> // Sleep
#include <string.h> //strcpy strlen
#include <ctype.h> // isdigit isalpha

#include <board_client.h> // Pour les fonctions du simulateur
#include "tools.h"
#include "jeux.h"

int levenshtein(char *s1, char *s2) {
    unsigned int x, y, s1len, s2len;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int matrix[s2len+1][s1len+1];
    matrix[0][0] = 0;
    for (x = 1; x <= s2len; x++)
        matrix[x][0] = matrix[x-1][0] + 1;
    for (y = 1; y <= s1len; y++)
        matrix[0][y] = matrix[0][y-1] + 1;
    for (x = 1; x <= s2len; x++)
        for (y = 1; y <= s1len; y++)
            matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (s1[y-1] == s2[x-1] ? 0 : 1));
 
    return(matrix[s2len][s1len]);
}

void sort(struct player p[], int nb_players) 
{
    int i = 0, unsorted = 1;
    struct player tmp;
    while (unsorted) {
        unsorted = 0;
        for (i = 0; i < nb_players-1; i++) {
            if(p[i].score > p[i+1].score) {
                /* Inversion des 2 éléments */
                tmp = p[i+1];
                p[i+1] = p[i];
                p[i] = tmp;
 
                unsorted = 1;
            }
        }
    }
}
static void str_form(char *str, struct double_char *str_formed) {
    /* une question est un tableau de chaîne, chaque ligne est découpée 
       de façon à ne pas dépasser la longueur de l'écran */
    int i = 0, j = 0, len_str = strlen(str);
    
    str_formed->n_i = 1+len_str/SCREEN_COLUMNS;
    str_formed->s = calloc(SCREEN_LINES, sizeof(char*));

    for(; i < str_formed->n_i; i++) {
        int k = 0;
        str_formed->s[i] = calloc((SCREEN_COLUMNS + 1), sizeof(char));

        for(k = 0; j < len_str && (k < SCREEN_COLUMNS +1); k++,j++) {
            if(str[j] == '\n') 
                str[j] = ' ';
          
            else if(str[j] == '?')
                str[j] = '#'; 
            /* c'est le ? du début, les autres 
            ont été enlevés dans init_deck() */

            str_formed->s[i][k] = str[j];
        }
    }
    strcat(str_formed->s[i-1], "?"); // moche mais dur avec les indices 
}


void __free(struct double_char *d_str) {
    int i = 0;
    for(; i < d_str->n_i; i++) {
        free(d_str->s[i]);
    }
    free(d_str->s);
}


void init_deck(struct deck *my_deck) {
    FILE *q_file = fopen("client/questions.txt", "r");
    char buffer[MAX_LEN] = "";

    my_deck->nb_lignes = 0;
    my_deck->nb_qst = 0;

    if(q_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    } 
    else {
        while(fgets(buffer, MAX_LEN, q_file) != NULL) {
            if(buffer[0] == '?') {
                my_deck->nb_qst++;
                my_deck->nb_lignes++;
    
            // on ne compte les ? suivants qui font partie de la même question
                while(fgets(buffer, MAX_LEN, q_file) != NULL && buffer[0] == '?')
                    ;

                if(buffer[0] != '?')
                    // pour ne pas sauter la ligne qu'on vient de lire
                    fseek(q_file, -strlen(buffer)* sizeof(char), SEEK_CUR); 
            }

            else if(isalnum(buffer[0]) || buffer[0] == '*')
                my_deck->nb_lignes++;
        }

        fseek(q_file, 0, SEEK_SET); // on se replace au début du fichier

        /* création et mise en mémoire du fichier de questions */
        my_deck->questions = calloc(my_deck->nb_lignes, sizeof(char*));

        int i = 0;
        while(i < my_deck->nb_lignes) {
            my_deck->questions[i] = calloc(MAX_LEN, sizeof(char));
            fgets(buffer, MAX_LEN, q_file);
            
            if(buffer[0] == '?') { // premier ? qu'on croise
                char big_buffer[MAX_LEN] = "";
                strcpy(big_buffer, buffer);

                /* concaténer ensemble les lignes de ? suivantes */
                while(fgets(buffer, MAX_LEN, q_file) != NULL && buffer[0] == '?') {
                    buffer[0] = ' '; // on suppr les ? qui suivent (laissant que le premier)
                    strcat(big_buffer, buffer); 
                } 
                if(buffer[0] != '?')
                    // pour ne pas sauter la ligne qu'on vient de lire
                    fseek(q_file, -strlen(buffer)* sizeof(char), SEEK_CUR); 

                strcpy(my_deck->questions[i++], big_buffer);
            }

            else if(isalnum(buffer[0]) || buffer[0] == '*') 
                strcpy(my_deck->questions[i++], buffer);            
        }
        fclose(q_file);
    }
}

void print_screen(const struct board_t *board, char **text) {
    int i = 0;
    for (i = 0; text[i] && i < SCREEN_LINES; i ++)
        bd_send_line(board, i, text[i]);
    while (i < SCREEN_LINES) {
        bd_send_line(board, i, "");
        i++;
    }
}


void countdown(const struct board_t *board, int line) {
    int i = 0;
    char linebuff[SCREEN_COLUMNS] = "                                                                                ";
 
    for(; i <= 5; i++) {
        linebuff[44 + 8]  = '0' + 5 - i;
        bd_send_line(board, line, linebuff);
        usleep(1000000);
    }
    bd_send_line(board, line, "                                                    Go !");  
}


void clear_screen(const struct board_t *board) {
    for(int i = 0; i < SCREEN_LINES; i ++)
        bd_send_line(board, i, "");
}


void get_input(const struct board_t *board, char *str, int nb_line) {
    char k = '0';
    char line[SCREEN_COLUMNS] = "                                                                                ";
    int digit = 40, i = 0;

    if(nb_line <= 0) {
        fprintf(stderr, "get_input : nb_line doit etre > 0 !\n");
        exit(EXIT_FAILURE);
    }

    while (k != ENTREE) { //
        bd_send_line(board, nb_line-1, "Reponse stp :"); // Affiche une invite
        k = bd_read_key(board); // on lit une touche

        if (isalnum(k) || k == ' ' || k == '\'') { 
            line[44 + 8 - digit] = k; // et on rajoute un carac à l'affichage
            str[i] = k; // on le stock
            digit --; i++;
        }

        else if(k == EFFACER) {
            i--; digit++;
            line[44 + 8 - digit] = ' ';
            str[i] = ' ';
        }
        
        bd_send_line(board, nb_line, line); // et on affiche les caractères
        usleep(100);
    }        
    str[++i] = '\0';
}


void print_answer(const struct board_t *board, struct deck m_deck, int num_q, char **qst_formed) {
    char rep[4][MAX_LEN] = {{0}}, c = 0;
    bd_send_line(board, 5, "Duo (1), carre (2) ou cash (3) ?");
    get_input(board, &c, 7);

    switch(c) {
        case DUO: {
            int i;
            for(i = 1; i < 5; i++) { //on cherche d'abord la bonne réponse
                if(m_deck.questions[num_q*5+i][0] == '*') {
                    int j = 1;
                    //on recopie la réponse sans récupérer le caractère * 
                    for(; m_deck.questions[num_q*5+i][j] != '\0'; j++)
                        rep[0][j-1] = m_deck.questions[num_q*5+i][j];

                    rep[0][j-1] = '\0';
                }
            }

            do { i = rand() %4; } // on en pioche ensuite une autre
            while(m_deck.questions[num_q*5+i+1][0] == '*');

            strcpy(rep[1], m_deck.questions[num_q*5+i+1]);
            break;
        }

        case CARRE:
            for(int i = 1; i < 5; i++) {
                if(m_deck.questions[num_q*5+i][0] == '*') {
                    int j = 1;
                    //on recopie la réponse sans récupérer le caractère * 
                    for(; m_deck.questions[num_q*5+i][j] != '\0'; j++)
                        rep[i-1][j-1] = m_deck.questions[num_q*5+i][j];
                    rep[i-1][j-1] = '\0';
                }
                else strcpy(rep[i-1], m_deck.questions[num_q*5+i]);
            }
            break;
        default:
            break;
    }

    clear_screen(board);

    print_screen(board, qst_formed);

    bd_send_line(board, 14, rep[0]);
    bd_send_line(board, 15, rep[1]);
    bd_send_line(board, 16, rep[2]);
    bd_send_line(board, 17, rep[3]);
}

int print_question(const struct board_t *board, struct deck m_deck, int flag) {
    int num_q = rand() % m_deck.nb_qst; // numéro de la question dans le fichier
    char *str = m_deck.questions[num_q*5]; // accéder à la question dans le deck
    struct double_char qst_formed;

    str_form(str, &qst_formed); // on adapte la question au board

    print_screen(board, qst_formed.s);

    if(flag != -1) // -1, ne pas laisser le choix, c'est du cash (comme dans certains jeux)
        print_answer(board, m_deck, num_q, qst_formed.s);

    __free(&qst_formed);  
    return num_q;
}
