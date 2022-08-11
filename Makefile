NAME 		= 	Cthulhu

LIBS 		=	-lcurses -ltermcap -lc
LDFLAGS		=

#CFLAGS		= -g -DTERMCAP -DSYSV
#CFLAGS		= -g -xenix -DTERMCAP
#CFLAGS		= -ql -DPROTO -DTERMCAP
CFLAGS		= -g -DTERMCAP
CC		= gcc

SRC =	clock.c combat.c end_game.c errors.c file_func.c graphics.c heal.c \
	init.c main.c misc.c monsters.c objects.c spell.c start_game.c  \
	travel.c

OBJ =	clock.o combat.o end_game.o errors.o file_func.o graphics.o heal.o \
	init.o main.o misc.o monsters.o objects.o spell.o start_game.o \
	travel.o

$(NAME):  $(OBJ) 
	$(CC)  -o $(NAME) $(LDFLAGS) $(CFLAGS) $(OPTIONS) $(OBJ) $(LIBS)
	@chmod 2711 $(NAME)
	@chgrp games $(NAME)
	@ls -ls $(NAME)

$(OBJ):	cthulhu.h proto.h
	$(CC) -c $(FILES) $(CFLAGS) $<

proto: 
	rm -f proto.h

proto.h:
	$(CC) -Zg -p1 /lib/p1 $(SRC) > proto.h

all: proto $(SRC)

clean:
	rm -f $(OBJ)
	rm -f proto.h

clobber: clean
	rm -f $(NAME)
