OD = O
SD = S
CC = gcc

MYCFLAGS = -I./Inc -I./Incl -I.

PROG = Shared_Server

SRC = \
	$(SD)/main.c	\
	$(SD)/server_daemon.c	\
	$(SD)/server_main.c	\

LINKOBJ =			\
	$(OD)/main.o	\
	$(OD)/server_daemon.o	\
	$(OD)/server_main.o	\

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
	$(CC) $(MYCFLAGS) -o $@ $(LINKOBJ) $(LIB) -lm
	rm $(OD)/*.o

$(OBJS) :  makefile $(SRC)
	$(CC) $(MYCFLAGS) -c $*.c
	mv *.o $(OD)
