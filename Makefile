main: src/main.c
	$(CC) src/main.c src/terminal.c src/input.c -o build/kilo -Wall -Wextra -pedantic -std=c99 -Iinclude