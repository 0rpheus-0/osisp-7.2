CC = gcc
CFLAGS = -W -Wall -std=c11 -pedantic -pthread -Wno-unused-parameter -Wno-unused-variable -g -g3 -fsanitize=address
.PHONY : clean

all : main generation

run : main generation
	./main

main: main.c Makefile
	$(CC) $(CFLAGS) main.c -o main

generation: generation.c Makefile
	$(CC) $(CFLAGS) generation.c -o generation

clean:
	rm -f main generation