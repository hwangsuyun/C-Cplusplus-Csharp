TARGET = test

#CC = gcc
cc = cc
PROC = proc
LIB = -L$(ORACLE_HOME)/lib -lclntsh
PROCINC = $(ORACLE_HOME)/precomp/public

OBJS = main.o
ORA_GARBAGE = *.lis tp* main.c *.log

all:    $(TARGET)

$(TARGET):  
	$(PROC) main.pc
	$(CC) -c main.c
	$(CC) -o $(TARGET) $(OBJS) $(LIB)
clean:
	rm -rf $(TARGET) $(OBJS) $(ORA_GARBAGE)
