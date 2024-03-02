CFLAGS = -Wall -Wextra -pedantic -std=c99 -g

main: src/main.c \
      src/terminal.c \
      src/input.c \
      src/output.c\
      src/io.c\
      src/row_operations.c\
      src/editor_operations.c
	$(CC) $(CFLAGS) $^ -o build/kilo -Iinclude
