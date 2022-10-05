ENV_HOME = /home

#MAKE_DIR = $(ENV_HOME)/Make_D
#MAKE_INCLUDE = $(MAKE_DIR)/make_env
#MAKEFILES = $(MAKE_INCLUDE)/makefile

#include $(MAKE_INCLUDE)

CC=gcc

G_ID = ../Inc

MYCFLAGS = -I../Inc -DVAR_LIST

EXE_PGM = nas_lib.a

OD = O

#NAS_LIB = $(ENV_HOME)/Lib/$(EXE_PGM)
NAS_LIB = $(G_LD)/$(EXE_PGM)
LIB_OBJS = $(OD)/date_lib.o $(OD)/dir_lib.o $(OD)/inifile_lib.o \
	$(OD)/debug.o \
	$(OD)/tcp_lib.o \
	$(OD)/util_lib.o \
	$(OD)/ipc.o \
	$(OD)/cJSON.o \
	$(OD)/parse_data.o \
	$(OD)/file_lib.o \

HEADERS = makefile \
	$(G_ID)/lib_fn.h $(G_ID)/date_lib.h $(G_ID)/dir_lib.h $(G_ID)/inifile_lib.h \
	$(G_ID)/debug.h $(G_ID)/tcp_lib.h \
	$(G_ID)/util_lib.h \
	$(G_ID)/ipc.h \
	$(G_ID)/cJSON.h \
	$(G_ID)/parse_data.h \
	$(G_ID)/file_lib.h \

all: $(EXE_PGM)
	echo "bset!!"
	
$(EXE_PGM) : $(LIB_OBJS) makefile $(HEADERS)
	touch x.a
	rm *.a
	ar -r $(@) $(LIB_OBJS)

mkobjhome : 
	echo $(OBJ_HOME)
	mkdir $(OBJ_HOME)

mkobj :
	echo $(OD)
	mkdir $(OD)

mkbin :
	echo $(BD)
	mkdir $(BD)

$(OD)/date_lib.o : date_lib.c $(HEADERS)
	$(CC)  $(MYCFLAGS) -c date_lib.c
	mv *.o $(OD)

$(OD)/dir_lib.o : dir_lib.c $(HEADERS)
	$(CC)  $(MYCFLAGS) -c dir_lib.c
	mv *.o $(OD)

$(OD)/inifile_lib.o : inifile_lib.c $(HEADERS)
	$(CC)  $(MYCFLAGS) -c inifile_lib.c
	mv *.o $(OD)

$(OD)/debug.o : debug.c $(HEADERS)
	$(CC)  $(MYCFLAGS) -c debug.c
	mv *.o $(OD)

$(OD)/tcp_lib.o : tcp_lib.c $(HEADERS)
	$(CC)  $(MYCFLAGS) -c tcp_lib.c
	mv *.o $(OD)

$(OD)/util_lib.o : util_lib.c $(HEADERS)
	$(CC)  $(MYCFLAGS) -c util_lib.c
	mv *.o $(OD)

$(OD)/ipc.o : ipc.c $(HEADERS)
	$(CC)  $(MYCFLAGS) -c ipc.c
	mv *.o $(OD)

$(OD)/cJSON.o : cJSON.c $(HEADERS)
	$(CC)  $(MYCFLAGS) -c cJSON.c
	mv *.o $(OD)

$(OD)/parse_data.o : parse_data.c $(HEADERS)
	$(CC)  $(MYCFLAGS) -c parse_data.c
	mv *.o $(OD)

$(OD)/file_lib.o : file_lib.c $(HEADERS)
	$(CC)  $(MYCFLAGS) -c file_lib.c
	mv *.o $(OD)
