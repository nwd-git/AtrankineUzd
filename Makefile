.PHONY = all clean

all: main

main: main.o
	@echo "Checking..."
	gcc -lm main.o -o main

main.o: main.c
	@echo "Creating object.."
	gcc -c main.c

clean:
	@echo "Cleaning up..."
	rm -rvf main.o main
