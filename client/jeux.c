#include <stdio.h> // printf
#include <stdlib.h> // perror/EXIT_SUCCESS/...
#include <unistd.h> // Sleep
#include <string.h> //strcpy strlen
#include <ctype.h> // isdigit isalpha

#include <board_client.h> // Pour les fonctions du simulateur
#include "jeux.h"

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
    "Chaque joueur dispose de 2 minutes pour repondre a un max",
    "de questions la suite.",
    "1 point par bonne reponse, le compteur retombe a 0 si une reponse est fausse",
    "","",
    "Capiche ? Combien de joueurs alors ? (touche b + entree pour revenir)", 
    "(1 seul caractere s'il vous plait)", 
    NULL    
};

char *bbbuzzer[] = {
    "Les regles sont simples :"
    "Des questions sont posees a TOUS les joueurs en meme temps",
    "Le joueur ayant buzzer dispose de 7 secondes pour repondre",
    "S'il a raison, il marque 1 point, 0 sinon",
    "Si aucun joueur n'a buzze pendant 10 secondes, on passe a la question suivante",
    "","",
    "Capiche ? Combien de joueurs alors ? (touche b + entree pour revenir)", 
    "(1 seul caractere s'il vous plait)", 
    NULL
};

void init_game(struct game *game, int nb_players) {
    game->state = STARTED;
    for(int i = 0; i < nb_players; i++) {
        game->players[i].score = 0;
        sprintf(game->players[i].name, "Joueur %d", i+1);
    }
}

void menu(const struct board_t *board) {
    char game_choice = 0, nb_players = 0;

    clear_screen(board);
    print_screen(board, accueil);

    get_input(board, &game_choice, 20);
    switch(game_choice) {
        case '1':
            clear_screen(board);
            print_screen(board, cinq_pour_tous);
            get_input(board, &nb_players, 20);
            break;
        case '2':
            clear_screen(board);
            print_screen(board, max_de_questions);
            get_input(board, &nb_players, 20);
            break;
        case '3':
            clear_screen(board);
            print_screen(board, bbbuzzer);
            get_input(board, &nb_players, 20);
            break;
        case 'q':
            bd_disconnect(board);
            exit(EXIT_SUCCESS);
        default:
            bd_send_line(board, 21, "Desole, ce n'est pas dans les choix");
            usleep(500000);
            menu(board);
    }

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

        clear_screen(board);
        bd_send_line(board, 0, "Le jeu va commencer dans...");
        countdown(board);

        /* on lance ensuite une partie */
    }
}

