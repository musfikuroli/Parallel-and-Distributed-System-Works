# Compiler
CC = mpicc

# Executable name
EXEC = mpi_pi_calculation

# Source file
SRC = $(EXEC).c

# Compilation flags
CFLAGS = -o

# Default number of processes
np ?= 4

.PHONY: all run clean

# Default target
all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $@ $<

run: $(EXEC)
	mpirun --oversubscribe -np $(np) ./$(EXEC)

clean:
	rm -f $(EXEC)
