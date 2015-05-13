#include <stdio.h> // printf
#include <stdlib.h> // perror/EXIT_SUCCESS/...
#include <unistd.h> // Sleep
#include <string.h> //strcpy strlen
#include <ctype.h> // isdigit isalpha

#include <board_client.h> // Pour les fonctions du simulateur
#include "tools.h"

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

void countdown(const struct board_t *board) {
    int i = 0;
    char line[SCREEN_COLUMNS] = "                                                                                ";
 
    for(; i <= 5; i++) {
        line[44 + 8]  = '0' + 5 - i;
        bd_send_line(board, 7, line);
        usleep(1000000);
    }
    bd_send_line(board, 7, "                                                    Go !");  
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

void print_question(const struct board_t *board, struct deck m_deck, int flag) {
    /* pour afficher les réponses :
       on affiche ligne par ligne, simplement
       flag = 4 <-> carré, flag = 2 <-> duo, flag = 0 <-> cash 
    */

    int num_q = rand() % m_deck.nb_qst; // numéro de la question dans le fichier
    char *str = m_deck.questions[num_q*5]; // accéder à la question dans le deck
    struct double_char qst_formed;
    char rep[4][MAX_LEN] = {{0}};

    str_form(str, &qst_formed); // on adapte la question au board

    switch(flag) {
        case 2: {
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

        case 4:
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

    print_screen(board, qst_formed.s);
    bd_send_line(board, 14, rep[0]);
    bd_send_line(board, 15, rep[1]);
    bd_send_line(board, 16, rep[2]);
    bd_send_line(board, 17, rep[3]);

    __free(&qst_formed);  
}