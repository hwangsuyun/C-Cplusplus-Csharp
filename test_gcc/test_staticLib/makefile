EXEC = test

OD = o
HD = h
SD = s
LD = lib
ID = .
CC = cc

CFLAG = -I$(HD)
HEADERS = $(HD)/printMath.h $(HD)/lib.h
OBJ = $(OD)/main.o $(OD)/printMath.o $(OD)/lib.o
STATICLIB = $(LD)/staticlib.a

all : $(EXEC)
	echo "success!!!"

$(EXEC) : $(OBJ)
	ar -rscv $(STATICLIB) $(OBJ)
	$(CC) -o $(EXEC) $(STATICLIB)
	rm -f ${OD}/*.o 

$(OD)/main.o : $(SD)/main.c
	$(CC) -c $(SD)/main.c $(CFLAG);
	mv main.o $(OD)

$(OD)/printMath.o : $(SD)/printMath.c $(HEADERS)
	$(CC) -c $(SD)/printMath.c $(CFLAG);
	mv printMath.o $(OD)

$(OD)/lib.o : $(LD)/lib.c $(HEADERS)
	$(CC) -c $(LD)/lib.c $(CFLAG);
	mv lib.o $(OD)

clean :
	rm -rf $(OBJ) $(EXEC) $(STATICLIB)
