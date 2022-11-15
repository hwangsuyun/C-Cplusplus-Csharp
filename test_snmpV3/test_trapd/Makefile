#DEFLAGS = -I../../net-snmp/include/net-snmp/library # header is not necessary
#CFLAGS = $(DEFLAGS)
CFLAGS = 
SNMP_LIBS = ./lib/libnetsnmpmibs.a ./lib/libnetsnmpagent.a ./lib/libnetsnmp.a -lcrypto
#SNMP_LIBS = ./lib/libnetsnmpmibs.a -ldl -lm -lrpm -lrpmio ./lib/libnetsnmpagent.a ./lib/libnetsnmp.a -lm -lcrypto
#SNMP_LIBS = -L/usr/local/lib -lnetsnmpmibs -ldl -lm -lrpm -lrpmio -lnetsnmpagent -lnetsnmp -lm -lcrypto
#SNMP_LIBS = -lnetsnmp -lnetsnmpagent -lcrypto
#SNMP_LIBS=`net-snmp-config --libs`
#SNMP_LIBS=`net-snmp-config --agent-libs`
#SNMP_LIBS=`net-snmp-config --libs --agent-libs`

#libnetsnmp.a equals -lnetsnmp

OBJS = snmptrapd.o
SRCS = $(OBJS:.o=.c)

TARGET = snmptrapd
$(TARGET) : $(OBJS)
	gcc -o $@ $(OBJS) $(SNMP_LIBS)

$(OBJS) : $(SRCS)
	gcc $(CFLAGS) -c -g $*.c

clean:
	rm -f *.o core*

