CC = mpicc
CFLAGS = -Wall

all: master worker

master: master.c
	$(CC) $(CFLAGS) -o master master.c

worker: worker.c
	$(CC) $(CFLAGS) -o worker worker.c

run: all
	mpirun --oversubscribe -np 1 ./master

clean:
	rm -f master worker

