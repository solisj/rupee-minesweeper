all: board.c
	gcc -O3 -shared board.c -o board.so
	gcc -O3 -shared slowboard.c -o slowboard.so
