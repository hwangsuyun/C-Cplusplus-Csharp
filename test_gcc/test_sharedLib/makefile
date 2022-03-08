EXEC = test

OD = o
HD = h
SD = s
LD = lib
ID = .
fPIC = -fPIC

CFLAG = -I$(HD)
HEADERS = $(HD)/printMath.h $(HD)/lib.h
OBJ = $(OD)/main.o $(OD)/printMath.o $(OD)/lib.o
STATICLIB = $(LD)/staticlib.a
DYNAMICLIB = $(LD)/dynamiclib.so

all : $(EXEC)
	echo "success!!!"

$(EXEC) : $(OBJ)
	$(CC) -shared -o $(DYNAMICLIB) $(OBJ)
	$(CC) -o $(EXEC) $(DYNAMICLIB)
	rm -f ${OD}/*.o 

$(OD)/main.o : $(SD)/main.c
	$(CC) -c $(fPIC) $(SD)/main.c $(CFLAG);
	mv main.o $(OD)

$(OD)/printMath.o : $(SD)/printMath.c $(HEADERS)
	$(CC) -c $(fPIC) $(SD)/printMath.c $(CFLAG);
	mv printMath.o $(OD)

$(OD)/lib.o : $(LD)/lib.c $(HEADERS)
	$(CC) -c $(fPIC) $(LD)/lib.c $(CFLAG);
	mv lib.o $(OD)

clean :
	rm -rf $(OBJ) $(EXEC) $(STATICLIB) $(DYNAMICLIB)
