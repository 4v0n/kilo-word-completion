CFLAGS = -Wall -Wextra -pedantic -std=c99 -g
SRCS = src/main.c \
      src/terminal.c \
      src/input.c \
      src/output.c \
      src/io.c \
      src/row_operations.c \
      src/editor_operations.c \
      src/search.c \
      src/syntax_highlighting.c \
      src/trie.c

main: $(SRCS)
	$(CC) $(CFLAGS) $^ -o build/kilo -Iinclude