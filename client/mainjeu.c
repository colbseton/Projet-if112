#include <stdio.h> // printf
#include <stdlib.h> // perror/EXIT_SUCCESS/...
#include <unistd.h> // Sleep
#include <time.h> // time
#include <ctype.h> // isdigit isalpha

#include <board_client.h> // Pour les fonctions du simulateur
#include "tools.h"
#include "jeux.h"

void exit_if(int cond, const char *msg) {
    if (cond) {
        perror(msg);
        exit(EXIT_FAILURE);
  }
}


int main(int argc, char *argv[]) {
    struct board_t board;
    int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;
    int res = bd_connect(&board, DEFAULT_HOST, port); // Se connecte au simulateur
    exit_if(res < 0, "Connection impossible");

    struct deck my_deck;

    srand(time(NULL));  //Générateur de nombres aléatoires

    bd_send_button_states(&board, 0x00); // Allume aucun bouton
    clear_screen(&board);
    
    init_deck(&my_deck);

    menu(&board);
    sleep(10);
    return EXIT_SUCCESS;
}