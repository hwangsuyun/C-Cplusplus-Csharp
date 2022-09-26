include $(USRINC)/mk.batch

TARGET=bgd_auto_recv_nas_sftp

SMWLIB=-L$(USRLIB) -lNAS

PROC_SRCS=db_apply.pc 

SRCS= \
	$(COMM_SRCS) \
	auto_recv_nas_sftp.c \
	process_init.c \
	sftp_lib.c \
	$(PROC_CS) 

all: $(TARGET) copy_target clean
	@echo "${CHR_REVERSE} MAKE OK!! ${CHR_NORMAL}"

$(TARGET): $(OBJS) Makefile 
	$(CC) $(CFLAGS) -o $(TARGET) -lcurl -m64 $(SRCS) $(ORALIBS) $(SMWLIB)

#$(OBJS): $(SRCS) Makefile
#	$(CC) $(CFLAGS) -c $*.c -m64

$(PROC_CS): $(PROC_SRCS) Makefile
	proc $(PROCFLAG) iname=$*.pc 

copy_target:
	cp $(TARGET) $(TARGETDIR)

clean:
	rm -f core $(OBJS) $(PROC_CS) $(PROC_LIS) tp*
	touch *
