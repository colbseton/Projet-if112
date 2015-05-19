Pour compiler le board :
    - depuis le répertoire projet-if112 : make -C board
    - depuis le répertoire board : make

Pour compiler le client :
    - depuis le répertoire projet-if112 : make -C client
    - depuis le répertoire client : make

Pour exécuter le board et ensuite le client :
    Depuis projet-if112 : ./board/board &
                        Puis ./client/Jeu 
                        (Jeu est le nom de l'exécutable créé le make)