#DEFLAGS = -I../../net-snmp/include/net-snmp/library # header is not necessary
#CFLAGS = $(DEFLAGS)
CFLAGS = 
SNMP_LIBS = ./lib/libnetsnmp.a -lcrypto
#SNMP_LIBS = -lnetsnmp 
#libnetsnmp.a equals -lnetsnmp

OBJS = snmpdemoapp.o
SRCS = $(OBJS:.o=.c)

TARGET = snmpdemoapp
$(TARGET) : $(OBJS)
	gcc -o $@ $(OBJS) $(SNMP_LIBS)

$(OBJS) : $(SRCS)
	gcc $(CFLAGS) -c -g $*.c

clean:
	rm -f *.o core*

