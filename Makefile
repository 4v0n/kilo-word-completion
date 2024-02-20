main: src/main.c \
      src/terminal.c \
      src/input.c \
      src/output.c
	$(CC) $^ -o build/kilo -Wall -Wextra -pedantic -std=c99 -Iinclude
