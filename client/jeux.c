#include <stdio.h> // printf
#include <stdlib.h> // perror/EXIT_SUCCESS/...
#include <unistd.h> // sleep
#include <string.h> //strcpy strlen
#include <ctype.h> // isdigit isalpha
#include <time.h>
#include <signal.h> //signal

#include <board_client.h> // Pour les fonctions du simulateur
#include "jeux.h"
#include "tools.h"

struct deck my_deck;

char *accueil[] = {
    "Bienvenu dans Pomelo, le jeu trop rigolo !",
    "Plusieurs modes de jeux sont proposes variant de 1 a 5 joueurs",
    "",
    "1. Cinq pour tous !",
    "2. Un max de questions !",
    "3. Buzzer buzzer buzzer !",
    "Votre choix : (1 seul caractere s'il vous plait, q pour quitter)", 
    NULL
};

char *cinq_pour_tous[] = {
    "Les regles sont simples :",
    "Chaque joueur repond a 5 questions. Il dispose de 10 secondes pour chacune d'elle",
    "Chaque joueur peut choisir le mode de reponse : duo, carre ou cash",
    "Les reponses duo rapportent 1 point, les carre 2 et les cash 3",
    "Si un joueur ne repond pas dans le temps imparti ou se trompe, il marque 0 point",
    "Essayez de ne pas faire de faute sur la reponse, lorsqu'elle est affichee !",
    "","",
    "Capiche ? Combien de joueurs alors ? (touche b + entree pour revenir)", 
    "(1 seul caractere s'il vous plait)", 
    NULL
};

char *max_de_questions[] = {
    "Les regles sont simples :",
    "Chaque joueur dispose de 30 secondes pour repondre a un max",
    "de questions la suite.",
    "1 point par bonne reponse, le compteur retombe a 0 si une reponse est fausse",
    "","",
    "Capiche ? Combien de joueurs alors ? (touche b + entree pour revenir)", 
    "(1 seul caractere s'il vous plait)", 
    NULL    
};

char *bbbuzzer[] = {
    "Les regles sont simples :",
    "11 questions sont posees a TOUS les joueurs en meme temps",
    "S'il a raison, il marque 1 point, 0 sinon",
    "Pour buzzer, ne pas oublier d'appuyer sur votre buzzer ET la touche entree",
    "","",
    "Capiche ? Combien de joueurs alors ? (touche b + entree pour revenir)", 
    "(1 seul caractere s'il vous plait)", 
    NULL
};

/* retourne l'indice dans la chaîne du caractère si trouvé, -1 sinon */
static int is_in(char *s, char ch) {
    char *s2 = s;
    for(; *s; s++)
        if(ch == *s)
            return (s - s2);

    return -1; 
}


void jeu_buzzer(const struct board_t *board, struct game *m_game) {
    clear_screen(board);
    char rep[SCREEN_COLUMNS] = "", buffer[SCREEN_COLUMNS]= "", c;
    char *buzzers = calloc(m_game->nb_players, sizeof(char));

    /* associer les buzzers aux joueurs */
    for(int i = 0; i < m_game->nb_players; i++) {
        bd_send_line(board, 0, m_game->players[i].name);        
        bd_send_line(board, 1, "Choix buzzer :");
        get_input(board, &buzzers[i], 3);
    }

    for(int i = 0; i < 11; i++) { // 11 questions à jouer
        c = 0;
        int qst;
        qst = print_question(board, my_deck, -1, NULL);
        bd_send_line(board, 5, "Qui buzze ?!");
        get_input(board, &c, 7);

        int k;
        while((k = is_in(buzzers, c)) == -1) { // tant que ce qui est buzzé n'existe pas
            bd_send_line(board, 8, "Buzzer inconnu !");
            sleep(1);
            bd_send_line(board, 8, "");
            get_input(board, &c, 7);
        }

        sprintf(buffer, "OK tente ta chance %s", m_game->players[k].name);
        bd_send_line(board, 8, buffer);

        get_input(board, rep, 10);

        for(int j = 1; j < 5; j++) { // vérification
            char *repv = my_deck.questions[qst*5+j];
            if(repv[0] == '*') {
                repv++; // on "enleve" l'étoile
                if(levenshtein(rep, repv) <= LIMIT) {
                    m_game->players[k].score++;
                    sprintf(buffer, "Correct ! score : %d", m_game->players[k].score);
                    bd_send_line(board, 10, buffer);
                    usleep(500000);
                }
              
                else {
                    sprintf(buffer, "FAUX ! score : %d", m_game->players[k].score);
                    bd_send_line(board, 11, buffer);
                    usleep(500000);
                }
            }
        }
    }  

    clear_screen(board);
    sort_and_print(board, *m_game);

    bd_send_line(board, 15, "JEU TERMINE, Voulez-vous rejouer ? y pour rejouer, autre pour revenir au menu");
    get_input(board, &c, 17);

    (c == 'y') ? jeu_cinq_pour_tous(board, m_game) : menu(board);           
}


void jeu_cinq_pour_tous(const struct board_t *board, struct game *m_game) { /* char *cinq_pour_tous[] */
    char rep[SCREEN_COLUMNS] = "", buffer[SCREEN_COLUMNS]= "", c;
    clear_screen(board);

    for(int i = 0; i < m_game->nb_players; i++) { // on fait jouer tous les joueurs
        sprintf(buffer, "Allez c'est a toi %s", m_game->players[i].name);
        clear_screen(board);

        if(i > 0) {
            bd_send_line(board, 10, "NEXT PLAYER");
            sleep(3);
            clear_screen(board);
        }
        
        bd_send_line(board, 0, buffer);
        sleep(2);    

        for(int a = 0; a < 5; a++) { // 5 questions chacun
            c = 0;
            int qst = print_question(board, my_deck, 0, &c);

            int t = time(NULL); /* chrono déclenché */
            get_input(board, rep, 15);

            if(time(NULL) - t > 10) { /* si ça a pris + de 10s */
                bd_send_line(board, 10, "TEMPS ECOULE");
                sleep(2); 
            }
            else 
            for(int j = 1; j < 5; j++) { // vérification
                char *repv = my_deck.questions[qst*5+j];
                if(repv[0] == '*') {
                    repv++; // on "enleve" l'étoile
                    if(levenshtein(rep, repv) <= LIMIT) {
                        m_game->players[i].score += (c - '0'); // on actualise le score selon DUO CARRE OU CASH
                        sprintf(buffer, "Correct ! score : %d", m_game->players[i].score);
                        bd_send_line(board, 10, buffer);
                        usleep(500000);
                    }

                    else {
                        sprintf(buffer, "FAUX ! score : %d", m_game->players[i].score);
                        bd_send_line(board, 11, buffer);
                        usleep(500000);
                    }
                }
            }
            sleep(1);
        }
    }
    clear_screen(board);
    sort_and_print(board, *m_game);

    bd_send_line(board, 15, "JEU TERMINE, Voulez-vous rejouer ? y pour rejouer, autre pour revenir au menu");
    get_input(board, &c, 17);

    (c == 'y') ? jeu_cinq_pour_tous(board, m_game) : menu(board);    
}

void jeu_max_de_questions(const struct board_t *board, struct game *m_game) { /* correspond à char *max_de_questions[] */
    char rep[SCREEN_COLUMNS] = "", buffer[SCREEN_COLUMNS]= ""; 
    clear_screen(board);

    for(int i = 0; i < m_game->nb_players; i++) { // on fait jouer tous les joueurs
        sprintf(buffer, "Allez c'est a toi %s", m_game->players[i].name);
        clear_screen(board);

        if(i > 0) {
            bd_send_line(board, 10, "NEXT PLAYER");
            sleep(3);
            clear_screen(board);
        }
        
        bd_send_line(board, 0, buffer);
        sleep(2);

    /* Mise en place du timer */
        int tdeb = time(NULL);
        while(time(NULL) - tdeb <= 30) { // 30 secondes pour enchainer les questions
            int qst;
            qst = print_question(board, my_deck, -1, NULL);
            get_input(board, rep, 3);

            for(int j = 1; j < 5; j++) { // vérification
                char *repv = my_deck.questions[qst*5+j];
                if(repv[0] == '*') {
                    repv++; // on "enleve" l'étoile
                    if(levenshtein(rep, repv) <= LIMIT) {
                        m_game->players[i].score++;
                        sprintf(buffer, "Correct ! score : %d", m_game->players[i].score);
                        bd_send_line(board, 10, buffer);
                        usleep(500000);
                    }

                    else {
                        m_game->players[i].score = 0; 
                        sprintf(buffer, "FAUX ! score : %d", m_game->players[i].score);
                        bd_send_line(board, 11, buffer);
                        usleep(500000);
                    }
                }
            }
        }
        bd_send_line(board, 9, "TEMPS ECOULE");
        sleep(2);
    }
    clear_screen(board);
    sort_and_print(board, *m_game);

    bd_send_line(board, 15, "JEU TERMINE, Voulez-vous rejouer ? y pour rejouer, autre pour revenir au menu");
    char c = 0; get_input(board, &c, 17);

    (c == 'y') ? jeu_max_de_questions(board, m_game) : menu(board);
}

void init_game(struct game *game, int nb_players) {
    game->state = STARTED;
    game->nb_players = nb_players;
    for(int i = 0; i < nb_players; i++) {
        game->players[i].score = 0;
        sprintf(game->players[i].name, "Joueur %d", i+1);
    }
}

void menu(const struct board_t *board) {
    char game_choice = 0, nb_players = 0;

    clear_screen(board);
    print_screen(board, accueil);

    get_input(board, &game_choice, 10);
    clear_screen(board);
    switch(game_choice) {
        case '1':
            print_screen(board, cinq_pour_tous);
            break;
        case '2':
            print_screen(board, max_de_questions);
            break;
        case '3':
            print_screen(board, bbbuzzer);
            break;
        case 'q': // on quitte
            bd_disconnect(board);
            exit(EXIT_SUCCESS);
        default:
            bd_send_line(board, 21, "Desole, ce n'est pas dans les choix");
            usleep(500000);
            menu(board);
    }

    get_input(board, &nb_players, 12);

    if(nb_players == 'b') 
        menu(board);

    else if(!isdigit(nb_players) || nb_players > '5') {
        bd_send_line(board, 21, "Desole, ce n'est pas dans les choix (n'oubliez pas 5 joueurs max)");
        usleep(500000);
        menu(board);
    }

    else {
        struct game my_game;

        init_game(&my_game, nb_players - '0'); // "cast" en int une fois passé en argument
        init_deck(&my_deck);

        clear_screen(board);
        bd_send_line(board, 0, "                      Le jeu va commencer dans...");
        countdown(board, 1);

        if(game_choice == '1')
            jeu_cinq_pour_tous(board, &my_game);

        else if(game_choice == '2')
            jeu_max_de_questions(board, &my_game);

        else 
            jeu_buzzer(board, &my_game);
        
    }
}
