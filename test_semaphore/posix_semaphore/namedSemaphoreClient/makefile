SRC_DIR=.
LIB=-L../lib
INC=-I../inc
DEBUG=-D DEBUG
CC=cc

PROG=SemClient

SRC = \
       $(SRC_DIR)/Main.c \
       $(SRC_DIR)/SemClient.c \
       $(SRC_DIR)/cJSON.c \

LINKOBJ = \
	$(SRC_DIR)/Main.o \
	$(SRC_DIR)/SemClient.o \
	$(SRC_DIR)/cJSON.o  \

OBJS=$(SRC:.c=.o)
LIB=

all: $(OBJS) SemClient

SemClient: $(OBJS) $(LIB)
	$(CC) -w -o $@ $(LINKOBJ) $(INC) $(LIB) $(DEBUG) -lm 
	cp $@ ../bin
	rm -rf *.o
	rm -rf core*

$(OBJS): makefile $(SRC)
	$(CC) -c $*.c $(INC) $(DEBUG)

clean :
	rm -rf core*
