#include <stdio.h> // printf
#include <stdlib.h> // perror/EXIT_SUCCESS/...
#include <unistd.h> // Sleep
#include <string.h> //strcpy strlen
#include <ctype.h> // isdigit isalpha

#include <board_client.h> // Pour les fonctions du simulateur
#include "jeu1.h"

void wait_for_quit(const struct board_t *board) {
    while(1) { // appuyer sur q ou shift Q pour quitter le programme
        int k = bd_read_key(board);
        if(k == 'q' || k == 'Q')
            bd_disconnect(board);
        } 
}


void countdown(const struct board_t *board) {
    int i = 0;
    char line[] = "                                                                                ";

    while (!(bd_read_button_state(board) & 0x02))
        ;

    for(; i <= 5; i++) {
        line[44 + 8]  = '0' + 5 - i;
        bd_send_line(board, 13, line);
        usleep(1000000);
    }
    
    bd_send_line(board, 14, "                                         Décompte fini !                                  ");  
}


static void str_form(char *str, struct double_char *str_formed) {
    /* une question est un tableau de chaîne, chaque ligne est découpée 
       de façon à ne pas dépasser la longueur de l'écran */
    int i = 0, j = 0, len_str = strlen(str);
    
    str_formed->n_i = 1+len_str/SCREEN_COLUMNS;
    str_formed->s = malloc(sizeof(char*) * SCREEN_LINES);
    for(; i < str_formed->n_i; i++) {
        int k = 0;
        str_formed->s[i] = malloc(sizeof(char) * (SCREEN_COLUMNS + 1));
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
    for(int i = 0; i < d_str->n_i; i++) {
        free(d_str->s[i]);
    }
    free(d_str->s);
}


void init_deck(struct deck *my_deck) {
    FILE *q_file = fopen("client/questions.txt", "r");
    char buffer[MAX_QUESTION_LEN] = "";

    my_deck->nb_lignes = 0;
    my_deck->nb_qst = 0;

    if(q_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    } 
    else {
        while(fgets(buffer, MAX_QUESTION_LEN, q_file) != NULL) {
            if(buffer[0] == '?') {
                my_deck->nb_qst++;
                my_deck->nb_lignes++;
    
            // on ne compte les ? suivants qui font partie de la même question
                while(fgets(buffer, MAX_QUESTION_LEN, q_file) != NULL && buffer[0] == '?')
                    ;

                if(buffer[0] != '?')
                    fseek(q_file, -strlen(buffer)* sizeof(char), SEEK_CUR); // pour ne pas sauter la ligne qu'on vient de lire
            }

            else if(isalnum(buffer[0]) || buffer[0] == '*')
                my_deck->nb_lignes++;
        }

        fseek(q_file, 0, SEEK_SET); // on se replace au début du fichier

        /* création et mise en mémoire du fichier de questions */

        my_deck->questions = malloc(sizeof(char*) * my_deck->nb_lignes);

        int i = 0;
        while(i < my_deck->nb_lignes) {
            my_deck->questions[i] = malloc(sizeof(char) * MAX_QUESTION_LEN);
            fgets(buffer, MAX_QUESTION_LEN, q_file);
            
            if(buffer[0] == '?') { // premier ? qu'on croise
                char big_buffer[MAX_QUESTION_LEN] = "";
                strcpy(big_buffer, buffer);

                /* concaténer ensemble les lignes de ? suivantes */
                while(fgets(buffer, MAX_QUESTION_LEN, q_file) != NULL && buffer[0] == '?') {

                    buffer[0] = ' '; // on efface les ? qui suivent (laissant que le premier)
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

void print_(struct double_char str) {
    for(int i = 0; i < str.n_i; i++)
        printf("%s", str.s[i]);

}
void print_question(const struct board_t *board, struct deck m_deck, int flag) {

    /* pour afficher les réponses :
        ligne 14    -> réponse 1    <-> bouton 3
        ligne 20    -> réponse 2    <-> bouton 4
        ligne 14    -> réponse 3    <-> bouton 7
        ligne 20    -> réponse 4    <-> bouton 8

       S'il n'y a que deux réponses, on affichera à côté du bouton 2 et 3 
       et c'est selon la valeur du flag
    */

    int num_q = rand() % m_deck.nb_qst; // numéro de la question dans le fichier
    char *str = m_deck.questions[num_q*5];

    struct double_char qst_formed;
    str_form(str, &qst_formed);

    print_screen(board, qst_formed.s);
    __free(&qst_formed);

}