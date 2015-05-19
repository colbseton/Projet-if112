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

!ATTENTION POUR LINUX!
	la touche de backspace n'est pas prise en compte
	(problème de compatibilité…)
	-- Dans ce cas -- : dans tools.h, replacer la valeur du #define EFFACER par la touche de votre choix
