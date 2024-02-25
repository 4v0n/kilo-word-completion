CFLAGS = -Wall -Wextra -pedantic -std=c99 -g

main: src/main.c \
      src/terminal.c \
      src/input.c \
      src/output.c\
      src/io.c
	$(CC) $(CFLAGS) $^ -o build/kilo -Iinclude
