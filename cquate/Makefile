CC = g++
CFLAGS = -O2 -fpermissive
export INCLUDES = -I. -I../ -I../../cq_container/cq_container -I../lang -I../prog -I../storage -I../var
BASE_INCLUDES = -I. -Ilang -Iarmadillo -Iprog -Istorage -Ivar -I../cq_container/cq_container

LANG_DIR = lang
PROG_DIR = prog
STOR_DIR = storage
VAR_DIR = var

BASE_OBJ = cquate.o options.o terminal.o
LANG_OBJ = $(LANG_DIR)/interpret.o $(LANG_DIR)/interpretAccess.o $(LANG_DIR)/interpretBranch.o $(LANG_DIR)/interpretOperation.o $(LANG_DIR)/lex.o $(LANG_DIR)/lexCall.o $(LANG_DIR)/lexFile.o
PROG_OBJ = $(PROG_DIR)/state.o
STOR_OBJ = $(STOR_DIR)/fcnHash.o $(STOR_DIR)/map_hash.o $(STOR_DIR)/scriptHash.o $(STOR_DIR)/storage.o $(STOR_DIR)/variableSpace.o
VAR_OBJ = $(VAR_DIR)/var_string.o $(VAR_DIR)/variable_file.o $(VAR_DIR)/reflect.o

all : clean just_compile cquate

.PHONY: all

clean:
	rm -f cquate

just_compile:
	cd lang && $(CC) $(CFLAGS) $(INCLUDES) -I. -c *.cpp
	cd prog && $(CC) $(CFLAGS) $(INCLUDES) -I. -c *.cpp
	cd storage && $(CC) $(CFLAGS) $(INCLUDES) -I. -c *.cpp
	cd var && $(CC) $(CFLAGS) $(INCLUDES) -I. -c *.cpp
	$(CC) $(CFLAGS) $(BASE_INCLUDES) -I. -c *.cpp


OBJECTS = $(BASE_OBJ) $(LANG_OBJ) $(PROG_OBJ) $(STOR_OBJ) $(VAR_OBJ)

cquate : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -L ../cq_container/cq_container -l container -l lapack -l blas -o cquate
