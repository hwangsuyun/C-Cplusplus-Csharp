.SUFFIXES = .c.o

#OSOPT = -lsocket -lnsl

PD = .
BD = ../../bin
SD = .
LD = ../Lib
ID = ../Incl

TARGET = $(BD)/PCC

CC = gcc

CFLAG = -c -I$(SD) -I$(ID) -I$(LD) -DVAR_LIST -DTEST 

LIBS = $(LD)/utillib.o \
	$(LD)/tcplib.o \
	$(LD)/filelib.o \
	$(LD)/alarmlib.o \
	$(LD)/semaphore.o \
	$(LD)/inifiles.o \
	$(LD)/dirinfo.o \
	$(LD)/date.o \
	$(LD)/log.o

OBJS = $(SD)/pcc.o 

$(TARGET) : $(OBJS) $(LIBS) 	
	$(CC) -o $(@) $(OSOPT) $(OBJS) $(LIBS)
	rm -f $(OBJS)

.c.o : 
	$(CC) $(CFLAG) $<

clean :
	rm -f *.o
	touch *.c
