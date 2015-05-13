# Remplacez main par le nom de votre projet
EXEC=Jeu
BOARD_PATH=../board
CFLAGS= -std=c99 -O3 -g -ggdb -Wall -Werror -I../board
LDLIBS=-L$(BOARD_PATH) -lbd
LD_FLAGS=-ggdb
CPPFLAGS=-D_XOPEN_SOURCE=500
# Completez par la liste de vos fichier objets (.o)
OBJS= mainjeu.o tools.o jeux.o

all: $(EXEC)
$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)
lib: $(BOARD_PATH)/libbd.a
clean:
	$(RM) $(EXEC) *.o
$(BOARD_PATH)/libbd.a:
	make -sC $(BOARD_PATH) libbd.a
.PHONY: all clean lib
