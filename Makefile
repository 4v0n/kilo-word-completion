main: src/main.c
	$(CC) src/main.c src/terminal.c -o build/main -Wall -Wextra -pedantic -std=c99 -Iinclude