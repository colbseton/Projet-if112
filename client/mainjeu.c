#include <stdio.h> // printf
#include <stdlib.h> // perror/EXIT_SUCCESS/...
#include <unistd.h> // Sleep
#include <time.h> // time

#include <board_client.h> // Pour les fonctions du simulateur
#include "jeu1.h"

void exit_if(int cond, const char *msg) {
    if (cond) {
        perror(msg);
        exit(EXIT_FAILURE);
  }
}

void clear_screen(struct board_t *board) {
    for(int i = 0; i < SCREEN_LINES; i ++)
        bd_send_line(board, i, "");
}


int main(int argc, char *argv[]) {
    struct board_t board;
    int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;
    int res = bd_connect(&board, DEFAULT_HOST, port); // Se connecte au simulateur
    exit_if(res < 0, "Connection impossible");

    struct deck my_deck;

    srand(time(NULL));  //Générateur de nombres aléatoires

    bd_send_button_states(&board, 0xff); // Allume tous les boutons
    clear_screen(&board);
    
    init_deck(&my_deck);
    while(1){
        print_question(&board, my_deck, 1);
        usleep(2000000);
        clear_screen(&board);
    }

    wait_for_quit(&board);


    return EXIT_SUCCESS;
}