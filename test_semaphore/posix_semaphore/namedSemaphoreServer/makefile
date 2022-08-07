OD = O
SD = S
CC = gcc

MYCFLAGS = -I./LibInc -I./Incl -I.

PROG = SemServer

SRC = \
	$(SD)/Main.c	\
	$(SD)/ServerDaemon.c	\
	$(SD)/ServerMain.c	\

LINKOBJ =			\
	$(OD)/Main.o	\
	$(OD)/ServerDaemon.o	\
	$(OD)/ServerMain.o	\

OBJS=$(SRC:.c=.o)

LIB = ./LibSrc/nas_lib.a

all : $(PROG)
	echo "Sucess!"

k : kill_p $(PROG)
	echo "Sucess!"
	
kill_p : 
	naskill $(PROG)
	sleep 1

$(PROG) : $(OBJS) $(LIB)
	$(CC) $(MYCFLAGS) -o $@ $(LINKOBJ) $(LIB) -lm -lpthread 
	rm $(OD)/*.o

$(OBJS) :  makefile $(SRC)
	$(CC) $(MYCFLAGS) -c $*.c
	mv *.o $(OD)
