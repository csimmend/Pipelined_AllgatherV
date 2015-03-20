MPI_DIR=/export/opt/intel2015/impi/5.0.1.035
CC = $(MPI_DIR)/bin64/mpicc

#MPI_DIR=/export/opt/intel2015/impi/5.0.1.035/mic
#CC = $(MPI_DIR)/bin64/mpiicc

#MPI_DIR=$(HOME)/mvapich2-2.1a_enable-threads-multiple
#CC = $(MPI_DIR)/bin/mpicc

NAME += AllgatherV

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wshadow
#CFLAGS += -O3 -debug -mmic
CFLAGS += -O3 
CFLAGS += -std=c99
CFLAGS += -DUSE_ALIGNMENT
#CFLAGS += -DDEBUG

###############################################################################

INCLUDE_DIR += $(MPI_DIR)/include 
INCLUDE_DIR += ..

#LIBRARY_DIR += $(MPI_DIR)/lib
LIBRARY_DIR += $(MPI_DIR)/lib64
#LIBRARY_DIR += $(MPI_DIR)/lib64/mic

LDFLAGS += $(addprefix -L,$(LIBRARY_DIR))
CFLAGS += $(addprefix -I,$(INCLUDE_DIR))


OBJ += ../data
OBJ += ../now

LIB += m
LIB += mpi
#LIB += mpich

###############################################################################

BIN = $(addsuffix .exe, $(NAME))

default: $(BIN)

%.exe: %.o $(addsuffix .o, $(OBJ))
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(addprefix -l, $(LIB))

###############################################################################

%.run: %.exe
	@$(CMD_RUN) $^ | grep ^#

.PHONY: run

run: $(addprefix $(CURDIR)/, $(addsuffix .run, $(NAME)))

###############################################################################

.PHONY: clean objclean

objclean:
	rm -f ../*.o *.o

clean: objclean
	rm -f $(BIN)
