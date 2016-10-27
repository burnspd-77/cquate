CC = g++
CFLAGS = -lm -O2
export INCLUDES = -I../ -I. -I../armadillo-6.500.5/include/armadillo_bits -I../armadillo -I../container -I../hash -I../matrix -I../string -I../variable -include ../armadillo-6.500.5/include/armadillo
BASE_INCLUDES = -I. -Iarmadillo-6.500.5/include/armadillo_bits -Iarmadillo -Icontainer -Ihash -Imatrix -Istring -Ivariable -include armadillo-6.500.5/include/armadillo

ARMA_DIR = armadillo
CONT_DIR = container
HASH_DIR = hash
MAT_DIR = matrix
STR_DIR = string
VAR_DIR = variable

BASE_OBJ = cubeOps.o my_errors.o
ARMA_OBJ = $(ARMA_DIR)/arma.o
CONTAINER_OBJ = $(CONT_DIR)/container.o $(CONT_DIR)/containerOps.o
HASH_OBJ = $(HASH_DIR)/hash.o
MATRIX_OBJ = $(MAT_DIR)/cube.o $(MAT_DIR)/cubeAccess.o $(MAT_DIR)/cubeLib.o $(MAT_DIR)/handlers.o $(MAT_DIR)/matAccess.o $(MAT_DIR)/matrix.o $(MAT_DIR)/matrixExp.o $(MAT_DIR)/matrixLib.o $(MAT_DIR)/matrixLinear.o $(MAT_DIR)/matrixOps.o $(MAT_DIR)/matrixStats.o $(MAT_DIR)/matrixTrig.o $(MAT_DIR)/random.o $(MAT_DIR)/special.o
STRING_OBJ = $(STR_DIR)/c_string.o
VARIABLE_OBJ = $(VAR_DIR)/variable.o


all : clean just_compile lib

.PHONY: all

clean:
	rm -f container.lib

just_compile:
	cd armadillo && $(CC) $(CFLAGS) $(INCLUDES) -I. -c *.cpp
	cd container && $(CC) $(CFLAGS) $(INCLUDES) -I. -c *.cpp
	cd hash && $(CC) $(CFLAGS) $(INCLUDES) -I. -c *.cpp
	cd matrix && $(CC) $(CFLAGS) $(INCLUDES) -I. -c *.cpp
	cd string && $(CC) $(CFLAGS) $(INCLUDES) -I. -c *.cpp
	cd variable && $(CC) $(CFLAGS) $(INCLUDES) -I. -c *.cpp
	$(CC) $(CFLAGS) $(BASE_INCLUDES) -I. -c *.cpp


OBJECTS = $(BASE_OBJ) $(ARMA_OBJ) $(CONTAINER_OBJ) $(HASH_OBJ) $(MATRIX_OBJ) $(STRING_OBJ) $(VARIABLE_OBJ)

lib : $(OBJECTS)
	ar rcs libcontainer.a $(OBJECTS)
